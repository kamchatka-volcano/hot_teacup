#pragma once
#include <hot_teacup/header.h>
#include <string>
#include <vector>

namespace http{

class FormField{
public:
    enum class Type{
        Param,
        File
    };

public:
    FormField(Header contentDispositionHeader,
              Header contentTypeHeader,
              std::string value);

    FormField(Header contentDispositionHeader,
              std::string value);

    Type type() const;
    bool hasFile() const;
    const std::string& name() const;
    const std::string& fileName() const;
    const std::string& fileType() const;
    const std::string& value() const;

private:
    Header contentDisposition_;
    std::optional<Header> contentType_;
    std::string value_;

    static inline const std::string valueNotFound = {};
};

using Form = std::vector<FormField>;
Form formFromString(const std::string& contentTypeHeader, const std::string& contentFields);

}

