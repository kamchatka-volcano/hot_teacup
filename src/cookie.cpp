#include <hot_teacup/cookie.h>
#include <hot_teacup/cookie_view.h>
#include <sfun/string_utils.h>

namespace http {
namespace str = sfun::string_utils;

Cookie::Cookie(const CookieView& cookieView)
    : Cookie{Header{cookieView.asHeader()}}
{}

Cookie::Cookie(std::string name,
               std::string value,
               std::optional<std::string> domain,
               std::optional<std::string> path,
               std::optional<std::chrono::seconds> maxAge,
               bool secure,
               bool removed)
    : header_{"Set-Cookie", ""}
{
    header_.setParam(std::move(name), std::move(value));
    if (domain)
        setDomain(*domain);
    if (path)
        setPath(*path);
    if (maxAge)
        setMaxAge(*maxAge);
    if (secure)
        setSecure();
    if (removed)
        setRemoved();
}

Cookie::Cookie(Header header)
    : header_(std::move(header))
{
}

const std::string& Cookie::name() const
{
    return header_.params().at(0).name();
}

const std::string& Cookie::value() const
{
    return header_.params().at(0).value();
}

std::optional<std::string> Cookie::domain() const
{
    if (header_.hasParam("Domain"))
        return header_.param("Domain");
    else
        return {};
}

std::optional<std::string> Cookie::path() const
{
    if (header_.hasParam("Path"))
        return header_.param("Path");
    else
        return {};
}

std::optional<std::chrono::seconds> Cookie::maxAge() const
{
    if (header_.hasParam("Max-Age")) {
        try {
            return std::chrono::seconds{std::stoi(header_.param("Max-Age"))};
        } catch (...) {
            return {};
        }
    } else
        return {};
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
    return lhs.name() == rhs.name() &&
           lhs.value() == rhs.value();
}

std::string cookiesToString(const std::vector<Cookie>& cookies)
{
    auto result = std::string{};
    for (const auto& cookie: cookies)
        result += cookie.name() + "=" + cookie.value() + "; ";
    if (!result.empty())
        result.resize(result.size() - 2); //remove last '; '
    return result;
}

std::vector<Cookie> makeCookies(const std::vector<CookieView>& cookieViewList)
{
    std::vector<Cookie> result;
    std::transform(cookieViewList.begin(), cookieViewList.end(), std::back_inserter(result),
                   [](const auto& cookieView) {
                       return Cookie{cookieView};
                   });
    return result;
}

}
