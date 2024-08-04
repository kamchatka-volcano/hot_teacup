#include "utils.h"
#include <hot_teacup/cookie.h>
#include <hot_teacup/cookie_view.h>
#include <sfun/functional.h>
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

void Cookie::init(std::vector<detail::CookieArg>&& args)
{
    const auto processArg = [this](detail::CookieArg& arg){
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

std::optional<std::string_view> Cookie::domain() const
{
    if (header_.hasParam("Domain"))
        return header_.param("Domain");
    else
        return {};
}

std::optional<std::string_view> Cookie::path() const
{
    if (header_.hasParam("Path"))
        return header_.param("Path");
    else
        return {};
}

std::optional<std::chrono::seconds> Cookie::maxAge() const
{
    if (header_.hasParam("Max-Age")) {
        return sfun::try_invoke([&]{ return std::chrono::seconds{std::stoi(std::string{header_.param("Max-Age")})}; });
    }
    else
        return std::nullopt;
}

bool Cookie::isSecure() const
{
    return header_.hasParam("Secure");
}

bool Cookie::isRemoved() const
{
    return header_.hasParam("Max-Age") && header_.param("Max-Age") == "0";
}

void Cookie::setDomain(std::string domain)
{
    header_.setParam("Domain", std::move(domain));
}

void Cookie::setPath(std::string path)
{
    header_.setParam("Path", std::move(path));
}

void Cookie::setMaxAge(const std::chrono::seconds& maxAge)
{
    header_.setParam("Max-Age", std::to_string(maxAge.count()));
}

void Cookie::setRemoved()
{
    header_.setParam("Max-Age", "0");
}

void Cookie::setSecure()
{
    header_.setParam("Secure");
}

std::string Cookie::toString() const
{
    return header_.toString();
}

bool operator==(const Cookie& lhs, const Cookie& rhs)
{
    return lhs.name() == rhs.name() && lhs.value() == rhs.value() && lhs.domain() == rhs.domain() &&
            lhs.path() == rhs.path() && lhs.maxAge() == rhs.maxAge() && lhs.isSecure() == rhs.isSecure() &&
            lhs.isRemoved() == rhs.isRemoved();
}

std::string cookiesToString(const std::vector<Cookie>& cookies)
{
    const auto cookieToString = [](const Cookie& cookie){
        return sfun::join_strings(cookie.name(), "=", cookie.value());
    };
    const auto cookieFcgiStringList = utils::transform(cookies, cookieToString);
    return sfun::join(cookieFcgiStringList, "; ");
}

std::vector<Cookie> makeCookies(const std::vector<CookieView>& cookieViewList)
{
    return utils::transform(
            cookieViewList,
            [](const CookieView& cookieView)
            {
                return Cookie{cookieView};
            });
}

} //namespace http
