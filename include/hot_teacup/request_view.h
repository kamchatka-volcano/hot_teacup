#pragma once
#include "query_view.h"
#include "cookie_view.h"
#include "types.h"
#include "form_view.h"
#include <map>
#include <string>

namespace http{

class RequestView
{
public:
    RequestView(
            std::string_view fcgiParamRequestMethod,
            std::string_view fcgiParamRemoteAddr,
            std::string_view fcgiParamHttpHost,
            std::string_view fcgiParamRequestUri,
            std::string_view fcgiParamQueryString,
            std::string_view fcgiParamHttpCookie,
            std::string_view fcgiParamContentType,
            std::string_view fcgiStdIn);

    RequestMethod method() const;
    std::string_view ipAddress() const;
    std::string_view domainName() const;
    std::string_view path() const;

    const std::vector<QueryView>& queries() const;
    std::string_view query(std::string_view name) const;
    bool hasQuery(std::string_view name) const;

    const std::vector<CookieView>& cookies() const;
    std::string_view cookie(std::string_view name) const;
    bool hasCookie(std::string_view name) const;

    const FormView& form() const;
    std::string_view formField(std::string_view name, int index = 0) const;
    std::vector<std::string> formFieldList() const;
    std::vector<std::string> fileList() const;
    int formFieldCount(std::string_view name) const;
    bool hasFormField(std::string_view name) const;

    std::string_view fileData(std::string_view name, int index = 0) const;
    int fileCount(std::string_view name) const;
    bool hasFile(std::string_view name) const;
    std::string_view fileName(std::string_view name, int index = 0) const;
    std::string_view fileType(std::string_view name, int index = 0) const;
    bool hasFiles() const;

private:
    RequestMethod method_;
    std::string_view ipAddress_;
    std::string_view domainName_;
    std::string_view path_;
    std::vector<QueryView> queries_;
    std::vector<CookieView> cookies_;
    FormView form_;
};

}
