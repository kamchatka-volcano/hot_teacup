#include <hot_teacup/request.h>
#include <hot_teacup/request_view.h>
#include <algorithm>

using namespace std::string_literals;

namespace http {

Request::Request(const RequestView& requestView)
    : method_{requestView.method()}
    , path_{requestView.path()}
    , ipAddress_{requestView.ipAddress()}
    , domainName_{requestView.domainName()}
    , queries_{makeQueries(requestView.queries())}
    , cookies_{makeCookies(requestView.cookies())}
    , form_{makeForm(requestView.form())}
{
}

void Request::init(std::vector<detail::RequestArg>&& args)
{
    const auto processArg = [this](detail::RequestArg& arg)
    {
        if (std::holds_alternative<std::vector<http::Query>>(arg))
            queries_ = std::move(std::get<std::vector<http::Query>>(arg));
        else if (std::holds_alternative<std::vector<http::Cookie>>(arg))
            cookies_ = std::move(std::get<std::vector<http::Cookie>>(arg));
        else if (std::holds_alternative<Form>(arg))
            form_ = std::move(std::get<Form>(arg));
    };
    std::for_each(args.begin(), args.end(), processArg);
}

void Request::setIpAddress(const std::string& ipAddress)
{
    if (isView())
        return;

    ipAddress_ = ipAddress;
}

void Request::setDomainName(const std::string& domainName)
{
    if (isView())
        return;

    domainName_ = domainName;
}

void Request::setQueries(const std::vector<Query>& queries)
{
    if (isView())
        return;

    queries_ = queries;
}

void Request::setCookies(const std::vector<Cookie>& cookies)
{
    if (isView())
        return;

    cookies_ = cookies;
}

void Request::setForm(const Form& form)
{
    if (isView())
        return;

    form_ = form;
}

RequestMethod Request::method() const
{
    return method_;
}

std::string_view Request::path() const
{
    return std::visit([](const auto& path) -> std::string_view { return path; }, path_);
}

std::string_view Request::ipAddress() const
{
    return std::visit([](const auto& ipAddress) -> std::string_view { return ipAddress; }, ipAddress_);
}

std::string_view Request::domainName() const
{
    return std::visit([](const auto& domainName) -> std::string_view { return domainName; }, domainName_);
}

std::string_view Request::query(std::string_view name) const
{
    auto it = std::find_if(
            queries_.begin(),
            queries_.end(),
            [&name](const auto& query)
            {
                return query.name() == name;
            });
    if (it != queries_.end())
        return it->value();

    return valueNotFound;
}

bool Request::hasQuery(std::string_view name) const
{
    auto it = std::find_if(
            queries_.begin(),
            queries_.end(),
            [&name](const auto& query)
            {
                return query.name() == name;
            });
    return (it != queries_.end());
}

std::string_view Request::cookie(std::string_view name) const
{
    auto it = std::find_if(
            cookies_.begin(),
            cookies_.end(),
            [&name](const auto& cookie)
            {
                return cookie.name() == name;
            });
    if (it != cookies_.end())
        return it->value();

    return valueNotFound;
}

bool Request::hasCookie(std::string_view name) const
{
    auto it = std::find_if(
            cookies_.begin(),
            cookies_.end(),
            [&name](const auto& cookie)
            {
                return cookie.name() == name;
            });
    return (it != cookies_.end());
}

const Form& Request::form() const
{
    return form_;
}

std::string_view Request::formField(std::string_view name, int index) const
{
    auto i = 0;
    for (const auto& [formFieldName, formField] : form_) {
        if (formFieldName == name && formField.type() == FormFieldType::Param) {
            if (i++ == index)
                return formField.value();
        }
    }
    return valueNotFound;
}

int Request::formFieldCount(std::string_view name) const
{
    return static_cast<int>(std::count_if(
            form_.begin(),
            form_.end(),
            [&name](const auto& namedFormField)
            {
                const auto& [formFieldName, formField] = namedFormField;
                return formFieldName == name && formField.type() == FormFieldType::Param;
            }));
}

bool Request::hasFormField(std::string_view name) const
{
    return formFieldCount(name) != 0;
}

std::string_view Request::fileData(std::string_view name, int index) const
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

std::string_view Request::fileName(std::string_view name, int index) const
{
    auto i = 0;
    for (const auto& [formFieldName, formField] : form_)
        if (formField.hasFile() && formFieldName == name)
            if (i++ == index)
                return formField.fileName();

    return valueNotFound;
}

std::string_view Request::fileType(std::string_view name, int index) const
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

std::vector<std::string_view> Request::formFieldList() const
{
    auto result = std::vector<std::string_view>{};
    for (const auto& [formFieldName, formField] : form_)
        if (formField.type() == FormFieldType::Param)
            result.push_back(formFieldName);
    return result;
}

std::vector<std::string_view> Request::fileList() const
{
    auto result = std::vector<std::string_view>{};
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

RequestFcgiData Request::toFcgiData(FormType formType, std::map<std::string, std::string> fcgiParams) const
{
    const auto formBoundary = "----asyncgiFormBoundary"s;

    auto makeFcgiParams = [&]
    {
        fcgiParams["REQUEST_METHOD"] = methodToString(method_);
        const auto path = std::visit([](const auto& path) -> std::string_view{ return path;}, path_);
        if (!path.empty())
            fcgiParams["REQUEST_URI"] = path;
        if (!queries_.empty())
            fcgiParams["QUERY_STRING"] = queriesToString(queries_);
        if (!cookies_.empty())
            fcgiParams["HTTP_COOKIE"] = cookiesToString(cookies_);
        if (!form_.empty()) {
            if (formType == FormType::Multipart)
                fcgiParams["CONTENT_TYPE"] = "multipart/form-data; boundary=" + formBoundary;
            else
                fcgiParams["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
        }
        return fcgiParams;
    };
    auto makeFcgiStdIn = [&]
    {
        if (formType == FormType::Multipart)
            return multipartFormToString(form_, formBoundary);
        else
            return urlEncodedFormToString(form_);
    };

    return {makeFcgiParams(), makeFcgiStdIn()};
}

bool Request::isView() const
{
    return std::holds_alternative<std::string_view>(path_);
}

bool operator==(const Request& lhs, const Request& rhs)
{
    return lhs.method() == rhs.method() && lhs.ipAddress() == rhs.ipAddress() && lhs.domainName() == rhs.domainName() &&
            lhs.path() == rhs.path() && lhs.queries() == rhs.queries() && lhs.cookies() == rhs.cookies() &&
            lhs.form() == rhs.form();
}

} //namespace http
