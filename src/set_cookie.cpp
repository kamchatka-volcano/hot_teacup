#include "utils.h"
#include <hot_teacup/set_cookie.h>
#include <hot_teacup/set_cookie_view.h>
#include <sfun/functional.h>
#include <sfun/string_utils.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

namespace http {

SetCookie::SetCookie(const SetCookieView& cookieView)
    : SetCookie{Header{cookieView.asHeader()}}
{
}

void SetCookie::init(std::vector<detail::SetCookieArg>&& args)
{
    const auto processArg = [this](detail::SetCookieArg& arg){
        if (std::holds_alternative<CookieDomain>(arg))
            setDomain(std::move(std::get<CookieDomain>(arg).value));
        else if (std::holds_alternative<CookiePath>(arg))
            setPath(std::move(std::get<CookiePath>(arg).value));
        else if (std::holds_alternative<CookieMaxAge>(arg))
            setMaxAge(std::get<CookieMaxAge>(arg).value);
        else if (std::holds_alternative<CookieIsSecure>(arg))
            setSecure();
        else if (std::holds_alternative<CookieIsRemoved>(arg))
            setRemoved();
    };
    std::for_each(args.begin(), args.end(), processArg);
}

SetCookie::SetCookie(Header header)
    : header_(std::move(header))
{
}

std::string_view SetCookie::name() const
{
    return header_.params().at(0).name();
}

std::string_view SetCookie::value() const
{
    return header_.params().at(0).value();
}

std::optional<std::string_view> SetCookie::domain() const
{
    if (header_.hasParam("Domain"))
        return header_.param("Domain");
    else
        return {};
}

std::optional<std::string_view> SetCookie::path() const
{
    if (header_.hasParam("Path"))
        return header_.param("Path");
    else
        return {};
}

std::optional<std::chrono::seconds> SetCookie::maxAge() const
{
    if (header_.hasParam("Max-Age")) {
        return sfun::try_invoke([&]{ return std::chrono::seconds{std::stoi(std::string{header_.param("Max-Age")})}; });
    }
    else
        return std::nullopt;
}

bool SetCookie::isSecure() const
{
    return header_.hasParam("Secure");
}

bool SetCookie::isRemoved() const
{
    return header_.hasParam("Max-Age") && header_.param("Max-Age") == "0";
}

void SetCookie::setDomain(std::string domain)
{
    header_.setParam("Domain", std::move(domain));
}

void SetCookie::setPath(std::string path)
{
    header_.setParam("Path", std::move(path));
}

void SetCookie::setMaxAge(const std::chrono::seconds& maxAge)
{
    header_.setParam("Max-Age", std::to_string(maxAge.count()));
}

void SetCookie::setRemoved()
{
    header_.setParam("Max-Age", "0");
}

void SetCookie::setSecure()
{
    header_.setParam("Secure");
}

std::string SetCookie::toString() const
{
    return header_.toString();
}

SetCookieView SetCookie::toView() const
{
    return SetCookieView{header_.toView()};
}

bool SetCookie::isView() const
{
    return static_cast<const ICopyOnWrite&>(header_).isView();
}

void SetCookie::makeOwnStateFromView()
{
    if (!isView())
        return;

    static_cast<ICopyOnWrite&>(header_).makeOwnStateFromView();
}

bool operator==(const SetCookie& lhs, const SetCookie& rhs)
{
    return lhs.name() == rhs.name() && lhs.value() == rhs.value() && lhs.domain() == rhs.domain() &&
            lhs.path() == rhs.path() && lhs.maxAge() == rhs.maxAge() && lhs.isSecure() == rhs.isSecure() &&
            lhs.isRemoved() == rhs.isRemoved();
}

std::string setCookiesToString(const std::vector<SetCookie>& cookies)
{
    const auto cookieToString = [](const SetCookie& cookie){
        return sfun::join_strings(cookie.name(), "=", cookie.value());
    };
    const auto cookieFcgiStringList = utils::transform(cookies, cookieToString);
    return sfun::join(cookieFcgiStringList, "; ");
}

std::vector<SetCookie> makeSetCookies(const std::vector<SetCookieView>& cookieViewList)
{
    return utils::transform(
            cookieViewList,
            [](const SetCookieView& cookieView)
            {
                return SetCookie{cookieView};
            });
}

} //namespace http
