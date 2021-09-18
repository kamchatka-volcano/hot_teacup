#pragma once
#include <hot_teacup/header.h>
#include <string>
#include <vector>

namespace http{

class FormField{
public:
    void setContentDisposition(Header header);
    void setContentType(Header header);
    void setValue(std::string value);

    bool isFile() const;
    bool hasFile() const;
    const std::string& name() const;
    const std::string& fileName() const;
    const std::string& fileType() const;
    const std::string& value() const;

private:
    Header contentDisposition_;
    Header contentType_;
    std::string value_;
};

class Form
{
public:
    Form(const std::string& contentTypeHeader, const std::string& contentFields);
    const std::vector<FormField>& fields() const;

private:
    void parseFormFields(const std::string& input, const std::string& boundary);
    void parseUrlEncodedFields(const std::string& input);

private:
    std::vector<FormField> fields_;
};

}
