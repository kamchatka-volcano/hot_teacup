#ifndef HOT_TEACUP_URL_ENCODED_FORM_H
#define HOT_TEACUP_URL_ENCODED_FORM_H

#include "url_encoded_form_impl.h"
#include "url_encoded_form_view.h"
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace http {

struct UrlEncodedFormField {
    std::string name;
    std::string value;
};

class UrlEncodedForm : public detail::UrlEncodedFormImpl<std::string, UrlEncodedFormField> {
public:
    using UrlEncodedFormImpl::UrlEncodedFormImpl;
    UrlEncodedForm(const UrlEncodedFormView& formView);

    std::string toString() const;
    UrlEncodedFormView toView() const;
};

std::optional<UrlEncodedForm> urlEncodedFormFromString(std::string_view requestBody);

} //namespace http

#endif //HOT_TEACUP_URL_ENCODED_FORM_H
