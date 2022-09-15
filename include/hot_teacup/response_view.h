#ifndef HOT_TEACUP_RESPONSE_VIEW_H
#define HOT_TEACUP_RESPONSE_VIEW_H

#include "cookie_view.h"
#include "header_view.h"
#include "types.h"
#include <string>

namespace http{

class ResponseView{
public:
    ResponseView(ResponseStatus status,
             std::string_view body = {},
             std::vector<CookieView> cookies = {},
             std::vector<HeaderView> headers = {});

    ResponseStatus status() const;
    std::string_view body() const;
    const std::vector<CookieView>& cookies() const;
    const std::vector<HeaderView>& headers() const;

private:
    ResponseStatus status_ = ResponseStatus::Code_404_Not_Found;
    std::string body_;
    std::vector<CookieView> cookies_;
    std::vector<HeaderView> headers_;
};

std::optional<ResponseView> responseFromString(std::string_view);

}

#endif //HOT_TEACUP_RESPONSE_VIEW_H
