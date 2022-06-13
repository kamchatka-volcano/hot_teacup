#include <hot_teacup/form.h>
#include <hot_teacup/header.h>
#include <sfun/string_utils.h>
#include <optional>

namespace http {
namespace str = sfun::string_utils;

FormField::FormField(std::string value)
        : value_{std::move(value)}
{
}

FormField::FormField(std::string fileData, std::string fileName, std::optional<std::string> fileType)
        : value_{FormFile{std::move(fileData),
                          std::move(fileName),
                          std::move(fileType)}}
{
}

FormFieldType FormField::type() const
{
    return std::holds_alternative<std::string>(value_) ? FormFieldType::Param : FormFieldType::File;
}

bool FormField::hasFile() const
{
    if (type() == FormFieldType::File && !std::get<FormFile>(value_).fileName.empty())
        return true;
    else
        return false;
}

const std::string& FormField::fileName() const
{
    if (type() == FormFieldType::File)
        return std::get<FormFile>(value_).fileName;
    else
        return valueNotFound;
}

const std::string& FormField::fileType() const
{
    if (type() == FormFieldType::File) {
        const auto& res = std::get<FormFile>(value_).mimeType;
        return res ? *res : valueNotFound;
    } else
        return valueNotFound;
}

const std::string& FormField::value() const
{
    if (type() == FormFieldType::Param)
        return std::get<std::string>(value_);
    else
        return std::get<FormFile>(value_).fileData;
}

namespace {
std::string getStringLine(std::string_view input, std::size_t& pos, std::string_view lineSeparator = "\r\n")
{
    auto lastPos = input.find(lineSeparator, pos);
    auto separatorSize = lineSeparator.size();
    if (lastPos == std::string::npos) {
        lastPos = input.size();
        separatorSize = 0;
    }
    auto lineSize = lastPos - pos;
    auto linePos = pos;
    pos += lineSize + separatorSize;
    return std::string{input.substr(linePos, lineSize)};
}

/// Reads HTTP headers between two blank lines
/// Returns a tuple { Content-Disposition header if found,
///                   Content-Type header if found
///                 }
/// if input is not at the end and has a valid state
///
std::optional<std::tuple<std::optional<Header>, std::optional<Header>>>
readContentHeaders(std::string_view input, std::size_t& pos)
{
    auto headerLine = getStringLine(input, pos);
    if (!headerLine.empty() || pos == input.size())
        return {};
    else
        headerLine = getStringLine(input, pos);

    auto contentDisposition = std::optional<Header>{};
    auto contentType = std::optional<Header>{};
    while (!headerLine.empty()) {
        auto header = headerFromString(headerLine);
        if (header.has_value()) {
            if (header->name() == "Content-Disposition")
                contentDisposition = std::move(header);
            else if (header->name() == "Content-Type")
                contentType = std::move(header);
        }
        headerLine = getStringLine(input, pos);
    }
    return std::make_tuple(std::move(contentDisposition), std::move(contentType));
}


Form parseFormFields(std::string_view input, const std::string& boundary)
{
    const auto separator = "--" + boundary;
    const auto endSeparator = "--" + boundary + "--";

    auto pos = std::size_t{0};
    auto firstSeparatorLine = getStringLine(input, pos, separator);
    if (!firstSeparatorLine.empty()) //a form must start with a "--<boundary>" separator
        return {};

    auto result = Form{};
    while (pos < input.size()) {
        auto contentHeaders = readContentHeaders(input, pos);
        if (!contentHeaders)
            return result;

        auto& [contentDisposition, contentType] = *contentHeaders;
        auto content = getStringLine(input, pos, separator);
        if (!contentDisposition.has_value() ||
            !contentDisposition->hasParam("name") ||
            contentDisposition->param("name").empty())
            continue;

        if (content.size() >= 2)
            content.resize(content.size() - 2); //remove \r\n

        const auto& paramName = contentDisposition->param("name");
        if (contentDisposition->hasParam("filename")) {
            const auto& fileName = contentDisposition->param("filename");
            auto fileType = std::optional<std::string>{};
            if (contentType.has_value())
                fileType = contentType->value();
            result.emplace(paramName, FormField{std::move(content), fileName, std::move(fileType)});
        } else
            result.emplace(paramName, FormField{std::move(content)});
    }
    return result;
}

std::tuple<std::string, std::string> parseUrlEncodedParamString(std::string_view paramStr)
{
    auto delimiterPos = paramStr.find('=');
    if (delimiterPos == std::string::npos)
        return {};
    auto name = str::trim(str::before(paramStr, "="));
    auto val = str::trimFront(str::after(paramStr, "="));
    return {std::move(name), std::move(val)};
}


Form parseUrlEncodedFields(std::string_view input)
{
    auto pos = std::size_t{0u};
    auto result = Form{};
    do {
        auto param = getStringLine(input, pos, "&");
        auto [paramName, paramValue] = parseUrlEncodedParamString(param);
        if (paramName.empty())
            continue;
        result.emplace(paramName, FormField{std::move(paramValue)});
    } while (pos < input.size());
    return result;
}
}

Form formFromString(std::string_view contentParam, std::string_view contentFields)
{
    auto contentTypeHeader = "Content-Type: " + std::string{contentParam};
    auto contentType = headerFromString(contentTypeHeader);
    if (!contentType.has_value())
        return {};

    if (contentType->value() == "multipart/form-data" && contentType->hasParam("boundary"))
        return parseFormFields(contentFields, contentType->param("boundary"));
    else if (contentType->value() == "application/x-www-form-urlencoded")
        return parseUrlEncodedFields(contentFields);

    return {};
}

std::string urlEncodedFormToString(const Form& form)
{
    auto result = std::string{};
    for (const auto& [name, field] : form) {
        if (!result.empty())
            result += "&";
        result += name + "=" + (field.type() == http::FormFieldType::Param ? field.value() : field.fileName());
    }
    return result;
}

std::string multipartFormToString(const Form& form, const std::string& formBoundary)
{
    auto result = std::string{};
    for (const auto& [name, field]: form) {
        result += "--" + formBoundary + "\r\n";
        auto header = http::Header{"Content-Disposition", "form-data"};
        header.setQuotingMode(http::Header::QuotingMode::ParamValue);
        header.setParam("name", name);
        if (field.type() == FormFieldType::Param)
            result += header.toString() + "\r\n\r\n" + field.value() + "\r\n";
        else {
            header.setParam("filename", field.fileName());
            auto fileHeader = std::optional<http::Header>{};
            if (!field.fileType().empty())
                fileHeader = http::Header{"Content-Type", field.fileType()};

            result += header.toString() + "\r\n";
            if (fileHeader)
                result += fileHeader->toString() + "\r\n";
            result += "\r\n";
            result += field.value() + "\r\n";
        }
    }
    result += "--" + formBoundary + "--\r\n";
    return result;
}

}