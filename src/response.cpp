#include "utils.h"
#include <hot_teacup/response.h>
#include <hot_teacup/response_view.h>
#include <sfun/string_utils.h>
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

void Response::init(std::vector<detail::ResponseArg>&& args)
{
    const auto processArg = [this](detail::ResponseArg& arg)
    {
        if (std::holds_alternative<ResponseStatus>(arg))
            status_ = std::get<ResponseStatus>(arg);
        else if (std::holds_alternative<std::string>(arg))
            body_ = std::move(std::get<std::string>(arg));
        else if (std::holds_alternative<http::Cookies>(arg))
            cookies_ = std::move(std::get<http::Cookies>(arg));
        else if (std::holds_alternative<http::Headers>(arg))
            headers_ = std::move(std::get<http::Headers>(arg));
        else if (std::holds_alternative<ContentType>(arg))
            defaultHeaders_.push_back({"Content-Type", detail::contentTypeToString(std::get<ContentType>(arg))});
        else if (std::holds_alternative<ContentTypeString>(arg))
            defaultHeaders_.push_back({"Content-Type", std::move(std::get<ContentTypeString>(arg).value)});
        else if (std::holds_alternative<Redirect>(arg)) {
            auto& redirect = std::get<Redirect>(arg);
            status_ = detail::redirectTypeStatus(redirect.type);
            defaultHeaders_.push_back({"Location", std::move(redirect.path)});
        }
    };

    std::for_each(args.begin(), args.end(), processArg);
    addDefaultContentTypeHeader();
}

void Response::addDefaultContentTypeHeader()
{
    if (!std::get<std::string>(body_).empty()) {
        const auto defaultHeadersHasContentType = std::find_if(
                                                          defaultHeaders_.begin(),
                                                          defaultHeaders_.end(),
                                                          [](const Header& header)
                                                          {
                                                              return header.name() == "Content-Type";
                                                          }) != defaultHeaders_.end();
        const auto headersHasContentType = std::find_if(
                                                   headers_.begin(),
                                                   headers_.end(),
                                                   [](const Header& header)
                                                   {
                                                       return header.name() == "Content-Type";
                                                   }) != headers_.end();

        if (!defaultHeadersHasContentType && !headersHasContentType)
            defaultHeaders_.emplace_back("Content-Type", detail::contentTypeToString(ContentType::Html));
    }
}

ResponseStatus Response::status() const
{
    return status_;
}

std::string_view Response::body() const
{
    return std::visit(
            [](const auto& body) -> std::string_view
            {
                return body;
            },
            body_);
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

void Response::setCookies(const std::vector<Cookie>& cookies)
{
    if (isView())
        return;

    cookies_ = cookies;
}

void Response::setHeaders(const std::vector<Header>& headers)
{
    if (isView())
        return;

    headers_ = headers;
}

std::string Response::statusData(ResponseMode mode) const
{
    return sfun::join_strings(
            mode == ResponseMode::Cgi ? "Status: " : "HTTP/1.1 ",
            detail::statusToString(status_),
            "\r\n");
}

std::string Response::cookiesData() const
{
    const auto cookieToString = [](const Cookie& cookie)
    {
        return cookie.toString();
    };
    const auto cookieStringList = utils::transform(cookies_, cookieToString);
    const auto lastSeparator = cookies_.empty() ? std::string_view{} : std::string_view{"\r\n"};
    return sfun::join_strings(sfun::join(cookieStringList, "\r\n"), lastSeparator);
}

std::string Response::headersData() const
{
    const auto headerToString = [](const Header& header)
    {
        return header.toString();
    };
    const auto defaultHeaderStringList = utils::transform(defaultHeaders_, headerToString);
    const auto headerStringList = utils::transform(headers_, headerToString);
    const auto defaultHeaderSeparator = defaultHeaders_.empty() ? std::string_view{} : std::string_view{"\r\n"};
    const auto lastSeparator = headers_.empty() ? std::string_view{} : std::string_view{"\r\n"};
    return sfun::join_strings(
            sfun::join(defaultHeaderStringList, "\r\n"),
            defaultHeaderSeparator,
            sfun::join(headerStringList, "\r\n"),
            lastSeparator);
}

std::string Response::data(ResponseMode mode) const
{
    const auto body = std::visit(
            [](const auto& body) -> std::string_view
            {
                return body;
            },
            body_);
    return sfun::join_strings(statusData(mode), headersData(), cookiesData(), "\r\n", body);
}

bool Response::isView() const
{
    return std::holds_alternative<std::string_view>(body_);
}

bool operator==(const Response& lhs, const Response& rhs)
{
    return lhs.status() == rhs.status() && lhs.body() == rhs.body() && lhs.cookies() == rhs.cookies() &&
            lhs.headers() == rhs.headers();
}

} //namespace http
