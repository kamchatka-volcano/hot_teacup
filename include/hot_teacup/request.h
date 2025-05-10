#ifndef HOT_TEACUP_REQUEST_H
#define HOT_TEACUP_REQUEST_H

#include "cookie.h"
#include "header.h"
#include "multipart_form.h"
#include "query.h"
#include "request_view.h"
#include "trait_utils.h"
#include "types.h"
#include "url_encoded_form.h"
#include "url_encoded_form_view.h"
#include "detail/view_or_owner_interface.h"
#include <map>
#include <string>
#include <unordered_map>

namespace http {
class RequestView;

struct RequestFcgiData {
    std::map<std::string, std::string> params;
    std::string stdIn;
};

class RequestBody : public detail::IViewOrOwner {
    class Data {
    public:
        Data(Header contentType, std::string data);
        Data(MultipartForm form);
        Data(UrlEncodedForm form);
        const Header& contentType() const;
        std::string_view content() const;
        std::optional<MultipartFormView> multipartForm() const;
        std::optional<UrlEncodedFormView> urlEncodedForm() const;

    private:
        Header contentType_;
        std::string content_;
        std::optional<MultipartForm> multipartForm_;
        std::optional<UrlEncodedForm> urlEncodedForm_;
    };

public:
    RequestBody(const RequestBodyView&);
    RequestBody(std::string data);
    RequestBody(ContentType, std::string data);
    RequestBody(std::string contentTypeHeaderValue, std::string data);
    RequestBody(Header contentType, std::string data);
    RequestBody(MultipartForm form);
    RequestBody(UrlEncodedForm form);

    Header contentType() const;
    std::string_view content() const;
    std::optional<MultipartFormView> multipartForm() const;
    std::optional<UrlEncodedFormView> urlEncodedForm() const;

private:
    bool isView() const override;
    void makeOwnStateFromView() override;

private:
    std::variant<Data, RequestBodyView> data_;
};

namespace detail{
using RequestArg = std::variant<std::vector<Query>, std::vector<Cookie>, std::vector<Header>, RequestBody>;
}

class Request : public detail::IViewOrOwner {
public:
    explicit Request(const RequestView&);
    template<
            typename... TArgs,
            typename = std::enable_if_t<
                    ((!std::is_same_v<std::decay_t<TArgs>, RequestView> &&
                      !std::is_same_v<std::decay_t<TArgs>, Request>) &&
                     ...)>>
    Request(RequestMethod method, std::string path, TArgs&&... args)
        : method_{method}
        , path_{std::move(path)}
    {
        static_assert(
                !detail::has_duplicate_v<detail::decay_to_string_view_t<TArgs>...>,
                "Response constructor arguments can't contain duplicate types");

        init({std::forward<TArgs>(args)...});
    }

    RequestMethod method() const;
    std::string_view ipAddress() const;
    std::string_view domainName() const;
    std::string_view path() const;

    const std::vector<Query>& queries() const;
    // value of the first query parameter with the same name, empty string if not found
    std::string_view query(std::string_view name) const;
    bool hasQuery(std::string_view name) const;

    const std::vector<Cookie>& cookies() const;
    // value of the first cookie with the same name, empty string if not found
    std::string_view cookie(std::string_view name) const;
    bool hasCookie(std::string_view name) const;

    const std::vector<Header>& headers() const;
    // value of the first header with the same name, empty string if not found
    std::string_view header(std::string_view name) const;
    bool hasHeader(std::string_view name) const;

    std::optional<Header> contentType() const;
    std::string_view body() const;
    std::optional<MultipartFormView> multipartForm() const;
    std::optional<UrlEncodedFormView> urlEncodedForm() const;

    std::unordered_map<std::string_view, std::string_view> fcgiParams() const;
    RequestFcgiData toFcgiData(std::map<std::string, std::string> fcgiParams = {}) const;

    void setBody(const RequestBody& body);
    void setIpAddress(const std::string&);
    void setDomainName(const std::string&);
    void addCookie(Cookie cookie);
    void addQuery(Query query);
    void addHeader(Header header);
    void setCookies(const std::vector<Cookie>&);
    void setQueries(const std::vector<Query>&);
    void setHeaders(const std::vector<Header>& headers);

    friend bool operator==(const Request& lhs, const Request& rhs);

private:
    bool isView() const override;
    void makeOwnStateFromView() override;

    void init(std::vector<detail::RequestArg>&& args);
    void addDefaultContentTypeHeader();

private:
    RequestMethod method_;
    std::variant<std::string, std::string_view> path_;
    std::variant<std::string, std::string_view> ipAddress_;
    std::variant<std::string, std::string_view> domainName_;
    std::vector<Query> queries_;
    std::vector<Cookie> cookies_;
    std::vector<Header> headers_;
    std::optional<RequestBody> body_;
    std::variant<std::unordered_map<std::string_view, std::string_view>, std::unordered_map<std::string, std::string>>
            fcgiParams_;
};

} //namespace http

#endif //HOT_TEACUP_REQUEST_H
