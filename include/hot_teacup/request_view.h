#ifndef HOT_TEACUP_REQUEST_VIEW_H
#define HOT_TEACUP_REQUEST_VIEW_H

#include "header_view.h"
#include "multipart_form_view.h"
#include "query_view.h"
#include "request_cookie_view.h"
#include "types.h"
#include "url_encoded_form.h"
#include "url_encoded_form_view.h"
#include "detail/lazy_initialized.h"
#include <unordered_map>

namespace http {

class RequestBodyView {
public:
    RequestBodyView(HeaderView contentType, std::string_view content);
    HeaderView contentType() const;
    std::string_view content() const;
    std::optional<MultipartFormView> multipartForm() const;
    std::optional<UrlEncodedFormView> urlEncodedForm() const;

    friend bool operator==(const RequestBodyView& lhs, const RequestBodyView& rhs);

private:
    std::optional<MultipartFormView> createMultipartForm() const;
    std::optional<UrlEncodedForm> createUrlEncodedForm() const;

private:
    HeaderView contentType_;
    std::string_view content_;
    detail::LazyInitialized<RequestBodyView, &RequestBodyView::createMultipartForm, std::optional<MultipartFormView>>
            multipartForm_;
    detail::LazyInitialized<RequestBodyView, &RequestBodyView::createUrlEncodedForm, std::optional<UrlEncodedForm>>
            urlEncodedForm_;
};

class RequestView {
public:
    RequestView(
            std::string_view fcgiParamRequestMethod,
            std::string_view fcgiParamRemoteAddr,
            std::string_view fcgiParamHttpHost,
            std::string_view fcgiParamRequestUri,
            std::string_view fcgiParamQueryString,
            std::string_view fcgiParamHttpCookie,
            std::string_view fcgiParamContentType,
            std::string_view fcgiStdIn,
            std::unordered_map<std::string_view, std::string_view> fcgiParams);

    RequestMethod method() const;
    std::string_view ipAddress() const;
    std::string_view domainName() const;
    std::string_view path() const;

    const std::vector<QueryView>& queries() const;
    std::string_view query(std::string_view name) const;
    bool hasQuery(std::string_view name) const;

    const std::vector<RequestCookieView>& cookies() const;
    std::string_view cookie(std::string_view name) const;
    bool hasCookie(std::string_view name) const;

    std::optional<HeaderView> contentType() const;
    std::string_view body() const;
    std::optional<MultipartFormView> multipartForm() const;
    std::optional<UrlEncodedFormView> urlEncodedForm() const;

    const std::unordered_map<std::string_view, std::string_view>& fcgiParams() const;
    std::optional<RequestBodyView> getRequestBody() const;
    friend bool operator==(const RequestView& lhs, const RequestView& rhs);

private:
    RequestMethod method_;
    std::string_view ipAddress_;
    std::string_view domainName_;
    std::string_view path_;
    std::vector<QueryView> queries_;
    std::vector<RequestCookieView> cookies_;
    std::optional<RequestBodyView> body_;
    std::unordered_map<std::string_view, std::string_view> fcgiParams_;
};

} //namespace http

#endif //HOT_TEACUP_REQUEST_VIEW_H
