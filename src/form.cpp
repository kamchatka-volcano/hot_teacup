#include <hot_teacup/form.h>
#include <hot_teacup/header.h>
#include <optional>

namespace http{

FormField::FormField(Header contentDispositionHeader, std::string value)
    : contentDisposition_(std::move(contentDispositionHeader))
    , value_(std::move(value))
{
}

void FormField::setFileType(std::string fileType)
{
    fileType_ = std::move(fileType);
}

bool FormField::isFile() const
{
    return contentDisposition_.hasParam("filename");
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
    return fileType_;
}

const std::string& FormField::value() const
{
    return value_;
}

Form::Form(const std::string& contentTypeHeader, const std::string& contentFields)
{
    auto contentType = headerFromString(contentTypeHeader);
    if (contentType.value() == "multipart/form-data" && contentType.hasParam("boundary"))
        parseFormFields(contentFields, contentType.param("boundary"));
    else if (contentType.value() == "application/x-www-form-urlencoded")
        parseUrlEncodedFields(contentFields);
}

const std::vector<FormField>& Form::fields() const
{
    return fields_;
}

namespace{
    std::string getStringLine(const std::string& input, std::size_t& pos, const std::string& lineSeparator = "\r\n")
    {
        auto lastPos = input.find(lineSeparator, pos);
        auto separatorSize = lineSeparator.size();
        if (lastPos == std::string::npos){
            lastPos = input.size();
            separatorSize = 0;
        }
        auto lineSize = lastPos - pos;
        auto linePos = pos;
        pos += lineSize + separatorSize;
        return input.substr(linePos, lineSize);
    }
}

void Form::parseFormFields(const std::string& input, const std::string& boundary)
{
    const auto separator = "--" + boundary;
    const auto endSeparator = "--" + boundary + "--";

    auto pos = std::size_t{0};
    getStringLine(input, pos);
    while (pos < input.size()){
        auto headerLine = getStringLine(input, pos);
        if (headerLine.empty())
            continue;
        if (headerLine == "--")
            return;

        auto contentDisposition = std::optional<Header>{};
        auto fileType = std::string{};
        while (!headerLine.empty()){
            auto header = headerFromString(headerLine);
            if (header.name() == "Content-Disposition")
                contentDisposition = std::move(header);
            else if (header.name() == "Content-Type")
                fileType = header.value();

            headerLine = getStringLine(input, pos);
        }

        auto content = getStringLine(input, pos, separator);
        content.resize(content.size() - 2); //remove \r\n

        if (!contentDisposition.has_value() ||
            !contentDisposition->hasParam("name") ||
            contentDisposition->param("name").empty())
            continue;

        auto field = FormField{std::move(*contentDisposition), std::move(content)};
        if (!fileType.empty())
            field.setFileType(fileType);
        fields_.emplace_back(std::move(field));
    }
}

namespace{
    std::tuple<std::string, std::string> parseUrlEncodedParamString(const std::string& paramStr)
    {
        auto delimiterPos = paramStr.find('=');
        if (delimiterPos == std::string::npos)
            return {};
        auto name = paramStr.substr(0, delimiterPos);
        auto val = paramStr.substr(delimiterPos + 1, paramStr.size() - delimiterPos - 1);
        return std::make_tuple(name, val);
    }
}

void Form::parseUrlEncodedFields(const std::string &input)
{
    auto pos = std::size_t{0u};
    do{
        auto param = getStringLine(input, pos, "&");
        auto [paramName, paramValue] = parseUrlEncodedParamString(param);
        if (paramName.empty())
            continue;
        auto header = Header{"Content-Disposition", ""};
        header.setParam("name", std::move(paramName));
        fields_.emplace_back(FormField{std::move(header), std::move(paramValue)});
    } while (pos < input.size());
}

}
