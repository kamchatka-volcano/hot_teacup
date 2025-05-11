#include <hot_teacup/response_view.h>

#include <algorithm>
#include <regex>
#include <utility>

namespace http {

ResponseView::ResponseView(
        ResponseStatus status,
        std::string_view body,
        std::vector<ResponseCookieView> cookies,
        std::vector<HeaderView> headers)
    : status_(status)
    , body_(body)
    , cookies_(std::move(cookies))
    , headers_(std::move(headers))
{
}

ResponseStatus ResponseView::status() const
{
    return status_;
}

std::string_view ResponseView::body() const
{
    return body_;
}

const std::vector<ResponseCookieView>& ResponseView::cookies() const
{
    return cookies_;
}

std::string_view ResponseView::cookieValue(std::string_view name) const
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

std::optional<ResponseCookieView> ResponseView::cookie(std::string_view name) const
{
    auto it = std::find_if(
            cookies_.begin(),
            cookies_.end(),
            [&name](const auto& cookie)
            {
                return cookie.name() == name;
            });
    if (it != cookies_.end())
        return *it;

    return std::nullopt;
}

bool ResponseView::hasCookie(std::string_view name) const
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

const std::vector<HeaderView>& ResponseView::headers() const
{
    return headers_;
}

std::string_view ResponseView::headerValue(std::string_view name) const
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

std::optional<HeaderView> ResponseView::header(std::string_view name) const
{
    auto it = std::find_if(
            headers_.begin(),
            headers_.end(),
            [&name](const auto& header)
            {
                return header.name() == name;
            });
    if (it != headers_.end())
        return *it;

    return std::nullopt;
}

bool ResponseView::hasHeader(std::string_view name) const
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

bool operator==(const ResponseView& lhs, const ResponseView& rhs)
{
    return lhs.status() == rhs.status() && lhs.body() == rhs.body() && lhs.cookies() == rhs.cookies() &&
            lhs.headers() == rhs.headers();
}

namespace {
template<ResponseMode mode>
auto makeStatusRegex()
{
    if constexpr (mode == ResponseMode::Http)
        return std::regex{"HTTP/1.1 (\\d+) ?(.*)"};
    else
        return std::regex{"Status: (\\d+) ?(.*)"};
}

template<ResponseMode mode>
std::optional<ResponseStatus> statusCodeFromString(const std::string& statusStr)
{
    static const auto statusRegex = makeStatusRegex<mode>();
    auto statusMatch = std::smatch{};
    if (!std::regex_match(statusStr, statusMatch, statusRegex))
        return std::nullopt;
    auto statusCode = std::stoi(statusMatch[1]);
    return statusFromCode(statusCode);
}

std::string_view getStringLine(std::string_view input, std::size_t& pos, std::string_view lineSeparator = "\r\n")
{
    auto lastPos = input.find(lineSeparator, pos);
    auto separatorSize = lineSeparator.size();
    if (lastPos == std::string::npos) {
        lastPos = input.size();
        separatorSize = 0;
    }
    auto lineSize = lastPos - pos;
    auto linePos = pos;
    pos += lineSize + separatorSize;
    return input.substr(linePos, lineSize);
}

} //namespace

std::optional<ResponseView> responseFromString(std::string_view data, ResponseMode mode)
{
    auto pos = std::size_t{};
    auto statusLine = getStringLine(data, pos);
    auto status = (mode == ResponseMode::Http) ? statusCodeFromString<ResponseMode::Http>(std::string{statusLine})
                                               : statusCodeFromString<ResponseMode::Cgi>(std::string{statusLine});
    if (status == std::nullopt)
        return std::nullopt;

    auto cookies = std::vector<ResponseCookieView>{};
    auto headers = std::vector<HeaderView>{};
    while (true) {
        auto headerLine = getStringLine(data, pos);
        if (headerLine.empty())
            break;

        auto header = headerFromString(headerLine);
        if (header == std::nullopt)
            return std::nullopt;
        if (header->name() == "Set-Cookie") {
            auto cookie = responseCookieFromHeader(*header);
            if (cookie.has_value())
                cookies.emplace_back(std::move(cookie.value()));
        }
        else
            headers.emplace_back(*header);
    }
    auto body = data.substr(pos, data.size() - pos);
    return ResponseView{*status, body, std::move(cookies), std::move(headers)};
}

} //namespace http
