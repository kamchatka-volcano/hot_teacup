#ifndef HOT_TEACUP_SET_COOKIE_VIEW_H
#define HOT_TEACUP_SET_COOKIE_VIEW_H

#include "header_view.h"
#include <chrono>
#include <optional>
#include <string_view>
#include <vector>

namespace http {

class ResponseCookieView {

public:
    ResponseCookieView(std::string_view name, std::string_view value);
    explicit ResponseCookieView(HeaderView header);

    std::string_view name() const;
    std::string_view value() const;
    std::optional<std::string_view> domain() const;
    std::optional<std::string_view> path() const;
    std::optional<std::chrono::seconds> maxAge() const;
    bool isSecure() const;
    bool isRemoved() const;
    const HeaderView& asHeader() const;

    friend bool operator==(const ResponseCookieView& lhs, const ResponseCookieView& rhs);

private:
    HeaderView header_;
};

std::optional<ResponseCookieView> responseCookieFromHeader(const HeaderView& header);

} //namespace http

#endif //HOT_TEACUP_SET_COOKIE_VIEW_H
