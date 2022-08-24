#pragma once
#include "cookie.h"
#include "header.h"
#include "query.h"
#include "types.h"
#include <string>

namespace http{
class ResponseView;

class Response{
    struct RawResponse{
        std::string data;
    };
    explicit Response(RawResponse);

public:
    explicit Response(const ResponseView&);
    Response(std::string data);
    Response(ResponseStatus status,
             std::string body = {},
             std::vector<Cookie> cookies = {},
             std::vector<Header> headers = {});

    ResponseStatus status() const;
    const std::string& body() const;
    const std::vector<Cookie>& cookies() const;
    const std::vector<Header>& headers() const;
    std::string data(ResponseMode mode = ResponseMode::Standard) const;

    void setBody(const std::string& body);
    void addCookie(Cookie cookie);
    void addHeader(Header header);
    void addCookies(const std::vector<Cookie>& cookies);
    void addHeaders(const std::vector<Header>& headers);

public:
    static Response Content(std::string text,
                            ContentType contentType = ContentType::HTML,
                            const std::vector<Cookie>& cookies = {},
                            const std::vector<Header>& headers = {});

    static Response Redirect(const std::string& path,
                             RedirectType type = RedirectType::Found,
                             const std::vector<Query>& queries = {},
                             const std::vector<Cookie>& cookies = {},
                             const std::vector<Header>& headers = {});

    static Response Raw(std::string value);

private:
    std::string statusData(ResponseMode mode) const;
    std::string cookiesData() const;
    std::string headersData() const;

private:
    ResponseStatus status_ = ResponseStatus::Code_404_Not_Found;
    std::string body_;
    std::vector<Cookie> cookies_;
    std::vector<Header> headers_;
    std::optional<RawResponse> rawResponse_;
};

}
