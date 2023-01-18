#include <hot_teacup/response_view.h>
#include <algorithm>
#include <regex>
#include <sstream>
#include <utility>

namespace http {

ResponseView::ResponseView(
        ResponseStatus status,
        std::string_view body,
        std::vector<CookieView> cookies,
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

const std::vector<CookieView>& ResponseView::cookies() const
{
    return cookies_;
}

const std::vector<HeaderView>& ResponseView::headers() const
{
    return headers_;
}

namespace {
std::optional<ResponseStatus> statusCodeFromString(const std::string& statusStr)
{
    static const auto statusRegex = std::regex{"HTTP/1.1 (\\d+) ?(.*)"};
    auto statusMatch = std::smatch{};
    if (!std::regex_match(statusStr, statusMatch, statusRegex))
        return std::nullopt;
    auto statusCode = std::stoi(statusMatch[1]);
    return detail::statusFromCode(statusCode);
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

std::optional<ResponseView> responseFromString(std::string_view data)
{
    auto pos = std::size_t{};
    auto statusLine = getStringLine(data, pos);
    auto status = statusCodeFromString(std::string{statusLine});
    if (status == std::nullopt)
        return std::nullopt;

    auto cookies = std::vector<CookieView>{};
    auto headers = std::vector<HeaderView>{};
    while (true) {
        auto headerLine = getStringLine(data, pos);
        if (headerLine.empty())
            break;

        auto header = headerFromString(headerLine);
        if (header == std::nullopt)
            return std::nullopt;
        if (header->name() == "Set-Cookie") {
            auto cookie = cookieFromHeader(*header);
            if (cookie)
                cookies.emplace_back(std::move(*cookie));
        }
        else
            headers.emplace_back(*header);
    }
    auto body = data.substr(pos, data.size() - pos);
    return ResponseView{*status, body, std::move(cookies), std::move(headers)};
}

} //namespace http
