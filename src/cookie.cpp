#include <hot_teacup/cookie.h>
#include <sfun/string_utils.h>

namespace http{
namespace str = sfun::string_utils;

Cookie::Cookie(std::string name,
               std::string value,
               int maxAgeSec,
               std::string domain,
               std::string path)
    : header_("Set-Cookie", "")
    , name_(std::move(name))
    , value_(std::move(value))
{
    header_.setParam(name_, value_);
    if (maxAgeSec >= 0)
        header_.setParam("Max-Age", std::to_string(maxAgeSec));
    if (!domain.empty())
        header_.setParam("Domain", std::move(domain));
    if (!path.empty())
        header_.setParam("Path", std::move(path));
    header_.setParam("Version", "1");
}

const std::string& Cookie::name() const
{
    return name_;
}

const std::string& Cookie::value() const
{
    return value_;
}

void Cookie::setDomain(std::string domain)
{
    if (!domain.empty())
        header_.setParam("Domain", std::move(domain));
}
void Cookie::setPath(std::string path)
{
    if (!path.empty())
        header_.setParam("Path", std::move(path));
}

void Cookie::setMaxAge(int maxAgeSec)
{
    if (maxAgeSec >= 0)
        header_.setParam("Max-Age", std::to_string(maxAgeSec));
}

void Cookie::remove()
{
    header_.setParam("Expires", "Fri, 01-Jan-1971 01:00:00 GMT");
}

void Cookie::secure()
{
    header_.setParam("Secure", "");
}

std::string Cookie::toString() const
{
    return header_.toString();
}

bool Cookie::operator==(const Cookie& other) const
{
    return name_ == other.name_ &&
           value_ == other.value_;
}

Cookies cookiesFromString(std::string_view input)
{
    auto result = Cookies{};
    std::vector<std::string> cookies = str::split(input, ";");
    for(const std::string& cookie : cookies){
        auto name = str::before(cookie, "=");
        auto value = str::after(cookie, "=");
        if (!name.empty() && !value.empty())
            result.emplace_back(str::trim(name), str::trim(value));
    }
    return result;
}

}
