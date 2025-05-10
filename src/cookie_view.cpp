#include <hot_teacup/cookie_view.h>
#include <sfun/string_utils.h>

namespace http {

CookieView::CookieView(std::string_view name, std::string_view value)
    : header_{"Set-Cookie", "", {HeaderParamView{name, value}}}
{
}

CookieView::CookieView(HeaderView header)
    : header_{std::move(header)}
{
}

std::string_view CookieView::name() const
{
    return header_.params().at(0).name();
}

std::string_view CookieView::value() const
{
    return header_.params().at(0).value();
}

const HeaderView& CookieView::asHeader() const
{
    return header_;
}

bool operator==(const CookieView& lhs, const CookieView& rhs)
{
    return lhs.name() == rhs.name() && lhs.value() == rhs.value();
}

std::vector<CookieView> cookiesFromString(std::string_view input)
{
    auto result = std::vector<CookieView>{};
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

std::optional<CookieView> cookieFromHeader(const HeaderView& header)
{
    if (header.params().empty())
        return std::nullopt;

    return CookieView{header};
}

} //namespace http
