#include "constants.h"
#include "utils.h"
#include <hot_teacup/request.h>
#include <hot_teacup/request_view.h>
#include <sfun/string_utils.h>
#include <algorithm>

using namespace std::string_literals;

namespace http {

namespace {
Header makeHeaderFromValueString(std::string_view name, std::string_view headerValue)
{
    const auto header = headerFromValueString(name, headerValue);
    if (!header.has_value())
        return Header{"Content-Type", "text/plain"};
    return Header{header.value()};
}
} //namespace

RequestBody::Data::Data(Header contentType, std::string data)
    : contentType_{std::move(contentType)}
    , content_{std::move(data)}
{
}

RequestBody::Data::Data(MultipartForm form)
    : contentType_{"Content-Type", "multipart/form-data", {HeaderParam{"boundary", detail::formBoundary}}}
    , content_(form.toString())
    , multipartForm_{std::move(form)}
{
}

RequestBody::Data::Data(UrlEncodedForm form)
    : contentType_{"Content-Type", "application/x-www-form-urlencoded"}
    , content_(form.toString())
    , urlEncodedForm_{std::move(form)}
{
}

HeaderView RequestBody::Data::contentType() const
{
    return contentType_.toView();
}

std::string_view RequestBody::Data::content() const
{
    return content_;
}

std::optional<MultipartFormView> RequestBody::Data::multipartForm() const
{
    if (!multipartForm_.has_value())
        return std::nullopt;

    return multipartForm_.value().toView();
}

std::optional<UrlEncodedFormView> RequestBody::Data::urlEncodedForm() const
{
    if (!urlEncodedForm_.has_value())
        return std::nullopt;

    return urlEncodedForm_.value().toView();
}

RequestBody::RequestBody(const RequestBodyView& bodyView)
    : data_(bodyView)
{
}

RequestBody::RequestBody(std::string data)
    : data_{Data{Header{"Content-Type", detail::contentTypeToString(ContentType::Json)}, std::move(data)}}
{
}

RequestBody::RequestBody(ContentType contentType, std::string data)
    : data_{Data{Header{"Content-Type", detail::contentTypeToString(contentType)}, std::move(data)}}
{
}

RequestBody::RequestBody(std::string contentTypeHeaderValue, std::string data)
    : data_{Data{makeHeaderFromValueString("Content-Type", contentTypeHeaderValue), std::move(data)}}
{
}

RequestBody::RequestBody(Header contentType, std::string data)
    : data_{Data{std::move(contentType), std::move(data)}}
{
}

RequestBody::RequestBody(MultipartForm form)
    : data_{Data{std::move(form)}}
{
}

RequestBody::RequestBody(UrlEncodedForm form)
    : data_{Data{std::move(form)}}
{
}

HeaderView RequestBody::contentType() const
{
    return std::visit(
            [](const auto& data) -> HeaderView
            {
                return data.contentType();
            },
            data_);
}

std::string_view RequestBody::content() const
{
    return std::visit(
            [](const auto& data) -> std::string_view
            {
                return data.content();
            },
            data_);
}

std::optional<MultipartFormView> RequestBody::multipartForm() const
{
    return std::visit(
            [](const auto& data) -> std::optional<MultipartFormView>
            {
                return data.multipartForm();
            },
            data_);
}

std::optional<UrlEncodedFormView> RequestBody::urlEncodedForm() const
{
    return std::visit(
            [](const auto& data) -> std::optional<UrlEncodedFormView>
            {
                return data.urlEncodedForm();
            },
            data_);
}

bool RequestBody::isView() const
{
    return std::holds_alternative<RequestBodyView>(data_);
}

void RequestBody::makeOwnStateFromView()
{
    if (!isView())
        return;

    const auto& bodyView = std::get<RequestBodyView>(data_);
    if (bodyView.multipartForm().has_value()) {
        auto multiPartForm = MultipartForm{bodyView.multipartForm().value()};
        static_cast<ICopyOnWrite&>(multiPartForm).makeOwnStateFromView();
        data_ = Data{std::move(multiPartForm)};
    }
    else if (bodyView.urlEncodedForm().has_value()) {
        auto urlEncodedForm = UrlEncodedForm{bodyView.urlEncodedForm().value()};
        data_ = Data{std::move(urlEncodedForm)};
    }
    else {
        auto header = Header{bodyView.contentType()};
        static_cast<ICopyOnWrite&>(header).makeOwnStateFromView();
        data_ = Data{header, std::string{bodyView.content()}};
    }
}

Request::Request(const RequestView& requestView)
    : method_{requestView.method()}
    , path_{requestView.path()}
    , ipAddress_{requestView.ipAddress()}
    , domainName_{requestView.domainName()}
    , queries_{makeQueries(requestView.queries())}
    , cookies_{makeCookies(requestView.cookies())}
    , body_{requestView.getRequestBody()}
    , fcgiParams_(requestView.fcgiParams())
{
}

void Request::init(std::vector<detail::RequestArg>&& args)
{
    const auto processArg = [this](detail::RequestArg& arg)
    {
        if (std::holds_alternative<std::vector<Query>>(arg))
            queries_ = std::move(std::get<std::vector<Query>>(arg));
        else if (std::holds_alternative<std::vector<Cookie>>(arg))
            cookies_ = std::move(std::get<std::vector<Cookie>>(arg));
        else if (std::holds_alternative<std::vector<Header>>(arg))
            headers_ = std::move(std::get<std::vector<Header>>(arg));
        else if (std::holds_alternative<RequestBody>(arg))
            body_ = std::move(std::get<RequestBody>(arg));
    };
    std::for_each(args.begin(), args.end(), processArg);
}

void Request::setIpAddress(const std::string& ipAddress)
{
    if (isView())
        makeOwnStateFromView();

    ipAddress_ = ipAddress;
}

void Request::setDomainName(const std::string& domainName)
{
    if (isView())
        makeOwnStateFromView();

    domainName_ = domainName;
}

void Request::addCookie(Cookie cookie)
{
    if (isView())
        makeOwnStateFromView();

    cookies_.emplace_back(std::move(cookie));
}

void Request::addQuery(Query query)
{
    if (isView())
        makeOwnStateFromView();

    queries_.emplace_back(std::move(query));
}

void Request::addHeader(Header header)
{
    if (isView())
        makeOwnStateFromView();

    headers_.emplace_back(std::move(header));
}

void Request::setCookies(const std::vector<Cookie>& cookies)
{
    if (isView())
        makeOwnStateFromView();

    cookies_ = cookies;
}

void Request::setQueries(const std::vector<Query>& queries)
{
    if (isView())
        makeOwnStateFromView();

    queries_ = queries;
}

void Request::setHeaders(const std::vector<Header>& headers)
{
    if (isView())
        makeOwnStateFromView();

    headers_ = headers;
}

RequestMethod Request::method() const
{
    return method_;
}

std::string_view Request::path() const
{
    return std::visit(
            [](const auto& path) -> std::string_view
            {
                return path;
            },
            path_);
}

std::string_view Request::ipAddress() const
{
    return std::visit(
            [](const auto& ipAddress) -> std::string_view
            {
                return ipAddress;
            },
            ipAddress_);
}

std::string_view Request::domainName() const
{
    return std::visit(
            [](const auto& domainName) -> std::string_view
            {
                return domainName;
            },
            domainName_);
}

std::string_view Request::query(std::string_view name) const
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

bool Request::hasQuery(std::string_view name) const
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

std::string_view Request::cookie(std::string_view name) const
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

bool Request::hasCookie(std::string_view name) const
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

const std::vector<Header>& Request::headers() const
{
    return headers_;
}

std::string_view Request::header(std::string_view name) const
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

bool Request::hasHeader(std::string_view name) const
{
    auto it = std::find_if(
            headers_.begin(),
            headers_.end(),
            [&name](const auto& header)
            {
                return header.name() == name;
            });
    return (it != headers_.end());
}

std::optional<HeaderView> Request::contentType() const
{
    if (!body_.has_value())
        return {};

    return body_.value().contentType();
}

std::string_view Request::body() const
{
    if (!body_.has_value())
        return {};

    return body_.value().content();
}

std::optional<MultipartFormView> Request::multipartForm() const
{
    if (!body_.has_value())
        return {};

    return body_.value().multipartForm();
}

std::optional<UrlEncodedFormView> Request::urlEncodedForm() const
{
    if (!body_.has_value())
        return {};

    return body_.value().urlEncodedForm();
}

const std::vector<Query>& Request::queries() const
{
    return queries_;
}

const std::vector<Cookie>& Request::cookies() const
{
    return cookies_;
}

std::unordered_map<std::string_view, std::string_view> Request::fcgiParams() const
{
    if (std::holds_alternative<std::unordered_map<std::string_view, std::string_view>>(fcgiParams_))
        return std::get<std::unordered_map<std::string_view, std::string_view>>(fcgiParams_);

    const auto& fcgiParams = std::get<std::unordered_map<std::string, std::string>>(fcgiParams_);
    return std::unordered_map<std::string_view, std::string_view>{fcgiParams.begin(), fcgiParams.end()};
}

RequestFcgiData Request::toFcgiData(std::map<std::string, std::string> fcgiParams) const
{
    const auto formBoundary = "----asyncgiFormBoundary"s;

    auto makeFcgiParams = [&]
    {
        fcgiParams["REQUEST_METHOD"] = methodToString(method_);
        const auto path = std::visit(
                [](const auto& path) -> std::string_view
                {
                    return path;
                },
                path_);
        if (!path.empty())
            fcgiParams["REQUEST_URI"] = path;
        if (!queries_.empty())
            fcgiParams["QUERY_STRING"] = queriesToString(queries_);
        if (!cookies_.empty())
            fcgiParams["HTTP_COOKIE"] = cookiesToString(cookies_);
        if (body_.has_value())
            fcgiParams["CONTENT_TYPE"] =
                    sfun::trim_front(sfun::after(Header{body_.value().contentType()}.toString(), ":").value());

        return fcgiParams;
    };
    auto makeFcgiStdIn = [&]() -> std::string
    {
        if (!body_.has_value())
            return {};

        return std::string{body_.value().content()};
    };

    return {makeFcgiParams(), makeFcgiStdIn()};
}

bool Request::isView() const
{
    return std::holds_alternative<std::string_view>(path_);
}

void Request::makeOwnStateFromView()
{
    if (!isView())
        return;

    const auto pathView = std::get<std::string_view>(path_);
    path_ = std::string{pathView};

    const auto ipAddress = std::get<std::string_view>(ipAddress_);
    ipAddress_ = std::string{ipAddress};

    const auto domainName = std::get<std::string_view>(domainName_);
    domainName_ = std::string{domainName};

    for (auto& query : queries_)
        static_cast<ICopyOnWrite&>(query).makeOwnStateFromView();

    for (auto& cookie : cookies_)
        static_cast<ICopyOnWrite&>(cookie).makeOwnStateFromView();

    for (auto& header : headers_)
        static_cast<ICopyOnWrite&>(header).makeOwnStateFromView();

    if (body_.has_value())
        static_cast<ICopyOnWrite&>(body_.value()).makeOwnStateFromView();

    const auto fcgiParams = std::get<std::unordered_map<std::string_view, std::string_view>>(fcgiParams_);
    const auto toOwningPair = [](const std::pair<std::string_view, std::string_view>& nameValuePair)
    {
        return std::make_pair(std::string{nameValuePair.first}, std::string{nameValuePair.second});
    };
    fcgiParams_ = utils::transformTo<std::unordered_map<std::string, std::string>>(fcgiParams, toOwningPair);
}

bool operator==(const Request& lhs, const Request& rhs)
{
    return lhs.method() == rhs.method() && lhs.ipAddress() == rhs.ipAddress() && lhs.domainName() == rhs.domainName() &&
            lhs.path() == rhs.path() && lhs.queries() == rhs.queries() && lhs.cookies() == rhs.cookies() &&
            lhs.contentType() == rhs.contentType() && lhs.body() == rhs.body() && lhs.fcgiParams() == rhs.fcgiParams();
}

} //namespace http
