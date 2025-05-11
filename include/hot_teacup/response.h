#ifndef HOT_TEACUP_RESPONSE_H
#define HOT_TEACUP_RESPONSE_H

#include "header.h"
#include "response_cookie.h"
#include "trait_utils.h"
#include "types.h"
#include "detail/view_or_owner_interface.h"
#include <string>
#include <variant>

namespace http {
class ResponseView;

struct Redirect {
    std::string path;
    RedirectType type = RedirectType::Found;
};

namespace detail {
using BodyResponseArg = std::variant<ContentType, ContentTypeString, std::vector<ResponseCookie>, std::vector<Header>>;

using StatusResponseArg =
        std::variant<std::string, ContentType, ContentTypeString, std::vector<ResponseCookie>, std::vector<Header>>;

using RedirectResponseArg = std::variant<std::vector<ResponseCookie>, std::vector<Header>>;
} //namespace detail

class Response : public detail::IViewOrOwner {
public:
    explicit Response(const ResponseView&);

    template<
            typename... TArgs,
            typename = std::enable_if_t<
                    ((!std::is_same_v<std::decay_t<TArgs>, ResponseView> &&
                      !std::is_same_v<std::decay_t<TArgs>, Response>) &&
                     ...)>>
    Response(std::string body, TArgs&&... args)
        : body_{std::move(body)}
    {
        static_assert(
                !detail::has_duplicate_v<detail::decay_to_string_view_t<TArgs>...>,
                "Response constructor arguments can't contain duplicate types");
        static_assert(
                !(detail::is_element_of_v<ContentType, TArgs...> &&
                  detail::is_element_of_v<ContentTypeString, TArgs...>),
                "ContentType enum and ContentTypeString can't be specified together in response constructor arguments");

        initBodyResponse({std::forward<TArgs>(args)...});
    }

    template<
            typename... TArgs,
            typename = std::enable_if_t<
                    ((!std::is_same_v<std::decay_t<TArgs>, ResponseView> &&
                      !std::is_same_v<std::decay_t<TArgs>, Response>) &&
                     ...)>>
    Response(ResponseStatus status, TArgs&&... args)
        : status_{status}
    {
        static_assert(
                !detail::has_duplicate_v<detail::decay_to_string_view_t<TArgs>...>,
                "Response constructor arguments can't contain duplicate types");
        static_assert(
                !(detail::is_element_of_v<ContentType, TArgs...> &&
                  detail::is_element_of_v<ContentTypeString, TArgs...>),
                "ContentType enum and ContentTypeString can't be specified together in response constructor arguments");
        static_assert(
                !(!detail::is_element_of_v<std::string_view, detail::decay_to_string_view_t<TArgs>...> &&
                  (detail::is_element_of_v<ContentType, TArgs...> ||
                   detail::is_element_of_v<ContentTypeString, TArgs...>)),
                "ContentType enum and ContentTypeString can't be specified without body in response constructor "
                "arguments");

        initStatusResponse({std::forward<TArgs>(args)...});
    }

    template<
            typename... TArgs,
            typename = std::enable_if_t<
                    ((!std::is_same_v<std::decay_t<TArgs>, ResponseView> &&
                      !std::is_same_v<std::decay_t<TArgs>, Response>) &&
                     ...)>>
    Response(Redirect redirect, TArgs&&... args)
        : redirect_{std::move(redirect)}
    {
        static_assert(
                !detail::has_duplicate_v<detail::decay_to_string_view_t<TArgs>...>,
                "Response constructor arguments can't contain duplicate types");

        initRedirectResponse({std::forward<TArgs>(args)...});
    }

    ResponseStatus status() const;
    std::string_view body() const;
    const std::vector<ResponseCookie>& cookies() const;
    // value of the first cookie with the same name, empty string if not found
    std::string_view cookie(std::string_view name) const;
    bool hasCookie(std::string_view name) const;

    const std::vector<Header>& headers() const;
    // value of the first header with the same name, empty string if not found
    std::string_view header(std::string_view name) const;
    bool hasHeader(std::string_view name) const;

    void addCookie(ResponseCookie cookie);
    void addHeader(Header header);
    void setCookies(const std::vector<ResponseCookie>& cookies);
    void setHeaders(const std::vector<Header>& headers);

    std::string toString(ResponseMode mode = ResponseMode::Http) const;
    friend bool operator==(const Response& lhs, const Response& rhs);

private:
    void initBodyResponse(std::vector<detail::BodyResponseArg>&& args);
    void initRedirectResponse(std::vector<detail::RedirectResponseArg>&& args);
    void initStatusResponse(std::vector<detail::StatusResponseArg>&& args);

    std::string statusData(ResponseMode mode) const;
    std::string cookiesData() const;
    std::string headersData() const;
    void addDefaultContentTypeHeader();
    void addDefaultLocationHeader();

    bool isView() const override;
    void makeOwnStateFromView() override;

private:
    ResponseStatus status_ = ResponseStatus::_200_Ok;
    std::variant<std::string, std::string_view> body_;
    std::vector<ResponseCookie> cookies_;
    std::vector<Header> headers_;
    std::optional<Redirect> redirect_;
    std::optional<Header> defaultContentTypeHeader_;
};

} //namespace http

#endif //HOT_TEACUP_RESPONSE_H