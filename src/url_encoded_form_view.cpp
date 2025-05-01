#include "url_encoder.h"
#include "utils.h"
#include <hot_teacup/url_encoded_form_view.h>
#include <sfun/string_utils.h>
#include <algorithm>
#include <set>

namespace http {

std::vector<UrlEncodedFormFieldView> UrlEncodedFormView::fields() const
{
    auto result = std::vector<UrlEncodedFormFieldView>{};
    for (const auto& [name, values] : getFields()) {
        for (const auto& value : values)
            result.emplace_back(UrlEncodedFormFieldView{name, value});
    }
    return result;
}

} //namespace http