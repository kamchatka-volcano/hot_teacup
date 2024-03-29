#ifndef HOT_TEACUP_FORM_VIEW_H
#define HOT_TEACUP_FORM_VIEW_H

#include "types.h"
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace http {

class FormFieldView {
    struct FormFile {
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

} //namespace http

#endif //HOT_TEACUP_FORM_VIEW_H
