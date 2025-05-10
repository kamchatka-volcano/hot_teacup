#ifndef HOT_TEACUP_MULTIPART_FORM_H
#define HOT_TEACUP_MULTIPART_FORM_H

#include "multipart_form_impl.h"
#include "multipart_form_view.h"
#include "types.h"
#include <string>
#include <string_view>
#include <variant>

namespace http {

class MultipartFormField {
    struct FormFile {
        std::string fileData;
        std::string fileName;
        std::string mimeType;
    };
public:
    explicit MultipartFormField(const MultipartFormFieldView& fieldView);
    MultipartFormField(std::string name, std::string value);
    MultipartFormField(std::string name, std::string fileData, std::string fileName, std::string fileType = {});
    MultipartFormFieldType type() const;
    std::string_view name() const;
    std::string_view fileName() const;
    std::string_view fileType() const;
    std::string_view value() const;

    friend bool operator==(const MultipartFormField& lhs, const MultipartFormField& rhs);

private:
    std::string name_;
    std::variant<std::string, FormFile> value_;
};

class MultipartForm : detail::MultipartFormImpl<std::string, MultipartFormField> {
public:
    using MultipartFormImpl::MultipartFormImpl;
    MultipartForm(const MultipartFormView& fieldView);

    std::string toString() const;
    MultipartFormView toView() const;
};

} //namespace http

#endif //HOT_TEACUP_MULTIPART_FORM_H
