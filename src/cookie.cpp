#include <hot_teacup/cookie.h>
#include <sfun/string_utils.h>

namespace http {
namespace str = sfun::string_utils;

Cookie::Cookie(std::string name,
               std::string value)
        : header_("Set-Cookie", "")
{
    header_.setParam(std::move(name), std::move(value));
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
    }
    else
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
    if (!domain.empty())
        header_.setParam("Domain", std::move(domain));
}

void Cookie::setPath(std::string path)
{
    if (!path.empty())
        header_.setParam("Path", std::move(path));
}

void Cookie::setMaxAge(const std::chrono::seconds& maxAge)
{
    if (maxAge.count() > 0)
        header_.setParam("Max-Age", std::to_string(maxAge.count()));
}

void Cookie::remove()
{
    header_.setParam("Max-Age", "0");
}

void Cookie::secure()
{
    header_.setParam("Secure");
}

std::string Cookie::toString() const
{
    return header_.toString();
}

bool operator==(const Cookie& lhs, const Cookie& rhs)
{
    return lhs.name()== rhs.name() &&
           lhs.value() == rhs.value();
}

Cookies cookiesFromString(std::string_view input)
{
    auto result = Cookies{};
    std::vector<std::string> cookies = str::split(input, ";");
    for (const std::string& cookie: cookies) {
        auto name = str::before(cookie, "=");
        auto value = str::after(cookie, "=");
        if (!name.empty() && !value.empty())
            result.emplace_back(str::trim(name), str::trim(value));
    }
    return result;
}

std::string cookiesToString(const Cookies& cookies)
{
    auto result = std::string{};
    for (const auto& cookie: cookies)
        result += cookie.name() + "=" + cookie.value() + "; ";
    if (!result.empty())
        result.resize(result.size() - 2); //remove last '; '
    return result;
}

std::optional<Cookie> cookieFromHeader(const Header& header)
{
    if (header.params().empty())
        return std::nullopt;

    return Cookie{header};
}

}
