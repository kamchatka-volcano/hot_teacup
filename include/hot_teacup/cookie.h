#ifndef HOT_TEACUP_COOKIE_H
#define HOT_TEACUP_COOKIE_H

#include "header.h"
#include "detail/view_or_owner_interface.h"
#include <string>
#include <string_view>
#include <vector>

namespace http {
class CookieView;

class Cookie : public detail::IViewOrOwner {

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

} //namespace http

#endif //HOT_TEACUP_COOKIE_H
