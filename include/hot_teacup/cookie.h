#ifndef HOT_TEACUP_COOKIE_H
#define HOT_TEACUP_COOKIE_H

#include "header.h"
#include <string_view>
#include <string>
#include <vector>
#include <optional>
#include <chrono>

namespace http{
class CookieView;

class Cookie{

public:
    explicit Cookie(const CookieView& cookieView);
    Cookie(std::string name,
           std::string value,
           std::optional<std::string> domain = {},
           std::optional<std::string> path = {},
           std::optional<std::chrono::seconds> maxAge = {},
           bool secure  = false,
           bool removed = false);

    const std::string& name() const;
    const std::string& value() const;
    std::optional<std::string> domain() const;
    std::optional<std::string> path() const;
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

private:
    Header header_;
};

bool operator==(const Cookie& lhs, const Cookie& rhs);
std::string cookiesToString(const std::vector<Cookie>& cookies);

std::vector<Cookie> makeCookies(const std::vector<CookieView>& cookieViewList);

}

#endif //HOT_TEACUP_COOKIE_H
