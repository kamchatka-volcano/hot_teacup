#include <hot_teacup/form.h>
#include <hot_teacup/header.h>
#include <sfun/string_utils.h>
#include <optional>

namespace http{
namespace str = sfun::string_utils;

FormField::FormField(Header contentDispositionHeader, Header contentTypeHeader, std::string value)
        : contentDisposition_(std::move(contentDispositionHeader))
        , contentType_(std::move(contentTypeHeader))
        , value_(std::move(value))
{
}

FormField::FormField(Header contentDispositionHeader, std::string value)
    : contentDisposition_(std::move(contentDispositionHeader))
    , value_(std::move(value))
{
}

FormField::Type FormField::type() const
{
    if (contentDisposition_.hasParam("filename"))
        return Type::File;
    else
        return Type::Param;
}

bool FormField::hasFile() const
{
    return contentDisposition_.hasParam("filename") && !contentDisposition_.param("filename").empty();
}

const std::string& FormField::name() const
{
    return contentDisposition_.param("name");
}

const std::string& FormField::fileName() const
{
    return contentDisposition_.param("filename");
}

const std::string& FormField::fileType() const
{
    if (!contentType_.has_value())
        return valueNotFound;

    return contentType_->value();
}

const std::string& FormField::value() const
{
    return value_;
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
/// Returns a tuple { the end of the form or invalid input state,
///                   Content-Disposition header if found,
///                   Content-Type header if found
///                 }
std::tuple<bool, std::optional<Header>, std::optional<Header>>
    readContentHeaders(std::string_view input, std::size_t& pos)
{
    auto headerLine = getStringLine(input, pos);
    if (!headerLine.empty() || pos == input.size())
        return {true, {}, {}};
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
    return {false, std::move(contentDisposition), std::move(contentType)};
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
    while (pos < input.size()){
        auto [atEnd, contentDisposition, contentType] = readContentHeaders(input, pos);
        if (atEnd)
            return result;

        auto content = getStringLine(input, pos, separator);
        if (!contentDisposition.has_value() ||
            !contentDisposition->hasParam("name") ||
            contentDisposition->param("name").empty())
            continue;

        if (content.size() >= 2)
            content.resize(content.size() - 2); //remove \r\n

        if (!contentType.has_value())
            result.emplace_back(FormField{std::move(*contentDisposition), std::move(content)});
        else
            result.emplace_back(FormField{std::move(*contentDisposition),
                                          std::move(*contentType),
                                          std::move(content)});
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
        auto[paramName, paramValue] = parseUrlEncodedParamString(param);
        if (paramName.empty())
            continue;
        auto header = Header{"Content-Disposition", ""};
        header.setParam("name", std::move(paramName));
        result.emplace_back(FormField{std::move(header), std::move(paramValue)});
    } while (pos < input.size());
    return result;
}
}

Form formFromString(std::string_view contentTypeHeader, std::string_view contentFields)
{
    auto contentType = headerFromString(contentTypeHeader);
    if (!contentType.has_value())
        return {};

    if (contentType->value() == "multipart/form-data" && contentType->hasParam("boundary"))
        return parseFormFields(contentFields, contentType->param("boundary"));
    else if (contentType->value() == "application/x-www-form-urlencoded")
        return parseUrlEncodedFields(contentFields);

    return {};
}

}
