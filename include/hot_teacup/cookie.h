#pragma once
#include "header.h"
#include <string_view>
#include <string>
#include <vector>
#include <optional>
#include <chrono>

namespace http{

class Cookie{

public:
    Cookie(std::string name,
           std::string value);

    const std::string& name() const;
    const std::string& value() const;

    void setDomain(std::string domain);
    void setPath(std::string path);
    void setMaxAge(const std::chrono::seconds& maxAge);
    void secure();
    void remove();

    std::optional<std::string> domain() const;
    std::optional<std::string> path() const;
    std::optional<std::chrono::seconds> maxAge() const;
    bool isSecure() const;
    bool isRemoved() const;

    std::string toString() const;
    friend bool operator==(const Cookie& lhs, const Cookie& rhs);
    friend std::optional<Cookie> cookieFromHeader(const Header& header);

private:
    explicit Cookie(Header header);

private:
    Header header_;
};

using Cookies = std::vector<Cookie>;
bool operator==(const Cookie& lhs, const Cookie& rhs);
Cookies cookiesFromString(std::string_view input);
std::string cookiesToString(const Cookies& cookies);
std::optional<Cookie> cookieFromHeader(const Header& header);

}

