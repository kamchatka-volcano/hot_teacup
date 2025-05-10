#ifndef HOT_TEACUP_SET_COOKIE_H
#define HOT_TEACUP_SET_COOKIE_H

#include "header.h"
#include "trait_utils.h"
#include "detail/copy_on_write_interface.h"
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace http {
class SetCookieView;

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

namespace detail {
using SetCookieArg = std::variant<CookieDomain, CookiePath, CookieMaxAge, CookieIsSecure, CookieIsRemoved>;
}

class SetCookie : public detail::ICopyOnWrite {

public:
    explicit SetCookie(const SetCookieView& cookieView);
    template<
            typename... TArgs,
            typename = std::enable_if_t<
                    ((!std::is_same_v<std::decay_t<TArgs>, SetCookieView> &&
                      !std::is_same_v<std::decay_t<TArgs>, SetCookie>) &&
                     ...)>>
    SetCookie(std::string name, std::string value, TArgs&&... args)
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
    SetCookieView toView() const;
    friend bool operator==(const SetCookie& lhs, const SetCookie& rhs);

private:
    bool isView() const override;
    void makeOwnStateFromView() override;

private:
    explicit SetCookie(Header header);
    void init(std::vector<detail::SetCookieArg>&& args);

private:
    Header header_;
};

using Cookies = std::vector<SetCookie>;

std::string setCookiesToString(const std::vector<SetCookie>& cookies);
std::vector<SetCookie> makeSetCookies(const std::vector<SetCookieView>& cookieViewList);

} //namespace http

#endif //HOT_TEACUP_COOKIE_H
