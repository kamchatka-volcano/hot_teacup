#include <hot_teacup/response.h>
#include <algorithm>
#include <utility>
#include <sstream>
#include <regex>

namespace http{

Response::Response(std::string data)
    : Response(ResponseStatus::Code_200_Ok,
               std::move(data),
               {},
               {{"ContentType", detail::contentTypeToString(ContentType::HTML)}})
{}

Response::Response(ResponseStatus status,
                   std::string body,
                   Cookies cookies,
                   Headers headers)
    : status_(status)
    , body_(std::move(body))
    , cookies_(std::move(cookies))
    , headers_(std::move(headers))
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

const Cookies& Response::cookies() const
{
    return cookies_;
}

const Headers& Response::headers() const
{
    return headers_;
}

void Response::addCookie(Cookie cookie)
{
    cookies_.emplace_back(std::move(cookie));
}

void Response::addHeader(Header header)
{
    headers_.emplace_back(std::move(header));
}

void Response::addCookies(const Cookies& cookies)
{
    std::copy(cookies.begin(), cookies.end(), std::back_inserter(cookies_));
}

void Response::addHeaders(const Headers& headers)
{
    std::copy(headers.begin(), headers.end(), std::back_inserter(headers_));
}

std::string Response::statusData() const
{
    return "HTTP/1.1 " + std::string{detail::statusToString(status_)} + "\r\n";
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

std::string Response::data() const
{
    if (!rawResponse_.data.empty())
        return rawResponse_.data;

    return statusData() +
           headersData() +
           cookiesData() +
           "\r\n" + body_;
}

Response Response::Redirect(const std::string &path,
                            RedirectType type,
                            const Queries &queries,
                            const Cookies &cookies,
                            const Headers &headers)
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
                           const Cookies &cookies,
                           const Headers &headers)
{
    auto responseValue = Response{ResponseStatus::Code_200_Ok,
                                  std::move(text),
                                  cookies,
                                  {{"ContentType", detail::contentTypeToString(contentType)}}
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

namespace{
std::optional<ResponseStatus> statusCodeFromString(const std::string &statusStr)
{
    static const auto statusRegex = std::regex{"HTTP/1.1 (\\d+) ?(.*)"};
    auto statusMatch = std::smatch{};
    if (!std::regex_match(statusStr, statusMatch, statusRegex))
        return std::nullopt;
    auto statusCode = std::stoi(statusMatch[1]);
    return detail::statusFromCode(statusCode);
}

std::string lineFromStream(std::istream& stream)
{
    auto res = std::string{};
    std::getline(stream, res);
    if (!res.empty())
        res.resize(res.size() - 1); //remove \r
    return res;
}
}

std::optional<Response> responseFromString(const std::string& data)
{
    auto stream = std::stringstream{data};
    auto statusLine = lineFromStream(stream);
    auto status = statusCodeFromString(statusLine);
    if (status == std::nullopt)
        return std::nullopt;

    auto cookies = Cookies{};
    auto headers = Headers{};
    while (true){
        auto headerLine = lineFromStream(stream);
        if (headerLine.empty())
            break;

        auto header = headerFromString(headerLine);
        if (header == std::nullopt)
            return std::nullopt;
        if (header->name() == "Set-Cookie"){
            auto cookie = cookieFromHeader(*header);
            if (cookie)
                cookies.emplace_back(std::move(*cookie));
        }
        else
            headers.emplace_back(*header);
    }
    stream >> std::noskipws;
    auto body = std::string{std::istream_iterator<char>{stream}, {}};
    return Response{*status, std::move(body), std::move(cookies), std::move(headers)};
}

}

