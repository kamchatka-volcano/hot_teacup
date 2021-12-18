#pragma once
#include <string>
#include "cookie.h"
#include "header.h"
#include "query.h"
#include "types.h"

namespace http{

class Response{
public:
    Response(std::string data);
    Response(ResponseStatus status,
             std::string body = {},
             Cookies cookies = {},
             Headers headers = {});

    ResponseStatus status() const;
    const std::string& body() const;
    const Cookies& cookies() const;
    const Headers& headers() const;
    std::string data() const;

    void addCookie(Cookie cookie);
    void addHeader(Header header);
    void addCookies(const Cookies& cookies);
    void addHeaders(const Headers& headers);

public:
    static Response Content(std::string text,
                            ContentType contentType = ContentType::HTML,
                            const Cookies& cookies = {},
                            const Headers& headers = {});

    static Response Redirect(const std::string& path,
                             RedirectType type = RedirectType::Found,
                             const Queries& queries = {},
                             const Cookies& cookies = {},
                             const Headers& headers = {});

    static Response Raw(std::string value);

private:
    std::string statusData() const;
    std::string cookiesData() const;
    std::string headersData() const;

private:
    ResponseStatus status_ = ResponseStatus::Code_404_Not_Found;
    std::string body_;
    Cookies cookies_;
    Headers headers_;

private:
    struct RawResponse{
        std::string data;
    } rawResponse_;
    Response(RawResponse);
};

}
