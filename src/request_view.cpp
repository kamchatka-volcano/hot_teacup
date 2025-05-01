#include "constants.h"
#include <hot_teacup/request_view.h>
#include <sfun/string_utils.h>
#include <algorithm>

using namespace std::string_literals;

namespace http {

namespace {

std::optional<RequestBodyView> readRequestBody(std::string_view contentTypeHeaderValue, std::string_view requestBody)
{
    if (requestBody.empty())
        return std::nullopt;

    const auto contentTypeHeader = headerFromValueString("Content-Type", contentTypeHeaderValue)
                                           .value_or(HeaderView{"Content-Type", "text/plain"});
    return RequestBodyView{contentTypeHeader, requestBody};
}

} //namespace

RequestBodyView::RequestBodyView(HeaderView contentType, std::string_view content)
    : contentType_{std::move(contentType)}
    , content_{content}
{
}

HeaderView RequestBodyView::contentType() const
{
    return contentType_;
}

std::string_view RequestBodyView::content() const
{
    return content_;
}

std::optional<MultipartFormView> RequestBodyView::multipartForm() const
{
    return multipartForm_.get(*this);
}

std::optional<UrlEncodedFormView> RequestBodyView::urlEncodedForm() const
{
    const auto& form = urlEncodedForm_.get(*this);
    if (!form.has_value())
        return std::nullopt;
    return form->toView();
}

std::optional<MultipartFormView> RequestBodyView::createMultipartForm() const
{
    if (contentType_.value() == "multipart/form-data")
        return multipartFormViewFromString(contentType_, content_);
    return std::nullopt;
}

std::optional<UrlEncodedForm> RequestBodyView::createUrlEncodedForm() const
{
    if (contentType_.value() == "application/x-www-form-urlencoded")
        return urlEncodedFormFromString(content_);
    return std::nullopt;
}

RequestView::RequestView(
        std::string_view fcgiParamRequestMethod,
        std::string_view fcgiParamRemoteAddr,
        std::string_view fcgiParamHttpHost,
        std::string_view fcgiParamRequestUri,
        std::string_view fcgiParamQueryString,
        std::string_view fcgiParamHttpCookie,
        std::string_view fcgiParamContentType,
        std::string_view fcgiStdIn,
        std::unordered_map<std::string_view, std::string_view> fcgiParams)
    : method_{methodFromString(fcgiParamRequestMethod)}
    , ipAddress_{fcgiParamRemoteAddr}
    , domainName_{sfun::before(fcgiParamHttpHost, ":").value_or(fcgiParamHttpHost)}
    , path_{sfun::before(fcgiParamRequestUri, "?").value_or(fcgiParamRequestUri)}
    , queries_{queriesFromString(fcgiParamQueryString)}
    , cookies_{cookiesFromString(fcgiParamHttpCookie)}
    , body_{readRequestBody(fcgiParamContentType, fcgiStdIn)}
    , fcgiParams_{std::move(fcgiParams)}

{
}

RequestMethod RequestView::method() const
{
    return method_;
}

std::string_view RequestView::ipAddress() const
{
    return ipAddress_;
}

std::string_view RequestView::domainName() const
{
    return domainName_;
}

std::string_view RequestView::path() const
{
    return path_;
}

std::string_view RequestView::query(std::string_view name) const
{
    auto it = std::find_if(
            queries_.begin(),
            queries_.end(),
            [&name](const auto& query)
            {
                return query.name() == name;
            });
    if (it != queries_.end())
        return it->value();

    return {};
}

bool RequestView::hasQuery(std::string_view name) const
{
    auto it = std::find_if(
            queries_.begin(),
            queries_.end(),
            [&name](const auto& query)
            {
                return query.name() == name;
            });
    return (it != queries_.end());
}

std::string_view RequestView::cookie(std::string_view name) const
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

bool RequestView::hasCookie(std::string_view name) const
{
    auto it = std::find_if(
            cookies_.begin(),
            cookies_.end(),
            [&name](const auto& cookie)
            {
                return cookie.name() == name;
            });
    return (it != cookies_.end());
}

std::optional<HeaderView> RequestView::contentType() const
{
    if (!body_.has_value())
        return std::nullopt;

    return body_.value().contentType();
}

std::string_view RequestView::body() const
{
    if (!body_.has_value())
        return {};

    return body_.value().content();
}

std::optional<RequestBodyView> RequestView::getRequestBody() const
{
    return body_;
}

std::optional<MultipartFormView> RequestView::multipartForm() const
{
    if (!body_.has_value())
        return std::nullopt;

    return body_.value().multipartForm();
}

std::optional<UrlEncodedFormView> RequestView::urlEncodedForm() const
{
    if (!body_.has_value())
        return std::nullopt;

    return body_.value().urlEncodedForm();
}

const std::vector<QueryView>& RequestView::queries() const
{
    return queries_;
}

const std::vector<CookieView>& RequestView::cookies() const
{
    return cookies_;
}

const std::unordered_map<std::string_view, std::string_view>& RequestView::fcgiParams() const
{
    return fcgiParams_;
}

bool operator==(const RequestBodyView& lhs, const RequestBodyView& rhs)
{
    return lhs.contentType() == rhs.contentType() && lhs.content() == rhs.content();
}

bool operator==(const RequestView& lhs, const RequestView& rhs)
{
    return lhs.method() == rhs.method() && lhs.ipAddress() == rhs.ipAddress() && lhs.domainName() == rhs.domainName() &&
            lhs.path() == rhs.path() && lhs.queries() == rhs.queries() && lhs.cookies() == rhs.cookies() &&
            lhs.getRequestBody() == rhs.getRequestBody();
}

} //namespace http
