#include <hot_teacup/request_cookie.h>

#include "utils.h"
#include <sfun/string_utils.h>
#include <string>
#include <vector>

namespace http {

std::string requestCookiesToHeaderValueString(const std::vector<RequestCookie>& cookies)
{
    const auto cookieToString = [](const RequestCookie& cookie)
    {
        if (!cookie.hasValue())
            return std::string{cookie.name()};
        return sfun::join_strings(cookie.name(), "=", cookie.value());
    };
    const auto cookieFcgiStringList = utils::transform(cookies, cookieToString);
    return sfun::join(cookieFcgiStringList, "; ");
}

} //namespace http
