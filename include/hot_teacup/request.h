#pragma once
#include <hot_teacup/query.h>
#include <hot_teacup/cookie.h>
#include <hot_teacup/types.h>
#include <hot_teacup/form.h>

namespace http{

struct Request
{
    ///
    /// \param requestMethod - "GET", "POST", etc.
    /// \param queryString - "param=foo&param2=bar"
    /// \param cookieHeaderValue - "name=value; name2=value2; name3=value3"
    /// \param formContentTypeHeader - "Content-Type: multipart/form-data; boundary=something"
    /// \param formContent - "part of the request following the Content-Type header of the form"
    explicit Request(std::string_view requestMethod,
                     std::string_view queryString = {},
                     std::string_view cookieHeaderValue = {},
                     std::string_view formContentTypeHeader = {},
                     std::string_view formContent = {});

    RequestMethod method() const;
    const Queries& queries() const;
    std::vector<std::string> queryList() const;
    const std::string& query(const std::string& name) const;
    bool hasQuery(const std::string& name) const;
    const std::string& cookie(const std::string& name) const;

    const Cookies& cookies() const;
    std::vector<std::string> cookieList() const;
    bool hasCookie(const std::string& name) const;
    const std::string& formField(const std::string &name, int index = 0) const;

    std::vector<std::string> formFieldList() const;
    std::vector<std::string> fileList() const;
    int formFieldCount(const std::string& name) const;
    bool hasFormField(const std::string &name) const;

    const std::string& fileData(const std::string &name, int index = 0) const;
    int fileCount(const std::string &name) const;
    bool hasFile(const std::string &name) const;
    const std::string& fileName(const std::string &name, int index = 0) const;
    const std::string& fileType(const std::string &name, int index = 0) const;
    bool hasFiles() const;

private:
    RequestMethod method_;
    Queries queries_;
    Cookies cookies_;
    Form form_;

private:
    static inline const std::string valueNotFound = {};
};

}
