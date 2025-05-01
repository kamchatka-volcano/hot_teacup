#include "utils.h"
#include <hot_teacup/header_view.h>
#include <hot_teacup/multipart_form_view.h>
#include <sfun/string_utils.h>
#include <optional>

namespace http {

MultipartFormFieldView::MultipartFormFieldView(std::string_view name, std::string_view value)
    : name_{name}
    , value_{value}
{
}

MultipartFormFieldView::MultipartFormFieldView(
        std::string_view name,
        std::string_view fileData,
        std::string_view fileName,
        std::string_view fileType)
    : name_{name}
    , value_{FormFile{fileData, fileName, fileType}}
{
}

std::string_view MultipartFormFieldView::name() const
{
    return name_;
}

MultipartFormFieldType MultipartFormFieldView::type() const
{
    return std::holds_alternative<std::string_view>(value_) ? MultipartFormFieldType::Param
                                                            : MultipartFormFieldType::File;
}

std::string_view MultipartFormFieldView::fileName() const
{
    if (type() != MultipartFormFieldType::File)
        return {};

    return std::get<FormFile>(value_).fileName;
}

std::string_view MultipartFormFieldView::fileType() const
{
    if (type() != MultipartFormFieldType::File)
        return {};

    return std::get<FormFile>(value_).mimeType;
}

std::string_view MultipartFormFieldView::value() const
{
    if (type() == MultipartFormFieldType::Param)
        return std::get<std::string_view>(value_);

    return std::get<FormFile>(value_).fileData;
}

bool operator==(const MultipartFormFieldView& lhs, const MultipartFormFieldView& rhs)
{
    return lhs.type() == rhs.type() && lhs.value() == rhs.value() && lhs.fileName() == rhs.fileName() &&
            lhs.fileType() == rhs.fileType();
}

namespace {
/// Reads HTTP headers between two blank lines
/// Returns a tuple { Content-Disposition header if found,
///                   Content-Type header if found
///                 }
/// if input is not at the end and has a valid state
///
std::optional<std::tuple<std::optional<HeaderView>, std::optional<HeaderView>>> readContentHeaders(
        std::string_view input,
        std::size_t& pos)
{
    auto headerLine = utils::getStringLine(input, pos);
    if (!headerLine.empty() || pos == input.size())
        return {};
    else
        headerLine = utils::getStringLine(input, pos);

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
        headerLine = utils::getStringLine(input, pos);
    }
    return std::make_tuple(std::move(contentDisposition), std::move(contentType));
}

std::vector<MultipartFormFieldView> parseMultipartFormFieldViews(std::string_view input, std::string_view boundary)
{
    const auto separator = "--" + std::string{boundary};
    const auto endSeparator = "--" + std::string{boundary} + "--";

    auto pos = std::size_t{};
    auto firstSeparatorLine = utils::getStringLine(input, pos, separator);
    if (!firstSeparatorLine.empty()) //a form must start with a "--<boundary>" separator
        return {};

    auto result = std::vector<MultipartFormFieldView>{};
    while (pos < input.size()) {
        auto contentHeaders = readContentHeaders(input, pos);
        if (!contentHeaders)
            return result;

        auto& [contentDisposition, contentType] = *contentHeaders;
        auto content = utils::getStringLine(input, pos, separator);
        if (!contentDisposition.has_value() || !contentDisposition->hasParam("name") ||
            contentDisposition->param("name").empty())
            continue;

        if (content.size() >= 2)
            content.remove_suffix(2); //remove \r\n

        const auto& paramName = contentDisposition->param("name");
        if (contentDisposition->hasParam("filename")) {
            const auto& fileName = contentDisposition->param("filename");
            auto fileType = std::string_view{};
            if (contentType.has_value())
                fileType = contentType->value();
            result.emplace_back(paramName, content, fileName, fileType);
        }
        else
            result.emplace_back(paramName, content);
    }
    return result;
}
} //namespace

std::vector<MultipartFormFieldView> MultipartFormView::fields() const
{
    auto result = std::vector<MultipartFormFieldView>{};
    for (const auto& [name, fields] : getFields())
        for (const auto& field : fields)
            result.emplace_back(field);
    return result;
}

std::optional<MultipartFormView> multipartFormViewFromString(
        const HeaderView& contentType,
        std::string_view requestBody)
{
    if (contentType.name() != "Content-Type" || contentType.value() != "multipart/form-data" ||
        !contentType.hasParam("boundary"))
        return std::nullopt;

    const auto fields = parseMultipartFormFieldViews(requestBody, contentType.param("boundary"));
    if (fields.empty())
        return std::nullopt;
    return MultipartFormView{fields};
}

} //namespace http