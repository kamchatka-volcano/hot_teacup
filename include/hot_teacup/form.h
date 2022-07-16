#pragma once
#include "types.h"
#include <string_view>
#include <string>
#include <vector>
#include <variant>
#include <map>

namespace http{
class FormFieldView;

class FormField{
    struct FormFile{
        std::string fileData;
        std::string fileName;
        std::optional<std::string> mimeType;
    };

public:
    explicit FormField(const FormFieldView&);
    explicit FormField(std::string value = {});
    FormField(std::string fileData, std::string fileName, std::optional<std::string> fileType = {});

    FormFieldType type() const;
    bool hasFile() const;

    const std::string& fileName() const;
    const std::string& fileType() const;
    const std::string& value() const;

private:
    std::variant<std::string, FormFile> value_;
    static inline const std::string valueNotFound = {};
};

using Form = std::map<std::string, FormField>;
std::string multipartFormToString(const Form& form, const std::string& formBoundary);
std::string urlEncodedFormToString(const Form& form);

Form makeForm(const std::map<std::string, FormFieldView>& formView);

}

