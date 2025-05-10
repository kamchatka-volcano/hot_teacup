#ifndef HOT_TEACUP_COOKIE_H
#define HOT_TEACUP_COOKIE_H

#include "header.h"
#include "trait_utils.h"
#include "detail/copy_on_write_interface.h"
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

class Cookie : public detail::ICopyOnWrite {

public:
    explicit Cookie(const CookieView& cookieView);
    Cookie(std::string name, std::string value);

    std::string_view name() const;
    std::string_view value() const;

    std::string toString() const;
    friend bool operator==(const Cookie& lhs, const Cookie& rhs);

private:
    bool isView() const override;
    void makeOwnStateFromView() override;

private:
    explicit Cookie(Header header);

private:
    Header header_;
};

using Cookies = std::vector<Cookie>;

std::string cookiesToString(const std::vector<Cookie>& cookies);
std::vector<Cookie> makeCookies(const std::vector<CookieView>& cookieViewList);

} //namespace http

#endif //HOT_TEACUP_COOKIE_H
