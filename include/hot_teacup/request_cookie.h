#ifndef HOT_TEACUP_COOKIE_H
#define HOT_TEACUP_COOKIE_H

#include "header.h"
#include "detail/param.h"
#include <optional>
#include <string>
#include <vector>

namespace http {
namespace detail {
struct RequestCookieTag;
}

using RequestCookie = detail::Param<detail::RequestCookieTag>;
using RequestCookieView = detail::ParamView<detail::RequestCookieTag>;

using RequestCookies = std::vector<RequestCookie>;

std::string requestCookiesToHeaderValueString(const std::vector<RequestCookie>& cookies);

} //namespace http

#endif //HOT_TEACUP_COOKIE_H
