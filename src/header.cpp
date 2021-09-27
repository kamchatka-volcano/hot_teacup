#include <hot_teacup/header.h>
#include <sfun/string_utils.h>
#include <stdexcept>
#include <utility>

namespace http{
namespace str = sfun::string_utils;

Header::Header(std::string name, std::string value)
    : name_(std::move(name))
    , value_(std::move(value))
    , quotingMode_(QuotingMode::None)
{
}

void Header::setParam(std::string name, std::string value)
{
    if (name.empty())
        return;

    for (auto& param : params_)
        if (param.name == name){
            param.value = std::move(value);
            return;
        }

    params_.push_back({std::move(name), std::move(value)});
}

void Header::setQuotingMode(QuotingMode mode)
{
    quotingMode_ = mode;
}

std::string Header::valueStr(const std::string& value, bool hasParams) const
{
    if (value.empty()){
        if (!hasParams)
            return "\"\"";
        else
            return {};
    }

    switch (quotingMode_){
    case QuotingMode::HeaderValue:
    case QuotingMode::AllValues:
        return "\"" + value + "\"";
    default: return value;
    }
}


std::string Header::paramValueStr(const std::string& value) const
{
    switch (quotingMode_){
    case QuotingMode::ParamValue:
    case QuotingMode::AllValues:
        return "\"" + value + "\"";
    default: return value;
    }
}

std::vector<std::string> Header::paramList() const
{
    auto result = std::vector<std::string>{};
    for (const auto& param : params_)
        result.push_back(param.name);
    return result;
}

const std::string& Header::param(const std::string& name) const
{
    for (const auto& param: params_)
        if (param.name == name)
            return param.value;
    throw std::out_of_range{"Header doesn't contain param '" + name + "'"};
}

bool Header::hasParam(const std::string &name) const
{
    for (const auto& param: params_)
        if (param.name == name)
            return true;
    return false;
}

std::string Header::toString() const
{
    auto result = name_ + ": " + valueStr(value_, !params_.empty());
    if (!value_.empty() && !params_.empty())
        result += "; ";
    for (const auto& param : params_){
        result += param.name;
        if (!param.value.empty())
            result += "=" + paramValueStr(param.value);
        result += + "; ";
    }
    if (!params_.empty())
        result.resize(result.size() - 2); // remove last ;
    return result;
}

namespace {
std::string unquoted(const std::string& str)
{
    auto res = std::string(str);
    if (str::startsWith(res, "\""))
        res.erase(0,1);
    if (str::endsWith(res, "\""))
        res.resize(res.size() - 1);
    return res;
}

void addParamToHeader(Header& header, const std::string& paramPart)
{
    if (paramPart.find('=') == std::string::npos)
        return;
    auto name = str::trimFront(str::before(paramPart,"="));
    auto value = unquoted(str::after(paramPart,"="));
    header.setParam(name, value);
};

}

std::optional<Header> headerFromString(const std::string& input)
{
    const auto parts = str::split(input, ";", false);
    if (parts.empty())
        return {};

    if (parts[0].find(':') == std::string::npos)
        return {};

    auto name = str::trim(str::before(parts[0], ":"));
    auto value = unquoted(str::trimFront(str::after(parts[0], ":")));
    if (name.empty())
        return {};

    const auto valueIsParam = (value.find('=') != std::string::npos);
    if (valueIsParam){
        auto header = Header{std::move(name), ""};
        addParamToHeader(header, value);
        for (auto i = 1u; i < parts.size(); ++i)
            addParamToHeader(header, parts[i]);
        return header;
    }
    else{
        auto header = Header{std::move(name), std::move(value)};
        for (auto i = 1u; i < parts.size(); ++i)
            addParamToHeader(header, parts[i]);
        return header;
    }
}

const std::string& Header::name() const
{
    return name_;
}

const std::string& Header::value() const
{
    return value_;
}

}
