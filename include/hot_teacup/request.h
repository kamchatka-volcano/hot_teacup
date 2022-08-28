#pragma once
#include "query.h"
#include "cookie.h"
#include "types.h"
#include "form.h"
#include <map>
#include <string>

namespace http{
class RequestView;

struct RequestFcgiData{
    std::map<std::string, std::string> params;
    std::string stdIn;
};

class Request
{
public:
    explicit Request(const RequestView&);
    Request(RequestMethod,
            std::string path,
            std::vector<Query> = {},
            std::vector<Cookie> = {},
            Form = {});

    RequestMethod method() const;
    const std::string& path() const;

    const std::vector<Query>& queries() const;
    const std::string& query(std::string_view name) const;
    bool hasQuery(std::string_view name) const;

    const std::vector<Cookie>& cookies() const;
    const std::string& cookie(std::string_view name) const;
    bool hasCookie(std::string_view name) const;

    const Form& form() const;
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

    void setQueries(const std::vector<Query>&);
    void setCookies(const std::vector<Cookie>&);
    void setForm(const Form&);
    void setFcgiParams(const std::map<std::string, std::string>& params);

private:
    RequestMethod method_;
    std::string path_;
    std::vector<Query> queries_;
    std::vector<Cookie> cookies_;
    Form form_;
    std::map<std::string, std::string> fcgiParams_;

private:
    static inline const std::string valueNotFound = {};
};

}
