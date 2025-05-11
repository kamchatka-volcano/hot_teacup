#ifndef HOT_TEACUP_REQUEST_COOKIE_VIEW_H
#define HOT_TEACUP_REQUEST_COOKIE_VIEW_H

#include "detail/param_view.h"
#include <vector>

namespace http {
namespace detail {
struct RequestCookieTag;
}

using RequestCookieView = detail::ParamView<detail::RequestCookieTag>;
std::vector<RequestCookieView> requestCookiesFromHeaderValueString(std::string_view input);

} //namespace http

#endif //HOT_TEACUP_REQUEST_COOKIE_VIEW_H
