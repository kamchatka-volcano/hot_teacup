#include <hot_teacup/form_view.h>
#include <hot_teacup/header_view.h>
#include <sfun/string_utils.h>
#include <optional>

namespace http {
namespace str = sfun::string_utils;

FormFieldView::FormFieldView(std::string_view value)
    : value_{value}
{
}

FormFieldView::FormFieldView(
        std::string_view fileData,
        std::string_view fileName,
        std::optional<std::string_view> fileType)
    : value_{FormFile{fileData, fileName, fileType}}
{
}

FormFieldType FormFieldView::type() const
{
    return std::holds_alternative<std::string_view>(value_) ? FormFieldType::Param : FormFieldType::File;
}

bool FormFieldView::hasFile() const
{
    if (type() == FormFieldType::File && !std::get<FormFile>(value_).fileName.empty())
        return true;
    else
        return false;
}

std::string_view FormFieldView::fileName() const
{
    if (type() == FormFieldType::File)
        return std::get<FormFile>(value_).fileName;
    else
        return {};
}

std::string_view FormFieldView::fileType() const
{
    if (type() == FormFieldType::File) {
        const auto& res = std::get<FormFile>(value_).mimeType;
        return res ? *res : std::string_view{};
    }
    else
        return {};
}

std::string_view FormFieldView::value() const
{
    if (type() == FormFieldType::Param)
        return std::get<std::string_view>(value_);
    else
        return std::get<FormFile>(value_).fileData;
}

namespace {
std::string_view getStringLine(std::string_view input, std::size_t& pos, std::string_view lineSeparator = "\r\n")
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
    return input.substr(linePos, lineSize);
}

/// Reads HTTP headers between two blank lines
/// Returns a tuple { Content-Disposition header if found,
///                   Content-Type header if found
///                 }
/// if input is not at the end and has a valid state
///
std::optional<std::tuple<std::optional<HeaderView>, std::optional<HeaderView>>>
readContentHeaders(std::string_view input, std::size_t& pos)
{
    auto headerLine = getStringLine(input, pos);
    if (!headerLine.empty() || pos == input.size())
        return {};
    else
        headerLine = getStringLine(input, pos);

    auto contentDisposition = std::optional<HeaderView>{};
    auto contentType = std::optional<HeaderView>{};
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


FormView parseFormFieldViews(std::string_view input, std::string_view boundary)
{
    const auto separator = "--" + std::string{boundary};
    const auto endSeparator = "--" + std::string{boundary} + "--";

    auto pos = std::size_t{};
    auto firstSeparatorLine = getStringLine(input, pos, separator);
    if (!firstSeparatorLine.empty()) //a form must start with a "--<boundary>" separator
        return {};

    auto result = FormView{};
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
            content.remove_suffix(2); //remove \r\n

        const auto& paramName = contentDisposition->param("name");
        if (contentDisposition->hasParam("filename")) {
            const auto& fileName = contentDisposition->param("filename");
            auto fileType = std::optional<std::string_view>{};
            if (contentType.has_value())
                fileType = contentType->value();
            result.emplace(std::string{paramName}, FormFieldView{content, fileName, fileType});
        }
        else
            result.emplace(std::string{paramName}, FormFieldView{content});
    }
    return result;
}

std::tuple<std::string_view, std::string_view> parseUrlEncodedParamString(std::string_view paramStr)
{
    auto delimiterPos = paramStr.find('=');
    if (delimiterPos == std::string::npos)
        return {};
    auto name = str::trim(str::before(paramStr, "="));
    auto val = str::trimFront(str::after(paramStr, "="));
    return {name, val};
}


FormView parseUrlEncodedFields(std::string_view input)
{
    auto pos = std::size_t{0u};
    auto result = FormView{};
    do {
        auto param = getStringLine(input, pos, "&");
        auto [paramName, paramValue] = parseUrlEncodedParamString(param);
        if (paramName.empty())
            continue;
        result.emplace(paramName, FormFieldView{paramValue});
    } while (pos < input.size());
    return result;
}
}

FormView formFromString(std::string_view contentParam, std::string_view contentFields)
{
    auto contentTypeHeader = "Content-Type: " + std::string{contentParam};
    auto contentType = headerFromString(contentTypeHeader);
    if (!contentType.has_value())
        return {};

    if (contentType->value() == "multipart/form-data" && contentType->hasParam("boundary"))
        return parseFormFieldViews(contentFields, contentType->param("boundary"));
    else if (contentType->value() == "application/x-www-form-urlencoded")
        return parseUrlEncodedFields(contentFields);

    return {};
}

}