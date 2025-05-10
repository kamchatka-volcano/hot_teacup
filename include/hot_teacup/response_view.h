#ifndef HOT_TEACUP_RESPONSE_VIEW_H
#define HOT_TEACUP_RESPONSE_VIEW_H

#include "set_cookie_view.h"
#include "header_view.h"
#include "types.h"
#include <string>

namespace http {

class ResponseView {
public:
    ResponseView(
            ResponseStatus status,
            std::string_view body = {},
            std::vector<SetCookieView> cookies = {},
            std::vector<HeaderView> headers = {});

    ResponseStatus status() const;
    std::string_view body() const;

    const std::vector<SetCookieView>& cookies() const;
    std::string_view cookieValue(std::string_view name) const;
    std::optional<SetCookieView> cookie(std::string_view name) const;
    bool hasCookie(std::string_view name) const;

    const std::vector<HeaderView>& headers() const;
    std::string_view headerValue(std::string_view name) const;
    std::optional<HeaderView> header(std::string_view name) const;
    bool hasHeader(std::string_view name) const;

    friend bool operator==(const ResponseView& lhs, const ResponseView& rhs);

private:
    ResponseStatus status_ = ResponseStatus::_404_Not_Found;
    std::string body_;
    std::vector<SetCookieView> cookies_;
    std::vector<HeaderView> headers_;
};

std::optional<ResponseView> responseFromString(std::string_view, ResponseMode mode = ResponseMode::Http);

} //namespace http

#endif //HOT_TEACUP_RESPONSE_VIEW_H
