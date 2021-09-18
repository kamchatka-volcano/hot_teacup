#pragma once
#include <hot_teacup/query.h>
#include <hot_teacup/cookie.h>
#include <hot_teacup/types.h>
#include <hot_teacup/form.h>

namespace http{

struct Request
{
    explicit Request(const std::string& method,
                     const std::string& queries = {},
                     const std::string& cookies = {},
                     const std::string& formContentTypeHeader = {},
                     const std::string& form = {});

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
    void writeFile(const std::string &name, const std::string& filePath, int index = 0) const;
    bool hasFiles() const;

private:
    Form form_;
    RequestMethod method_;
    Queries queries_;
    Cookies cookies_;

private:
    static inline const std::string valueNotFound = {};
};

}
