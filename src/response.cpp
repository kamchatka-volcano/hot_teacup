#include <hot_teacup/response.h>

#include "utils.h"
#include <hot_teacup/response_view.h>
#include <sfun/string_utils.h>
#include <algorithm>
#include <iterator>
#include <utility>

namespace http {

namespace {
std::vector<Header> makeHeaders(const std::vector<HeaderView>& headerViewList)
{
    return utils::transform(
            headerViewList,
            [](const HeaderView& headerView)
            {
                return Header{headerView};
            });
}
std::vector<SetCookie> makeSetCookies(const std::vector<SetCookieView>& cookieViewList)
{
    return utils::transform(
            cookieViewList,
            [](const SetCookieView& cookieView)
            {
                return SetCookie{cookieView};
            });
}
} //namespace

Response::Response(const ResponseView& responseView)
    : status_{responseView.status()}
    , body_{responseView.body()}
    , cookies_{makeSetCookies(responseView.cookies())}
    , headers_{makeHeaders(responseView.headers())}
{
}

void Response::initBodyResponse(std::vector<detail::BodyResponseArg>&& args)
{
    const auto processArg = [this](detail::BodyResponseArg& arg)
    {
        if (std::holds_alternative<SetCookies>(arg)) {
            cookies_ = std::move(std::get<SetCookies>(arg));
            for (auto& cookie : cookies_)
                static_cast<detail::IViewOrOwner&>(cookie).makeOwnStateFromView();
        }
        else if (std::holds_alternative<Headers>(arg)) {
            headers_ = std::move(std::get<Headers>(arg));
            for (auto& header : headers_)
                static_cast<detail::IViewOrOwner&>(header).makeOwnStateFromView();
        }
        else if (std::holds_alternative<ContentType>(arg))
            defaultContentTypeHeader_ = {"Content-Type", detail::contentTypeToString(std::get<ContentType>(arg))};
        else if (std::holds_alternative<ContentTypeString>(arg))
            defaultContentTypeHeader_ = {"Content-Type", std::move(std::get<ContentTypeString>(arg).value)};
    };

    std::for_each(args.begin(), args.end(), processArg);
    if (!defaultContentTypeHeader_.has_value() && !body().empty())
        defaultContentTypeHeader_ = {"Content-Type", detail::contentTypeToString(ContentType::Html)};

    addDefaultContentTypeHeader();
}

void Response::initRedirectResponse(std::vector<detail::RedirectResponseArg>&& args)
{
    const auto processArg = [this](detail::RedirectResponseArg& arg)
    {
        if (std::holds_alternative<SetCookies>(arg)) {
            cookies_ = std::move(std::get<SetCookies>(arg));
            for (auto& cookie : cookies_)
                static_cast<detail::IViewOrOwner&>(cookie).makeOwnStateFromView();
        }
        else if (std::holds_alternative<Headers>(arg)) {
            headers_ = std::move(std::get<Headers>(arg));
            for (auto& header : headers_)
                static_cast<detail::IViewOrOwner&>(header).makeOwnStateFromView();
        }
    };

    std::for_each(args.begin(), args.end(), processArg);
    addDefaultLocationHeader();
}

void Response::initStatusResponse(std::vector<detail::StatusResponseArg>&& args)
{
    const auto processArg = [this](detail::StatusResponseArg& arg)
    {
        if (std::holds_alternative<std::string>(arg))
            body_ = std::move(std::get<std::string>(arg));
        else if (std::holds_alternative<SetCookies>(arg)) {
            cookies_ = std::move(std::get<SetCookies>(arg));
            for (auto& cookie : cookies_)
                static_cast<detail::IViewOrOwner&>(cookie).makeOwnStateFromView();
        }
        else if (std::holds_alternative<Headers>(arg)) {
            headers_ = std::move(std::get<Headers>(arg));
            for (auto& header : headers_)
                static_cast<detail::IViewOrOwner&>(header).makeOwnStateFromView();
        }
        else if (std::holds_alternative<ContentType>(arg))
            defaultContentTypeHeader_ = {"Content-Type", detail::contentTypeToString(std::get<ContentType>(arg))};
        else if (std::holds_alternative<ContentTypeString>(arg))
            defaultContentTypeHeader_ = {"Content-Type", std::move(std::get<ContentTypeString>(arg).value)};
    };
    std::for_each(args.begin(), args.end(), processArg);
    if (!defaultContentTypeHeader_.has_value() && !body().empty())
        defaultContentTypeHeader_ = {"Content-Type", detail::contentTypeToString(ContentType::Html)};

    addDefaultContentTypeHeader();
}

void Response::addDefaultContentTypeHeader()
{
    if (!defaultContentTypeHeader_.has_value())
        return;

    const auto contentTypeHeaderIt = std::find_if(
            headers_.begin(),
            headers_.end(),
            [](const auto& header)
            {
                return header.name() == "Content-Type";
            });
    if (contentTypeHeaderIt != headers_.end())
        headers_.erase(contentTypeHeaderIt);

    headers_.emplace_back(defaultContentTypeHeader_.value());
}

void Response::addDefaultLocationHeader()
{
    if (!redirect_.has_value())
        return;

    const auto locationHeaderIt = std::find_if(
            headers_.begin(),
            headers_.end(),
            [](const auto& header)
            {
                return header.name() == "Location";
            });
    if (locationHeaderIt != headers_.end())
        headers_.erase(locationHeaderIt);
    status_ = detail::redirectTypeStatus(redirect_.value().type);
    headers_.emplace_back("Location", redirect_.value().path);
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

const std::vector<SetCookie>& Response::cookies() const
{
    return cookies_;
}

std::string_view Response::cookie(std::string_view name) const
{
    auto it = std::find_if(
            cookies_.begin(),
            cookies_.end(),
            [&name](const auto& cookie)
            {
                return cookie.name() == name;
            });
    if (it != cookies_.end())
        return it->value();

    return {};
}

bool Response::hasCookie(std::string_view name) const
{
    auto it = std::find_if(
            cookies_.begin(),
            cookies_.end(),
            [&name](const auto& cookie)
            {
                return cookie.name() == name;
            });
    return it != cookies_.end();
}

const std::vector<Header>& Response::headers() const
{
    return headers_;
}

std::string_view Response::header(std::string_view name) const
{
    auto it = std::find_if(
            headers_.begin(),
            headers_.end(),
            [&name](const auto& header)
            {
                return header.name() == name;
            });
    if (it != headers_.end())
        return it->value();

    return {};
}

bool Response::hasHeader(std::string_view name) const
{
    auto it = std::find_if(
            headers_.begin(),
            headers_.end(),
            [&name](const auto& header)
            {
                return header.name() == name;
            });
    return it != headers_.end();
}

void Response::addCookie(SetCookie cookie)
{
    if (isView())
        makeOwnStateFromView();
    static_cast<detail::IViewOrOwner&>(cookie).makeOwnStateFromView();
    cookies_.emplace_back(std::move(cookie));
}

void Response::addHeader(Header header)
{
    if (isView())
        makeOwnStateFromView();

    if (defaultContentTypeHeader_.has_value() && header.name() == "Content-Type")
        return;
    if (redirect_.has_value() && header.name() == "Location")
        return;
    static_cast<detail::IViewOrOwner&>(header).makeOwnStateFromView();
    headers_.emplace_back(std::move(header));
}

void Response::setCookies(const std::vector<SetCookie>& cookies)
{
    if (isView())
        makeOwnStateFromView();

    cookies_ = cookies;
    for (auto& cookie : cookies_)
        static_cast<detail::IViewOrOwner&>(cookie).makeOwnStateFromView();
}

void Response::setHeaders(const std::vector<Header>& headers)
{
    if (isView())
        makeOwnStateFromView();

    headers_ = headers;
    for (auto& header : headers_)
        static_cast<detail::IViewOrOwner&>(header).makeOwnStateFromView();
    addDefaultLocationHeader();
    addDefaultContentTypeHeader();
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
    const auto cookieToString = [](const SetCookie& cookie)
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
    const auto headerStringList = utils::transform(headers_, headerToString);
    const auto lastSeparator = headers_.empty() ? std::string_view{} : std::string_view{"\r\n"};
    return sfun::join_strings(sfun::join(headerStringList, "\r\n"), lastSeparator);
}

std::string Response::toString(ResponseMode mode) const
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

void Response::makeOwnStateFromView()
{
    if (!isView())
        return;

    body_ = std::string{std::get<std::string_view>(body_)};
    for (auto& cookie : cookies_)
        static_cast<IViewOrOwner&>(cookie).makeOwnStateFromView();
    for (auto& header : headers_)
        static_cast<IViewOrOwner&>(header).makeOwnStateFromView();
}

bool operator==(const Response& lhs, const Response& rhs)
{
    return lhs.status() == rhs.status() && lhs.body() == rhs.body() && lhs.cookies() == rhs.cookies() &&
            lhs.headers() == rhs.headers();
}

} //namespace http
