#ifndef HOT_TEACUP_REQUEST_H
#define HOT_TEACUP_REQUEST_H

#include "cookie.h"
#include "form.h"
#include "query.h"
#include "types.h"
#include <map>
#include <string>

namespace http {
class RequestView;

struct RequestFcgiData {
    std::map<std::string, std::string> params;
    std::string stdIn;
};

class Request {
public:
    explicit Request(const RequestView&);
    Request(RequestMethod, std::string path, std::vector<Query> = {}, std::vector<Cookie> = {}, Form = {});

    RequestMethod method() const;
    std::string_view ipAddress() const;
    std::string_view domainName() const;
    std::string_view path() const;

    const std::vector<Query>& queries() const;
    std::string_view query(std::string_view name) const;
    bool hasQuery(std::string_view name) const;

    const std::vector<Cookie>& cookies() const;
    std::string_view cookie(std::string_view name) const;
    bool hasCookie(std::string_view name) const;

    const Form& form() const;
    std::string_view formField(std::string_view name, int index = 0) const;
    std::vector<std::string_view> formFieldList() const;
    std::vector<std::string_view> fileList() const;
    int formFieldCount(std::string_view name) const;
    bool hasFormField(std::string_view name) const;

    std::string_view fileData(std::string_view name, int index = 0) const;
    int fileCount(std::string_view name) const;
    bool hasFile(std::string_view name) const;
    std::string_view fileName(std::string_view name, int index = 0) const;
    std::string_view fileType(std::string_view name, int index = 0) const;
    bool hasFiles() const;

    RequestFcgiData toFcgiData(FormType, std::map<std::string, std::string> fcgiParams = {}) const;

    void setIpAddress(const std::string&);
    void setDomainName(const std::string&);
    void setQueries(const std::vector<Query>&);
    void setCookies(const std::vector<Cookie>&);
    void setForm(const Form&);

    friend bool operator==(const Request& lhs, const Request& rhs);

private:
    bool isView() const;

private:
    RequestMethod method_;
    std::variant<std::string, std::string_view> path_;
    std::variant<std::string, std::string_view> ipAddress_;
    std::variant<std::string, std::string_view> domainName_;
    std::vector<Query> queries_;
    std::vector<Cookie> cookies_;
    Form form_;

private:
    static inline const std::string valueNotFound = {};
};

} //namespace http

#endif //HOT_TEACUP_REQUEST_H
