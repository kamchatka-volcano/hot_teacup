#pragma once
#include "query.h"
#include "cookie.h"
#include "types.h"
#include "form.h"
#include <map>
#include <string>

namespace http{

struct RequestFcgiData{
    std::map<std::string, std::string> params;
    std::string stdIn;
};

class Request
{
public:
    Request(std::string_view fcgiParamRequestMethod,
            std::string_view fcgiParamRemoteAddr,
            std::string_view fcgiParamHttpHost,
            std::string_view fcgiParamRequestUri,
            std::string_view fcgiParamQueryString,
            std::string_view fcgiParamHttpCookie,
            std::string_view fcgiParamContentType,
            std::string_view fcgiStdIn);

    explicit Request(RequestMethod, std::string path, Queries = {}, Cookies = {}, Form = {});

    void setIpAddress(const std::string& ipAddress);
    void setDomain(const std::string& domain);

    RequestMethod method() const;
    const std::string& ipAddress() const;
    const std::string& domainName() const;
    const std::string& path() const;

    const Queries& queries() const;
    const std::string& query(std::string_view name) const;
    bool hasQuery(std::string_view name) const;

    const Cookies& cookies() const;
    const std::string& cookie(std::string_view name) const;
    bool hasCookie(std::string_view name) const;

    const std::string& formField(std::string_view name, int index = 0) const;
    std::vector<std::string> formFieldList() const;
    std::vector<std::string> fileList() const;
    int formFieldCount(std::string_view name) const;
    bool hasFormField(std::string_view name) const;

    const std::string& fileData(std::string_view name, int index = 0) const;
    int fileCount(std::string_view name) const;
    bool hasFile(std::string_view name) const;
    const std::string& fileName(std::string_view name, int index = 0) const;
    const std::string& fileType(std::string_view name, int index = 0) const;
    bool hasFiles() const;

    RequestFcgiData toFcgiData(FormType) const;

private:
    RequestMethod method_;
    std::string ipAddress_;
    std::string domainName_;
    std::string path_;
    Queries queries_;
    Cookies cookies_;
    Form form_;

private:
    static inline const std::string valueNotFound = {};
};

}
