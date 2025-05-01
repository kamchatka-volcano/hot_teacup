#ifndef HOT_TEACUP_MULTIPART_FORM_VIEW_H
#define HOT_TEACUP_MULTIPART_FORM_VIEW_H

#include "header_view.h"
#include "multipart_form_impl.h"
#include "types.h"
#include <optional>
#include <string_view>
#include <variant>

namespace http {
class MultipartFormFieldView {
    struct FormFile {
        std::string_view fileData;
        std::string_view fileName;
        std::string_view mimeType;
    };

public:
    MultipartFormFieldView(std::string_view name, std::string_view value);
    MultipartFormFieldView(
            std::string_view name,
            std::string_view fileData,
            std::string_view fileName,
            std::string_view fileType = {});
    std::string_view name() const;
    MultipartFormFieldType type() const;
    std::string_view fileName() const;
    std::string_view fileType() const;
    std::string_view value() const;

    friend bool operator==(const MultipartFormFieldView& lhs, const MultipartFormFieldView& rhs);

private:
    std::string_view name_;
    std::variant<std::string_view, FormFile> value_;
};

class MultipartFormView : public detail::MultipartFormImpl<std::string_view, MultipartFormFieldView> {
public:
    using MultipartFormImpl::MultipartFormImpl;

    std::vector<MultipartFormFieldView> fields() const;
};

std::optional<MultipartFormView> multipartFormViewFromString(
        const HeaderView& contentType,
        std::string_view requestBody);

} //namespace http

#endif //HOT_TEACUP_MULTIPART_FORM_VIEW_H
