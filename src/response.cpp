#include <hot_teacup/response.h>
#include <hot_teacup/response_view.h>
#include <algorithm>
#include <iterator>
#include <utility>

namespace http {

Response::Response(const ResponseView& responseView)
    : status_{responseView.status()}
    , body_{responseView.body()}
    , cookies_{makeCookies(responseView.cookies())}
    , headers_{makeHeaders(responseView.headers())}
{
}

Response::Response(ResponseStatus status, std::string body, std::vector<Cookie> cookies, std::vector<Header> headers)
    : status_{status}
    , body_{std::move(body)}
    , cookies_{std::move(cookies)}
    , headers_{std::move(headers)}
{
    if (!std::get<std::string>(body_).empty()) {
        if (std::find_if(
                    headers_.begin(),
                    headers_.end(),
                    [](const Header& header)
                    {
                        return header.name() == "Content-Type";
                    }) == headers_.end())
            headers_.emplace_back("Content-Type", detail::contentTypeToString(ContentType::Html));
    }
}

namespace {
std::vector<Header> concatHeaders(std::vector<Header> lhs, const Header& rhs)
{
    lhs.push_back(rhs);
    return lhs;
}
} //namespace

Response::Response(
        ResponseStatus status,
        std::string body,
        std::string contentType,
        std::vector<Cookie> cookies,
        std::vector<Header> headers)
    : Response{
              status,
              std::move(body),
              std::move(cookies),
              concatHeaders(std::move(headers), {"Content-Type", std::move(contentType)})}
{
}

Response::Response(
        ResponseStatus status,
        std::string body,
        ContentType contentType,
        std::vector<Cookie> cookies,
        std::vector<Header> headers)
    : Response{
              status,
              std::move(body),
              detail::contentTypeToString(contentType),
              std::move(cookies),
              std::move(headers)}
{
}

Response::Response(std::string body, std::string contentType, std::vector<Cookie> cookies, std::vector<Header> headers)
    : Response{ResponseStatus::_200_Ok, std::move(body), std::move(contentType), std::move(cookies), std::move(headers)}
{
}

Response::Response(std::string body, ContentType contentType, std::vector<Cookie> cookies, std::vector<Header> headers)
    : Response{ResponseStatus::_200_Ok, std::move(body), contentType, std::move(cookies), std::move(headers)}
{
}

Response::Response(std::string body, std::vector<Cookie> cookies, std::vector<Header> headers)
    : Response{ResponseStatus::_200_Ok, std::move(body), std::move(cookies), std::move(headers)}
{
}

Response::Response(std::string path, RedirectType type, std::vector<Cookie> cookies, std::vector<Header> headers)
    : Response{detail::redirectTypeStatus(type), {}, std::move(cookies), std::move(headers)}
{
    addHeader({"Location", std::move(path)});
}

Response::Response(Redirect redirect, std::vector<Cookie> cookies, std::vector<Header> headers)
    : Response{std::move(redirect.path), redirect.type, std::move(cookies), std::move(headers)}
{
}

ResponseStatus Response::status() const
{
    return status_;
}

std::string_view Response::body() const
{
    return std::visit([](const auto& body) -> std::string_view{ return body;}, body_);
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
    if (isView())
        return;

    body_ = body;
}

void Response::addCookie(Cookie cookie)
{
    if (isView())
        return;

    cookies_.emplace_back(std::move(cookie));
}

void Response::addHeader(Header header)
{
    if (isView())
        return;

    headers_.emplace_back(std::move(header));
}

void Response::addCookies(const std::vector<Cookie>& cookies)
{
    if (isView())
        return;

    std::copy(cookies.begin(), cookies.end(), std::back_inserter(cookies_));
}

void Response::addHeaders(const std::vector<Header>& headers)
{
    if (isView())
        return;

    std::copy(headers.begin(), headers.end(), std::back_inserter(headers_));
}

std::string Response::statusData(ResponseMode mode) const
{
    return (mode == ResponseMode::Cgi ? "Status: " : "HTTP/1.1 ") + std::string{detail::statusToString(status_)} +
            "\r\n";
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
    const auto body = std::visit([](const auto& body) -> std::string_view{ return body;}, body_);
    return statusData(mode) + headersData() + cookiesData() + "\r\n" + std::string{body};
}

bool Response::isView() const
{
    return std::holds_alternative<std::string_view>(body_);
}

} //namespace http
