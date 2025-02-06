#ifndef HOT_TEACUP_FORM_VIEW_H
#define HOT_TEACUP_FORM_VIEW_H

#include "types.h"
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

namespace http {
class FormViewString {
public:
    struct OwningTag {};

public:
    FormViewString();
    template<typename T, std::enable_if_t<!std::is_same_v<T, std::string&&>>* = nullptr>
    FormViewString(T&& value)
        : value_{std::string_view{std::forward<T>(value)}}
    {
    }

    FormViewString(std::string&&);
    std::string_view data() const;
    operator std::string_view() const;
    friend bool operator<(const FormViewString& lhs, const FormViewString& rhs);
    friend bool operator==(const FormViewString& lhs, const FormViewString& rhs);
    bool operator==(std::string_view) const;

private:
    std::variant<std::string_view, std::string> value_;
};
} //namespace http

namespace http {

class FormFieldView {
    struct FormFile {
        std::string_view fileData;
        std::string_view fileName;
        std::optional<std::string_view> mimeType;
    };

public:
    explicit FormFieldView(FormViewString value = {});
    FormFieldView(std::string_view fileData, std::string_view fileName, std::optional<std::string_view> fileType = {});

    FormFieldType type() const;
    bool hasFile() const;

    std::string_view fileName() const;
    std::string_view fileType() const;
    std::string_view value() const;

    friend bool operator==(const FormFieldView& lhs, const FormFieldView& rhs);

private:
    std::variant<FormViewString, FormFile> value_;
};

using FormView = std::map<FormViewString, FormFieldView>;

FormView formFromString(std::string_view contentTypeHeader, std::string_view contentFields);

} //namespace http

#endif //HOT_TEACUP_FORM_VIEW_H
