#include <hot_teacup/header.h>
#include <hot_teacup/header_view.h>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace http {

std::string_view HeaderParam::Data::name() const
{
    return name_;
}

std::string_view HeaderParam::Data::value() const
{
    if (value_)
        return *value_;
    return valueNotFound;
}

bool HeaderParam::Data::hasValue() const
{
    return value_.has_value();
}

HeaderParam::HeaderParam(const HeaderParamView& paramView)
    : data_{paramView}
{
}

HeaderParam::HeaderParam(std::string name)
    : data_{Data{std::move(name), {}}}
{
}

HeaderParam::HeaderParam(std::string name, std::string value)
    : data_{Data{std::move(name), std::move(value)}}
{
}

std::string_view HeaderParam::name() const
{
    return std::visit([](const auto& data){ return data.name();}, data_);
}

std::string_view HeaderParam::value() const
{
    return std::visit([](const auto& data){ return data.value();}, data_);
}

bool HeaderParam::hasValue() const
{
    return std::visit([](const auto& data){ return data.hasValue();}, data_);
}

std::string HeaderParam::toString(HeaderQuotingMode quotingMode) const
{
    auto res = std::string{name()};
    if (!hasValue())
        return res;
    res += "=";
    switch (quotingMode) {
    case HeaderQuotingMode::ParamValue:
    case HeaderQuotingMode::AllValues:
        res += "\"" + std::string{value()} + "\"";
        break;
    default:
        res += std::string{value()};
    }
    return res;
}


std::string_view Header::Data::name() const
{
    return name_;
}
std::string_view Header::Data::value() const
{
    return value_;
}

Header::Header(const HeaderView& headerView)
    : data_{headerView}
    , params_{makeHeaderParams(headerView.params())}
{
}

Header::Header(std::string name, std::string value)
    : data_{Data{std::move(name), std::move(value)}}
{
}

void Header::setParam(std::string name)
{
    if (isView())
        return;

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
    if (isView())
        return;

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
std::string valueStr(std::string_view value, bool hasParams, HeaderQuotingMode quotingMode)
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
        return "\"" + std::string{value} + "\"";
    default:
        return std::string{value};
    }
}

} //namespace

const std::vector<HeaderParam>& Header::params() const
{
    return params_;
}

std::string_view Header::param(std::string_view name) const
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
    auto result = std::string{name()} + ": " + valueStr(value(), !params_.empty(), quotingMode_);
    if (!value().empty() && !params_.empty())
        result += "; ";
    for (const auto& param : params_) {
        result += param.toString(quotingMode_);
        result += +"; ";
    }
    if (!params_.empty())
        result.resize(result.size() - 2); // remove last ;
    return result;
}

std::string_view Header::name() const
{
    return std::visit([](const auto& data){ return data.name();}, data_);
}

std::string_view Header::value() const
{
    return std::visit([](const auto& data){ return data.value();}, data_);
}

bool Header::isView() const
{
    return std::holds_alternative<HeaderView>(data_);
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
