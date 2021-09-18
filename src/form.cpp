#include <hot_teacup/form.h>
#include <hot_teacup/header.h>

namespace http{

void FormField::setContentType(Header header)
{
    contentType_ = std::move(header);
}

void FormField::setContentDisposition(Header header)
{
    contentDisposition_ = std::move(header);
}

void FormField::setValue(std::string value)
{
    value_ = std::move(value);
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
    return contentType_.value();
}

const std::string& FormField::value() const
{
    return value_;
}

Form::Form(const std::string& contentTypeHeader, const std::string& contentFields)
{
    auto contentType = Header{};
    contentType.fromString(contentTypeHeader);
    if (contentType.value() == "multipart/form-data")
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
        auto field = FormField{};
        auto headerLine = getStringLine(input, pos);
        if (headerLine.empty())
            continue;
        if (headerLine == "--")
            return;
        while (!headerLine.empty()){
            auto header = Header{};
            header.fromString(headerLine);
            if (header.name() == "Content-Disposition")
                field.setContentDisposition(std::move(header));
            else if (header.name() == "Content-Type")
                field.setContentType(std::move(header));

            headerLine = getStringLine(input, pos);
        }
        auto content = getStringLine(input, pos, separator);
        content.resize(content.size() - 2); //remove \r\n
        field.setValue(std::move(content));

        if (!field.name().empty())
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
        auto field = FormField{};
        field.setValue(paramValue);
        auto header = Header{};
        header.setParam("name", std::move(paramName));
        field.setContentDisposition(std::move(header));
        fields_.emplace_back(std::move(field));
    } while (pos < input.size());
}

}
