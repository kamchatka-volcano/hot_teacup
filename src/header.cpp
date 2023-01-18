#include <hot_teacup/header.h>
#include <hot_teacup/header_view.h>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace http {

HeaderParam::HeaderParam(const HeaderParamView& paramView)
    : name_{paramView.name()}
{
    if (paramView.hasValue())
        value_ = std::string{paramView.value()};
}

HeaderParam::HeaderParam(std::string name)
    : name_{std::move(name)}
{
}

HeaderParam::HeaderParam(std::string name, std::string value)
    : name_{std::move(name)}
    , value_{std::move(value)}
{
}

const std::string& HeaderParam::name() const
{
    return name_;
}

const std::string& HeaderParam::value() const
{
    if (value_)
        return *value_;
    return valueNotFound;
}

std::string HeaderParam::toString(HeaderQuotingMode quotingMode) const
{
    auto res = name_;
    if (!value_)
        return res;
    res += "=";
    switch (quotingMode) {
    case HeaderQuotingMode::ParamValue:
    case HeaderQuotingMode::AllValues:
        res += "\"" + *value_ + "\"";
        break;
    default:
        res += *value_;
    }
    return res;
}

Header::Header(const HeaderView& headerView)
    : name_{headerView.name()}
    , value_{headerView.value()}
    , params_{makeHeaderParams(headerView.params())}
{
}

Header::Header(std::string name, std::string value)
    : name_{std::move(name)}
    , value_{std::move(value)}
{
}

void Header::setParam(std::string name)
{
    if (name.empty())
        return;

    for (auto& param : params_)
        if (param.name() == name) {
            param = HeaderParam{std::move(name)};
            return;
        }
    params_.emplace_back(std::move(name));
}

void Header::setParam(std::string name, std::string value)
{
    if (name.empty())
        return;

    for (auto& param : params_)
        if (param.name() == name) {
            param = HeaderParam{std::move(name), std::move(value)};
            return;
        }
    params_.emplace_back(std::move(name), std::move(value));
}

void Header::setQuotingMode(HeaderQuotingMode mode)
{
    quotingMode_ = mode;
}

namespace {
std::string valueStr(const std::string& value, bool hasParams, HeaderQuotingMode quotingMode)
{
    if (value.empty()) {
        if (!hasParams)
            return "\"\"";
        else
            return {};
    }

    switch (quotingMode) {
    case HeaderQuotingMode::HeaderValue:
    case HeaderQuotingMode::AllValues:
        return "\"" + value + "\"";
    default:
        return value;
    }
}

} //namespace

const std::vector<HeaderParam>& Header::params() const
{
    return params_;
}

const std::string& Header::param(std::string_view name) const
{
    for (const auto& param : params_)
        if (param.name() == name)
            return param.value();
    throw std::out_of_range{"Header doesn't contain param '" + std::string{name} + "'"};
}

bool Header::hasParam(std::string_view name) const
{
    for (const auto& param : params_)
        if (param.name() == name)
            return true;
    return false;
}

std::string Header::toString() const
{
    auto result = name_ + ": " + valueStr(value_, !params_.empty(), quotingMode_);
    if (!value_.empty() && !params_.empty())
        result += "; ";
    for (const auto& param : params_) {
        result += param.toString(quotingMode_);
        result += +"; ";
    }
    if (!params_.empty())
        result.resize(result.size() - 2); // remove last ;
    return result;
}

const std::string& Header::name() const
{
    return name_;
}

const std::string& Header::value() const
{
    return value_;
}

std::vector<HeaderParam> makeHeaderParams(const std::vector<HeaderParamView>& headerParamViewList)
{
    auto result = std::vector<HeaderParam>{};
    std::transform(
            headerParamViewList.begin(),
            headerParamViewList.end(),
            std::back_inserter(result),
            [](const auto& headerParamView)
            {
                return HeaderParam{headerParamView};
            });
    return result;
}

std::vector<Header> makeHeaders(const std::vector<HeaderView>& headerViewList)
{
    auto result = std::vector<Header>{};
    std::transform(
            headerViewList.begin(),
            headerViewList.end(),
            std::back_inserter(result),
            [](const auto& headerView)
            {
                return Header{headerView};
            });
    return result;
}

} //namespace http
