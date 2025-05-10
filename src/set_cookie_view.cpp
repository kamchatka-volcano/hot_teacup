#include <hot_teacup/set_cookie_view.h>
#include <sfun/string_utils.h>

namespace http {

SetCookieView::SetCookieView(std::string_view name, std::string_view value)
    : header_{"Set-Cookie", "", {HeaderParamView{name, value}}}
{
}

SetCookieView::SetCookieView(HeaderView header)
    : header_{std::move(header)}
{
}

std::string_view SetCookieView::name() const
{
    return header_.params().at(0).name();
}

std::string_view SetCookieView::value() const
{
    return header_.params().at(0).value();
}

std::optional<std::string_view> SetCookieView::domain() const
{
    if (header_.hasParam("Domain"))
        return header_.param("Domain");
    else
        return {};
}

std::optional<std::string_view> SetCookieView::path() const
{
    if (header_.hasParam("Path"))
        return header_.param("Path");
    else
        return {};
}

std::optional<std::chrono::seconds> SetCookieView::maxAge() const
{
    if (header_.hasParam("Max-Age")) {
        try {
            return std::chrono::seconds{std::stoi(std::string{header_.param("Max-Age")})};
        }
        catch (...) {
            return {};
        }
    }
    else
        return {};
}

bool SetCookieView::isSecure() const
{
    return header_.hasParam("Secure");
}

bool SetCookieView::isRemoved() const
{
    return header_.hasParam("Max-Age") && header_.param("Max-Age") == "0";
}

const HeaderView& SetCookieView::asHeader() const
{
    return header_;
}

bool operator==(const SetCookieView& lhs, const SetCookieView& rhs)
{
    return lhs.name() == rhs.name() && lhs.value() == rhs.value() && lhs.domain() == rhs.domain() &&
            lhs.path() == rhs.path() && lhs.maxAge() == rhs.maxAge() && lhs.isSecure() == rhs.isSecure() &&
            lhs.isRemoved() == rhs.isRemoved();
}

std::vector<SetCookieView> setCookiesFromString(std::string_view input)
{
    auto result = std::vector<SetCookieView>{};
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

std::optional<SetCookieView> setCookieFromHeader(const HeaderView& header)
{
    if (header.params().empty())
        return std::nullopt;

    return SetCookieView{header};
}

} //namespace http
