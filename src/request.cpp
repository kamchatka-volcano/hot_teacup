#include <hot_teacup/request.h>
#include <hot_teacup/request_view.h>
#include <algorithm>


using namespace std::string_literals;

namespace http {

Request::Request(const RequestView& requestView)
    : method_{requestView.method()}
    , path_{requestView.path()}
    , queries_{makeQueries(requestView.queries())}
    , cookies_{makeCookies(requestView.cookies())}
    , form_{makeForm(requestView.form())}
{
}

Request::Request(
        RequestMethod method,
        std::string path,
        std::vector<Query> queries,
        std::vector<Cookie> cookies,
        Form form)
    : method_{method},
      path_{std::move(path)},
      queries_{std::move(queries)},
      cookies_{std::move(cookies)},
      form_{std::move(form)}
{
}

void Request::setQueries(const std::vector<Query>& queries)
{
    queries_ = queries;
}

void Request::setCookies(const std::vector<Cookie>& cookies)
{
    cookies_ = cookies;
}

void Request::setForm(const Form& form)
{
    form_ = form;
}

void Request::setFcgiParams(const std::map<std::string, std::string>& params)
{
    fcgiParams_ = params;
}

RequestMethod Request::method() const
{
    return method_;
}

const std::string& Request::path() const
{
    return path_;
}

const std::string& Request::query(std::string_view name) const
{
    auto it = std::find_if(queries_.begin(), queries_.end(), [&name](const auto& query) {
        return query.name() == name;
    });
    if (it != queries_.end())
        return it->value();

    return valueNotFound;
}

bool Request::hasQuery(std::string_view name) const
{
    auto it = std::find_if(queries_.begin(), queries_.end(), [&name](const auto& query) {
        return query.name() == name;
    });
    return (it != queries_.end());
}

const std::string& Request::cookie(std::string_view name) const
{
    auto it = std::find_if(cookies_.begin(), cookies_.end(), [&name](const auto& cookie) {
        return cookie.name() == name;
    });
    if (it != cookies_.end())
        return it->value();

    return valueNotFound;
}

bool Request::hasCookie(std::string_view name) const
{
    auto it = std::find_if(cookies_.begin(), cookies_.end(), [&name](const auto& cookie) {
        return cookie.name() == name;
    });
    return (it != cookies_.end());
}

const Form& Request::form() const
{
    return form_;
}

const std::string& Request::formField(std::string_view name, int index) const
{
    auto i = 0;
    for (const auto& [formFieldName, formField] : form_){
        if (formFieldName == name && formField.type() == FormFieldType::Param){
            if (i++ == index)
                return formField.value();
        }
    }
    return valueNotFound;
}

int Request::formFieldCount(std::string_view name) const
{
    return static_cast<int>(std::count_if(form_.begin(), form_.end(), [&name](const auto& namedFormField){
        const auto& [formFieldName, formField] = namedFormField;
        return formFieldName == name && formField.type() == FormFieldType::Param;
    }));
}

bool Request::hasFormField(std::string_view name) const
{
    return formFieldCount(name) != 0;
}

const std::string& Request::fileData(std::string_view name, int index) const
{
    auto i = 0;
    for (const auto& [formFieldName, formField] : form_)
        if (formField.hasFile() && formFieldName == name)
            if (i++ == index)
                return formField.value();

    return valueNotFound;
}

int Request::fileCount(std::string_view name) const
{
    auto result = 0;
    for (const auto& [formFieldName, formField] : form_)
        if (formField.hasFile() && formFieldName == name)
            result++;
    return result;
}

bool Request::hasFile(std::string_view name) const
{
    return fileCount(name) != 0;
}

const std::string& Request::fileName(std::string_view name, int index) const
{
    auto i = 0;
    for (const auto& [formFieldName, formField] : form_)
        if (formField.hasFile() && formFieldName == name)
            if (i++ == index)
                return formField.fileName();

    return valueNotFound;
}

const std::string& Request::fileType(std::string_view name, int index) const
{
    auto i = 0;
    for (const auto& [formFieldName, formField] : form_)
        if (formField.hasFile() && formFieldName == name)
            if (i++ == index)
                return formField.fileType();

    return valueNotFound;
}

const std::vector<Query>& Request::queries() const
{
    return queries_;
}

const std::vector<Cookie>& Request::cookies() const
{
    return cookies_;
}

std::vector<std::string> Request::formFieldList() const
{
    auto result = std::vector<std::string>{};
    for (const auto& [formFieldName, formField] : form_)
        if (formField.type() == FormFieldType::Param)
            result.push_back(formFieldName);
    return result;
}

std::vector<std::string> Request::fileList() const
{
    auto result = std::vector<std::string>{};
    for (const auto& [formFieldName, formField] : form_)
        if (formField.hasFile())
            result.push_back(formFieldName);
    return result;
}

bool Request::hasFiles() const
{
    for (const auto& [formFieldName, formField] : form_)
        if (formField.hasFile())
            return true;

    return false;
}

RequestFcgiData Request::toFcgiData(FormType formType) const
{
    const auto formBoundary = "----asyncgiFormBoundary"s;

    auto makeFcgiParams = [&] {
        auto res = fcgiParams_;
        res["REQUEST_METHOD"] = methodToString(method_);
        if (!path_.empty())
            res["REQUEST_URI"] = path_;
        if (!queries_.empty())
            res["QUERY_STRING"] = queriesToString(queries_);
        if (!cookies_.empty())
            res["HTTP_COOKIE"] = cookiesToString(cookies_);
        if (!form_.empty()) {
            if (formType == FormType::Multipart)
                res["CONTENT_TYPE"] = "multipart/form-data; boundary=" + formBoundary;
            else
                res["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
        }
        return res;
    };
    auto makeFcgiStdIn = [&] {
        if (formType == FormType::Multipart)
            return multipartFormToString(form_, formBoundary);
        else
            return urlEncodedFormToString(form_);
    };

    return {makeFcgiParams(), makeFcgiStdIn()};
}


}
