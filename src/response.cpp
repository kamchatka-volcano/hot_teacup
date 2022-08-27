#include <hot_teacup/response.h>
#include <hot_teacup/response_view.h>
#include <algorithm>
#include <utility>
#include <sstream>

namespace http{

Response::Response(const ResponseView& responseView)
    : Response{responseView.status(),
               std::string{responseView.body()},
               makeCookies(responseView.cookies()),
               makeHeaders(responseView.headers())}
{}

Response::Response(std::string data)
    : Response{ResponseStatus::Code_200_Ok,
               std::move(data),
               {},
               {{"ContentType", detail::contentTypeToString(ContentType::HTML)}}}
{}

Response::Response(ResponseStatus status,
                   std::string body,
                   std::vector<Cookie> cookies,
                   std::vector<Header> headers)
    : status_{status}
    , body_{std::move(body)}
    , cookies_{std::move(cookies)}
    , headers_{std::move(headers)}
{
}

ResponseStatus Response::status() const
{
    return status_;
}

const std::string& Response::body() const
{
    return body_;
}

const std::vector<Cookie>& Response::cookies() const
{
    return cookies_;
}

const std::vector<Header>& Response::headers() const
{
    return headers_;
}

void Response::setBody(const std::string& body)
{
    body_ = body;
}

void Response::addCookie(Cookie cookie)
{
    cookies_.emplace_back(std::move(cookie));
}

void Response::addHeader(Header header)
{
    headers_.emplace_back(std::move(header));
}

void Response::addCookies(const std::vector<Cookie>& cookies)
{
    std::copy(cookies.begin(), cookies.end(), std::back_inserter(cookies_));
}

void Response::addHeaders(const std::vector<Header>& headers)
{
    std::copy(headers.begin(), headers.end(), std::back_inserter(headers_));
}

std::string Response::statusData(ResponseMode mode) const
{
    auto res = "HTTP/1.1 " + std::string{detail::statusToString(status_)} + "\r\n";
    if (mode == ResponseMode::CGI)
        res += "Status: " + std::string{detail::statusToString(status_)} + "\r\n";
    return res;
}

std::string Response::cookiesData() const
{
    auto result = std::string{};
    for (const auto& cookie : cookies_)
        result += cookie.toString() + "\r\n";
    return result;
}

std::string Response::headersData() const
{
    auto result = std::string{};
    for (auto& header : headers_)
        result += header.toString() + "\r\n";
    return result;
}

std::string Response::data(ResponseMode mode) const
{
    if (rawResponse_)
        return rawResponse_->data;

    return statusData(mode) +
           headersData() +
           cookiesData() +
           "\r\n" + body_;
}

Response Response::Redirect(const std::string &path,
                            RedirectType type,
                            const std::vector<Query>& queries,
                            const std::vector<Cookie>& cookies,
                            const std::vector<Header>& headers)
{
    auto responseValue = Response{detail::redirectTypeStatus(type),
                                  {},
                                  cookies,
                                  {{"Location", pathWithQueries(path, queries)}}
    };
    responseValue.addHeaders(headers);
    return responseValue;
}

Response Response::Content(std::string text,
                           ContentType contentType,
                           const std::vector<Cookie>& cookies,
                           const std::vector<Header>& headers)
{
    return Response::Content(std::move(text), detail::contentTypeToString(contentType), cookies, headers);
}

Response Response::Content(std::string text,
                           std::string contentType,
                           const std::vector<Cookie>& cookies,
                           const std::vector<Header>& headers)
{
    auto responseValue = Response{ResponseStatus::Code_200_Ok,
                                  std::move(text),
                                  cookies,
                                  {{"ContentType", std::move(contentType)}}
    };

    responseValue.addHeaders(headers);
    return responseValue;
}


Response::Response(RawResponse rawResponse)
    : rawResponse_(std::move(rawResponse))
{
}

Response Response::Raw(std::string value)
{
    return Response{RawResponse{std::move(value)}};
}


}

