#include <hot_teacup/response_cookie.h>

#include "utils.h"
#include <hot_teacup/response_cookie_view.h>
#include <sfun/functional.h>
#include <sfun/string_utils.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

namespace http {

ResponseCookie::ResponseCookie(const ResponseCookieView& cookieView)
    : ResponseCookie{Header{cookieView.asHeader()}}
{
}

void ResponseCookie::init(std::vector<detail::ResponseCookieArg>&& args)
{
    const auto processArg = [this](detail::ResponseCookieArg& arg)
    {
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

ResponseCookie::ResponseCookie(Header header)
    : header_(std::move(header))
{
}

std::string_view ResponseCookie::name() const
{
    return header_.params().at(0).name();
}

std::string_view ResponseCookie::value() const
{
    return header_.params().at(0).value();
}

std::optional<std::string_view> ResponseCookie::domain() const
{
    if (header_.hasParam("Domain"))
        return header_.param("Domain");
    else
        return {};
}

std::optional<std::string_view> ResponseCookie::path() const
{
    if (header_.hasParam("Path"))
        return header_.param("Path");
    else
        return {};
}

std::optional<std::chrono::seconds> ResponseCookie::maxAge() const
{
    if (header_.hasParam("Max-Age")) {
        return sfun::try_invoke(
                [&]
                {
                    return std::chrono::seconds{std::stoi(std::string{header_.param("Max-Age")})};
                });
    }
    else
        return std::nullopt;
}

bool ResponseCookie::isSecure() const
{
    return header_.hasParam("Secure");
}

bool ResponseCookie::isRemoved() const
{
    return header_.hasParam("Max-Age") && header_.param("Max-Age") == "0";
}

void ResponseCookie::setDomain(std::string domain)
{
    if (isView())
        makeOwnStateFromView();

    header_.setParam("Domain", std::move(domain));
}

void ResponseCookie::setPath(std::string path)
{
    if (isView())
        makeOwnStateFromView();

    header_.setParam("Path", std::move(path));
}

void ResponseCookie::setMaxAge(const std::chrono::seconds& maxAge)
{
    if (isView())
        makeOwnStateFromView();

    header_.setParam("Max-Age", std::to_string(maxAge.count()));
}

void ResponseCookie::setRemoved()
{
    if (isView())
        makeOwnStateFromView();

    header_.setParam("Max-Age", "0");
}

void ResponseCookie::setSecure()
{
    if (isView())
        makeOwnStateFromView();

    header_.setParam("Secure");
}

std::string ResponseCookie::toString() const
{
    return header_.toString();
}

bool ResponseCookie::isView() const
{
    return static_cast<const IViewOrOwner&>(header_).isView();
}

void ResponseCookie::makeOwnStateFromView()
{
    if (!isView())
        return;

    static_cast<IViewOrOwner&>(header_).makeOwnStateFromView();
}

bool operator==(const ResponseCookie& lhs, const ResponseCookie& rhs)
{
    return lhs.name() == rhs.name() && lhs.value() == rhs.value() && lhs.domain() == rhs.domain() &&
            lhs.path() == rhs.path() && lhs.maxAge() == rhs.maxAge() && lhs.isSecure() == rhs.isSecure() &&
            lhs.isRemoved() == rhs.isRemoved();
}

} //namespace http
