#include <hot_teacup/request_view.h>
#include <sfun/string_utils.h>
#include <algorithm>

using namespace std::string_literals;

namespace http {

RequestView::RequestView(
        std::string_view fcgiParamRequestMethod,
        std::string_view fcgiParamRemoteAddr,
        std::string_view fcgiParamHttpHost,
        std::string_view fcgiParamRequestUri,
        std::string_view fcgiParamQueryString,
        std::string_view fcgiParamHttpCookie,
        std::string_view fcgiParamContentType,
        std::string_view fcgiStdIn)
    : method_{methodFromString(fcgiParamRequestMethod)}
    , ipAddress_{fcgiParamRemoteAddr}
    , domainName_{sfun::before(fcgiParamHttpHost, ":").value_or(fcgiParamHttpHost)}
    , path_{sfun::before(fcgiParamRequestUri, "?").value_or(fcgiParamRequestUri)}
    , queries_{queriesFromString(fcgiParamQueryString)}
    , cookies_{cookiesFromString(fcgiParamHttpCookie)}
    , form_{formFromString(fcgiParamContentType, fcgiStdIn)}
{
}

RequestMethod RequestView::method() const
{
    return method_;
}

std::string_view RequestView::ipAddress() const
{
    return ipAddress_;
}

std::string_view RequestView::domainName() const
{
    return domainName_;
}

std::string_view RequestView::path() const
{
    return path_;
}

std::string_view RequestView::query(std::string_view name) const
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

    return {};
}

bool RequestView::hasQuery(std::string_view name) const
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

std::string_view RequestView::cookie(std::string_view name) const
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

    return {};
}

bool RequestView::hasCookie(std::string_view name) const
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

const FormView& RequestView::form() const
{
    return form_;
}

std::string_view RequestView::formField(std::string_view name, int index) const
{
    auto i = 0;
    for (const auto& [formFieldName, formField] : form_) {
        if (formFieldName == name && formField.type() == FormFieldType::Param) {
            if (i++ == index)
                return formField.value();
        }
    }
    return {};
}

int RequestView::formFieldCount(std::string_view name) const
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

bool RequestView::hasFormField(std::string_view name) const
{
    return formFieldCount(name) != 0;
}

std::string_view RequestView::fileData(std::string_view name, int index) const
{
    auto i = 0;
    for (const auto& [formFieldName, formField] : form_)
        if (formField.hasFile() && formFieldName == name)
            if (i++ == index)
                return formField.value();

    return {};
}

int RequestView::fileCount(std::string_view name) const
{
    auto result = 0;
    for (const auto& [formFieldName, formField] : form_)
        if (formField.hasFile() && formFieldName == name)
            result++;
    return result;
}

bool RequestView::hasFile(std::string_view name) const
{
    return fileCount(name) != 0;
}

std::string_view RequestView::fileName(std::string_view name, int index) const
{
    auto i = 0;
    for (const auto& [formFieldName, formField] : form_)
        if (formField.hasFile() && formFieldName == name)
            if (i++ == index)
                return formField.fileName();

    return {};
}

std::string_view RequestView::fileType(std::string_view name, int index) const
{
    auto i = 0;
    for (const auto& [formFieldName, formField] : form_)
        if (formField.hasFile() && formFieldName == name)
            if (i++ == index)
                return formField.fileType();

    return {};
}

const std::vector<QueryView>& RequestView::queries() const
{
    return queries_;
}

const std::vector<CookieView>& RequestView::cookies() const
{
    return cookies_;
}

std::vector<std::string_view> RequestView::formFieldList() const
{
    auto result = std::vector<std::string_view>{};
    for (const auto& [formFieldName, formField] : form_)
        if (formField.type() == FormFieldType::Param)
            result.push_back(formFieldName);
    return result;
}

std::vector<std::string_view> RequestView::fileList() const
{
    auto result = std::vector<std::string_view>{};
    for (const auto& [formFieldName, formField] : form_)
        if (formField.hasFile())
            result.push_back(formFieldName);
    return result;
}

bool RequestView::hasFiles() const
{
    return std::any_of(
            form_.begin(),
            form_.end(),
            [](const auto& formFieldPair)
            {
                return formFieldPair.second.hasFile();
            });
}

} //namespace http
