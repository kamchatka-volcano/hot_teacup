#ifndef HOT_TEACUP_FORM_H
#define HOT_TEACUP_FORM_H

#include "types.h"
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace http {
class FormFieldView;

class FormField {
    struct FormFile {
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

} //namespace http

#endif //HOT_TEACUP_FORM_H
