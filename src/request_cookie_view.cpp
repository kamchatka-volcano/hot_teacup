#include <hot_teacup/request_cookie_view.h>
#include <sfun/string_utils.h>
#include <vector>

namespace http {

std::vector<RequestCookieView> requestCookiesFromHeaderValueString(std::string_view input)
{
    auto result = std::vector<RequestCookieView>{};
    auto cookies = sfun::split(input, ";");
    for (const auto& cookie : cookies) {
        const auto namePart = sfun::before(cookie, "=");
        const auto valuePart = sfun::after(cookie, "=");
        if (!namePart.has_value())
            continue;
        const auto name = sfun::trim(namePart.value());
        if (!name.empty())
            result.emplace_back(name, valuePart.value_or(""));
    }
    return result;
}

} //namespace http