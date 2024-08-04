#ifndef HOT_TEACUP_RESPONSE_H
#define HOT_TEACUP_RESPONSE_H

#include "cookie.h"
#include "header.h"
#include "query.h"
#include "trait_utils.h"
#include "types.h"
#include <string>
#include <variant>

namespace http {
class ResponseView;

struct Redirect {
    std::string path;
    RedirectType type = RedirectType::Found;
};

namespace detail {
using ResponseArg = std::variant<
        ResponseStatus,
        std::string,
        ContentType,
        ContentTypeString,
        http::Redirect,
        std::vector<Cookie>,
        std::vector<Header>>;
}

class Response {
public:
    explicit Response(const ResponseView&);

    template<
            typename... TArgs,
            typename = std::enable_if_t<
                    ((!std::is_same_v<std::decay_t<TArgs>, ResponseView> &&
                      !std::is_same_v<std::decay_t<TArgs>, Response>) &&
                     ...)>>
    Response(TArgs&&... args)
    {
        static_assert(
                !detail::has_duplicate_v<detail::decay_to_string_view_t<TArgs>...>,
                "Response constructor arguments can't contain duplicate types");
        static_assert(
                !(detail::is_element_of_v<Redirect, TArgs...> && detail::is_element_of_v<ResponseStatus, TArgs...>),
                "Response status and redirect can't be specified together in response constructor arguments");
        static_assert(
                !(detail::is_element_of_v<ContentType, TArgs...> && detail::is_element_of_v<ContentTypeString, TArgs...>),
                "ContentType enum and ContentTypeString can't be specified together in response constructor arguments");

        init({std::forward<TArgs>(args)...});
    }

    ResponseStatus status() const;
    std::string_view body() const;
    const std::vector<Cookie>& cookies() const;
    const std::vector<Header>& headers() const;
    std::string data(ResponseMode mode = ResponseMode::Http) const;

    void setBody(const std::string& body);
    void addCookie(Cookie cookie);
    void addHeader(Header header);
    void setCookies(const std::vector<Cookie>& cookies);
    void setHeaders(const std::vector<Header>& headers);

    friend bool operator==(const Response& lhs, const Response& rhs);

private:
    void init(std::vector<detail::ResponseArg>&& args);
    std::string statusData(ResponseMode mode) const;
    std::string cookiesData() const;
    std::string headersData() const;
    bool isView() const;
    void addDefaultContentTypeHeader();

private:
    ResponseStatus status_ = ResponseStatus::_200_Ok;
    std::variant<std::string, std::string_view> body_;
    std::vector<Cookie> cookies_;
    std::vector<Header> defaultHeaders_;
    std::vector<Header> headers_;
};

} //namespace http

#endif //HOT_TEACUP_RESPONSE_H