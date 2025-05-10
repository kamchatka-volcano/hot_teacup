#include <hot_teacup/cookie.h>

#include "utils.h"
#include <hot_teacup/cookie_view.h>
#include <sfun/string_utils.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

namespace http {

Cookie::Cookie(const CookieView& cookieView)
    : Cookie{Header{cookieView.asHeader()}}
{
}

Cookie::Cookie(std::string name, std::string value)
    : header_{"Cookie", ""}
{
    header_.setParam(std::move(name), std::move(value));
}

Cookie::Cookie(Header header)
    : header_(std::move(header))
{
}

std::string_view Cookie::name() const
{
    return header_.params().at(0).name();
}

std::string_view Cookie::value() const
{
    return header_.params().at(0).value();
}

std::string Cookie::toString() const
{
    return header_.toString();
}

bool Cookie::isView() const
{
    return static_cast<const IViewOrOwner&>(header_).isView();
}

void Cookie::makeOwnStateFromView()
{
    if (!isView())
        return;

    static_cast<IViewOrOwner&>(header_).makeOwnStateFromView();
}

bool operator==(const Cookie& lhs, const Cookie& rhs)
{
    return lhs.name() == rhs.name() && lhs.value() == rhs.value();
}

std::string cookiesToString(const std::vector<Cookie>& cookies)
{
    const auto cookieToString = [](const Cookie& cookie){
        return sfun::join_strings(cookie.name(), "=", cookie.value());
    };
    const auto cookieFcgiStringList = utils::transform(cookies, cookieToString);
    return sfun::join(cookieFcgiStringList, "; ");
}

} //namespace http
