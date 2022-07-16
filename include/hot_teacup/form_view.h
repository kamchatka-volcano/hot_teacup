#pragma once
#include "types.h"
#include <string_view>
#include <string>
#include <vector>
#include <variant>
#include <map>

namespace http{

class FormFieldView{
    struct FormFile{
        std::string_view fileData;
        std::string_view fileName;
        std::optional<std::string_view> mimeType;
    };

public:
    explicit FormFieldView(std::string_view value = {});
    FormFieldView(std::string_view fileData, std::string_view fileName, std::optional<std::string_view> fileType = {});

    FormFieldType type() const;
    bool hasFile() const;

    std::string_view fileName() const;
    std::string_view fileType() const;
    std::string_view value() const;

private:
    std::variant<std::string_view, FormFile> value_;
};

using FormView = std::map<std::string, FormFieldView>;

FormView formFromString(std::string_view contentTypeHeader, std::string_view contentFields);

}

