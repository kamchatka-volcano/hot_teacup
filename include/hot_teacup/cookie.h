#ifndef HOT_TEACUP_COOKIE_H
#define HOT_TEACUP_COOKIE_H

#include "header.h"
#include "trait_utils.h"
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace http {
class CookieView;

struct CookieIsSecure {};
struct CookieIsRemoved {};
struct CookieDomain {
    std::string value;
};
struct CookiePath {
    std::string value;
};
struct CookieMaxAge {
    std::chrono::seconds value;
};

namespace detail{
using CookieArg = std::variant<CookieDomain, CookiePath, CookieMaxAge, CookieIsSecure, CookieIsRemoved>;
}

class Cookie {

public:
    explicit Cookie(const CookieView& cookieView);
    template<
            typename... TArgs,
            typename = std::enable_if_t<
                    ((!std::is_same_v<std::decay_t<TArgs>, CookieView> &&
                      !std::is_same_v<std::decay_t<TArgs>, Cookie>) &&
                     ...)>>
    Cookie(std::string name, std::string value, TArgs&&... args)
        : header_{"Set-Cookie", ""}
    {
        header_.setParam(std::move(name), std::move(value));

        static_assert(
                !detail::has_duplicate_v<detail::decay_to_string_view_t<TArgs>...>,
                "Response constructor arguments can't contain duplicate types");
        init({std::forward<TArgs>(args)...});
    }

    std::string_view name() const;
    std::string_view value() const;
    std::optional<std::string_view> domain() const;
    std::optional<std::string_view> path() const;
    std::optional<std::chrono::seconds> maxAge() const;
    bool isSecure() const;
    bool isRemoved() const;

    void setDomain(std::string domain);
    void setPath(std::string path);
    void setMaxAge(const std::chrono::seconds& maxAge);
    void setSecure();
    void setRemoved();

    std::string toString() const;
    friend bool operator==(const Cookie& lhs, const Cookie& rhs);

private:
    explicit Cookie(Header header);
    void init(std::vector<detail::CookieArg>&& args);

private:
    Header header_;
};

using Cookies = std::vector<Cookie>;

std::string cookiesToString(const std::vector<Cookie>& cookies);
std::vector<Cookie> makeCookies(const std::vector<CookieView>& cookieViewList);

} //namespace http

#endif //HOT_TEACUP_COOKIE_H
