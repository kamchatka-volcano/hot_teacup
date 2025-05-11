#include <hot_teacup/response_cookie_view.h>

#include <sfun/string_utils.h>

namespace http {

ResponseCookieView::ResponseCookieView(std::string_view name, std::string_view value)
    : header_{"Set-Cookie", "", {HeaderParamView{name, value}}}
{
}

ResponseCookieView::ResponseCookieView(HeaderView header)
    : header_{std::move(header)}
{
}

std::string_view ResponseCookieView::name() const
{
    return header_.params().at(0).name();
}

std::string_view ResponseCookieView::value() const
{
    return header_.params().at(0).value();
}

std::optional<std::string_view> ResponseCookieView::domain() const
{
    if (header_.hasParam("Domain"))
        return header_.param("Domain");
    else
        return {};
}

std::optional<std::string_view> ResponseCookieView::path() const
{
    if (header_.hasParam("Path"))
        return header_.param("Path");
    else
        return {};
}

std::optional<std::chrono::seconds> ResponseCookieView::maxAge() const
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

bool ResponseCookieView::isSecure() const
{
    return header_.hasParam("Secure");
}

bool ResponseCookieView::isRemoved() const
{
    return header_.hasParam("Max-Age") && header_.param("Max-Age") == "0";
}

const HeaderView& ResponseCookieView::asHeader() const
{
    return header_;
}

bool operator==(const ResponseCookieView& lhs, const ResponseCookieView& rhs)
{
    return lhs.name() == rhs.name() && lhs.value() == rhs.value() && lhs.domain() == rhs.domain() &&
            lhs.path() == rhs.path() && lhs.maxAge() == rhs.maxAge() && lhs.isSecure() == rhs.isSecure() &&
            lhs.isRemoved() == rhs.isRemoved();
}

std::optional<ResponseCookieView> responseCookieFromHeader(const HeaderView& header)
{
    if (header.params().empty())
        return std::nullopt;

    return ResponseCookieView{header};
}

} //namespace http
