#ifndef HOT_TEACUP_RESPONSE_H
#define HOT_TEACUP_RESPONSE_H

#include "cookie.h"
#include "header.h"
#include "query.h"
#include "types.h"
#include <string>

namespace http {
class ResponseView;

struct Redirect {
    std::string path;
    RedirectType type = RedirectType::Found;
};

class Response {
public:
    explicit Response(const ResponseView&);
    Response(
            ResponseStatus status,
            std::string body = {},
            std::vector<Cookie> cookies = {},
            std::vector<Header> headers = {});
    Response(
            ResponseStatus status,
            std::string body,
            ContentType contentType,
            std::vector<Cookie> cookies = {},
            std::vector<Header> headers = {});
    Response(
            ResponseStatus status,
            std::string body,
            std::string contentType,
            std::vector<Cookie> cookies = {},
            std::vector<Header> headers = {});
    Response(
            std::string body,
            ContentType contentType,
            std::vector<Cookie> cookies = {},
            std::vector<Header> headers = {});
    Response(
            std::string body,
            std::string contentType,
            std::vector<Cookie> cookies = {},
            std::vector<Header> headers = {});
    Response(std::string path, RedirectType type, std::vector<Cookie> cookies = {}, std::vector<Header> headers = {});
    Response(Redirect redirect, std::vector<Cookie> cookies = {}, std::vector<Header> headers = {});
    Response(std::string body, std::vector<Cookie> cookies = {}, std::vector<Header> headers = {});

    ResponseStatus status() const;
    const std::string& body() const;
    const std::vector<Cookie>& cookies() const;
    const std::vector<Header>& headers() const;
    std::string data(ResponseMode mode = ResponseMode::Http) const;

    void setBody(const std::string& body);
    void addCookie(Cookie cookie);
    void addHeader(Header header);
    void addCookies(const std::vector<Cookie>& cookies);
    void addHeaders(const std::vector<Header>& headers);

private:
    std::string statusData(ResponseMode mode) const;
    std::string cookiesData() const;
    std::string headersData() const;

private:
    ResponseStatus status_ = ResponseStatus::_404_Not_Found;
    std::string body_;
    std::vector<Cookie> cookies_;
    std::vector<Header> headers_;
};

} //namespace http

#endif //HOT_TEACUP_RESPONSE_H