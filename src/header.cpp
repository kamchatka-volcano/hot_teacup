#include <hot_teacup/header.h>
#include <sfun/string_utils.h>
#include <stdexcept>
#include <utility>

namespace http{
namespace str = sfun::string_utils;

Header::Param::Param(std::string name)
    : name_{std::move(name)}
    , hasValue_{false}
{}

Header::Param::Param(std::string name, std::string value)
    : name_{std::move(name)}
    , value_{std::move(value)}
    , hasValue_{true}
{}

const std::string& Header::Param::name() const
{
    return name_;
}

const std::string& Header::Param::value() const
{
    return value_;
}

std::string Header::Param::toString(QuotingMode quotingMode) const
{
    auto res = name_;
    if (!hasValue_)
        return res;
    res += "=";
    switch (quotingMode) {
        case QuotingMode::ParamValue:
        case QuotingMode::AllValues:
            res += "\"" + value_ + "\"";
        break;
        default:
            res += value_;
    }
    return res;
}

Header::Header(std::string name, std::string value)
    : name_{std::move(name)}
    , value_{std::move(value)}
    , quotingMode_{QuotingMode::None}
{
}

void Header::setParam(std::string name)
{
    if (name.empty())
        return;

    for (auto& param : params_)
        if (param.name() == name){
            param = Param{std::move(name)};
            return;
        }
    params_.emplace_back(std::move(name));
}

void Header::setParam(std::string name, std::string value)
{
    if (name.empty())
        return;

    for (auto& param : params_)
        if (param.name() == name){
            param = Param{std::move(name), std::move(value)};
            return;
        }
    params_.emplace_back(std::move(name), std::move(value));
}

void Header::setQuotingMode(QuotingMode mode)
{
    quotingMode_ = mode;
}

namespace {
std::string valueStr(const std::string& value, bool hasParams, Header::QuotingMode quotingMode)
{
    if (value.empty()){
        if (!hasParams)
            return "\"\"";
        else
            return {};
    }

    switch (quotingMode) {
        case Header::QuotingMode::HeaderValue:
        case Header::QuotingMode::AllValues:
            return "\"" + value + "\"";
        default:
            return value;
    }
}

}

const std::vector<Header::Param>& Header::params() const
{
    return params_;
}

const std::string& Header::param(std::string_view name) const
{
    for (const auto& param: params_)
        if (param.name() == name)
            return param.value();
    throw std::out_of_range{"Header doesn't contain param '" + std::string{name} + "'"};
}

bool Header::hasParam(std::string_view name) const
{
    for (const auto& param: params_)
        if (param.name() == name)
            return true;
    return false;
}

std::string Header::toString() const
{
    auto result = name_ + ": " + valueStr(value_, !params_.empty(), quotingMode_);
    if (!value_.empty() && !params_.empty())
        result += "; ";
    for (const auto& param : params_){
        result += param.toString(quotingMode_);
        result += + "; ";
    }
    if (!params_.empty())
        result.resize(result.size() - 2); // remove last ;
    return result;
}

namespace {
std::string unquoted(std::string_view str)
{
    if (str::startsWith(str, "\""))
        str.remove_prefix(1);
    if (str::endsWith(str, "\""))
        str.remove_suffix(1);
    return std::string{str};
}

void addParamToHeader(Header& header, std::string_view paramPart)
{
    if (paramPart.find('=') == std::string::npos)
        return;
    auto name = str::trimFront(str::before(paramPart,"="));
    auto value = unquoted(str::after(paramPart,"="));
    header.setParam(std::move(name), std::move(value));
};

}

std::optional<Header> headerFromString(std::string_view input)
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
