#ifndef HOT_TEACUP_URL_ENCODED_FORM_VIEW_H
#define HOT_TEACUP_URL_ENCODED_FORM_VIEW_H

#include "url_encoded_form_impl.h"
#include <string_view>
#include <vector>

namespace http {

struct UrlEncodedFormFieldView {
    std::string_view name;
    std::string_view value;
};

class UrlEncodedFormView : public detail::UrlEncodedFormImpl<std::string_view, UrlEncodedFormFieldView> {
public:
    using UrlEncodedFormImpl::UrlEncodedFormImpl;
    std::vector<UrlEncodedFormFieldView> fields() const;
};
} //namespace http

#endif //HOT_TEACUP_URL_ENCODED_FORM_VIEW_H
