#include <hot_teacup/request.h>
#include <algorithm>
#include <fstream>

namespace http{

Request::Request(const std::string& method,
                 const std::string& queries,
                 const std::string& cookies,
                 const std::string& formContentTypeHeader,
                 const std::string& form)
    : form_(formContentTypeHeader, form)
    , method_(detail::methodFromString(method))
    , queries_(queriesFromString(queries))
    , cookies_(cookiesFromString(cookies))
{}

RequestMethod Request::method() const
{
    return method_;
}

const std::string& Request::query(const std::string& name) const
{
    auto it = std::find_if(queries_.begin(), queries_.end(), [&name](const auto& query){
        return query.name() == name;
    });
    if (it != queries_.end())
        return it->value();

    return valueNotFound;
}

bool Request::hasQuery(const std::string& name) const
{
    auto it = std::find_if(queries_.begin(), queries_.end(), [&name](const auto& query){
        return query.name() == name;
    });
    return (it != queries_.end());
}

const std::string& Request::cookie(const std::string& name) const
{
    auto it = std::find_if(cookies_.begin(), cookies_.end(), [&name](const auto& cookie){
        return cookie.name() == name;
    });
    if (it != cookies_.end())
        return it->value();

    return valueNotFound;
}

bool Request::hasCookie(const std::string& name) const
{
    auto it = std::find_if(cookies_.begin(), cookies_.end(), [&name](const auto& cookie){
        return cookie.name() == name;
    });
    return (it != cookies_.end());
}

const std::string& Request::formField(const std::string &name, int index) const
{
    auto i = 0;
    for (const auto& formField :form_.fields()){
        if (formField.name() == name && !formField.isFile()){
            if (i++ == index)
                return formField.value();
        }
    }
    return valueNotFound;
}

int Request::formFieldCount(const std::string& name) const
{
    auto result = 0;
    for (const auto& formField : form_.fields())
        if (formField.name() == name && !formField.isFile())
            result++;
    return result;
}

bool Request::hasFormField(const std::string &name) const
{
    return formFieldCount(name) != 0;
}

const std::string& Request::fileData(const std::string &name, int index) const
{
    auto i = 0;
    for (const auto& formField : form_.fields())
        if (formField.hasFile() && formField.name() == name)
            if (i++ == index)
                return formField.value();

    return valueNotFound;
}

int Request::fileCount(const std::string &name) const
{
    auto result = 0;
    for (const auto& formField : form_.fields())
        if (formField.hasFile() && formField.name() == name)
            result++;
    return result;
}

bool Request::hasFile(const std::string &name) const
{
    return fileCount(name) != 0;
}

const std::string& Request::fileName(const std::string &name, int index) const
{
    auto i = 0;
    for (const auto& formField : form_.fields())
        if (formField.hasFile() && formField.name() == name)
            if (i++ == index)
                return formField.fileName();

    return valueNotFound;
}

const std::string& Request::fileType(const std::string &name, int index) const
{
    auto i = 0;
    for (const auto& formField : form_.fields())
        if (formField.hasFile() && formField.name() == name)
            if (i++ == index)
                return formField.fileType();

    return valueNotFound;
}

void Request::writeFile(const std::string &name, const std::string& filePath, int index) const
{
    auto i = 0;
    for (const auto& formField : form_.fields())
        if (formField.hasFile() && formField.name() == name)
            if (i++ == index){
                auto stream = std::ofstream(filePath);
                stream.write(formField.value().c_str(), static_cast<std::streamsize>(formField.value().size()));
                return;
            }
}

const Queries& Request::queries() const
{
    return queries_;
}

std::vector<std::string> Request::queryList() const
{
    auto result = std::vector<std::string>{};
    std::transform(queries_.begin(), queries_.end(), std::back_inserter(result),
                   [](const auto& query){return query.name();});
    return result;
}

const Cookies& Request::cookies() const
{
    return cookies_;
}

std::vector<std::string> Request::cookieList() const
{
    auto result = std::vector<std::string>{};
    std::transform(cookies_.begin(), cookies_.end(), std::back_inserter(result),
                   [](const auto& cookie){return cookie.name();});
    return result;
}

std::vector<std::string> Request::formFieldList() const
{
    auto result = std::vector<std::string>{};
    for (const auto& formField : form_.fields())
        if (!formField.isFile())
            result.push_back(formField.name());
    return result;
}

std::vector<std::string> Request::fileList() const
{
    auto result = std::vector<std::string>{};
    for (const auto& formField : form_.fields())
        if (formField.hasFile())
            result.push_back(formField.name());
    return result;
}

bool Request::hasFiles() const
{
    for (const auto& formField : form_.fields())
        if (formField.hasFile())
            return true;

    return false;
}

}
