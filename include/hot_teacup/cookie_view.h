#pragma once
#include "header_view.h"
#include <string_view>
#include <string>
#include <vector>
#include <optional>
#include <chrono>

namespace http{

class CookieView{

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

}

