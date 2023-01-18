#ifndef HOT_TEACUP_COOKIE_VIEW_H
#define HOT_TEACUP_COOKIE_VIEW_H

#include "header_view.h"
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace http {

class CookieView {

public:
    CookieView(std::string_view name, std::string_view value);

    std::string_view name() const;
    std::string_view value() const;
    std::optional<std::string_view> domain() const;
    std::optional<std::string_view> path() const;
    std::optional<std::chrono::seconds> maxAge() const;
    bool isSecure() const;
    bool isRemoved() const;
    const HeaderView& asHeader() const;

    friend bool operator==(const CookieView& lhs, const CookieView& rhs);
    friend std::optional<CookieView> cookieFromHeader(const HeaderView& header);

private:
    explicit CookieView(HeaderView header);

private:
    HeaderView header_;
};

bool operator==(const CookieView& lhs, const CookieView& rhs);
std::vector<CookieView> cookiesFromString(std::string_view input);
std::optional<CookieView> cookieFromHeader(const HeaderView& header);

} //namespace http

#endif //HOT_TEACUP_COOKIE_VIEW_H
