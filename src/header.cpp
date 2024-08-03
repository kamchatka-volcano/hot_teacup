#include "utils.h"
#include <hot_teacup/header.h>
#include <hot_teacup/header_view.h>
#include <sfun/string_utils.h>
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
    if (!hasValue())
        return std::string{name()};

    switch (quotingMode) {
    case HeaderQuotingMode::ParamValue:
    case HeaderQuotingMode::AllValues:
        return sfun::join_strings(name(), "=", "\"", value(), "\"");
    default:
        return sfun::join_strings(name(), "=", value());
    }
}

bool operator==(const HeaderParam& lhs, const HeaderParam& rhs)
{
    return lhs.name() == rhs.name() && lhs.hasValue() == rhs.hasValue() && lhs.value() == rhs.value();
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
        return sfun::join_strings("\"", value, "\"");
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
    const auto paramListSeparator = [&]() -> std::string_view{
        if (!value().empty() && !params_.empty())
            return "; ";
        return {};
    }();

    const auto paramListString = [&]
    {
        const auto paramToString = [&](const HeaderParam& param)
        {
            return param.toString(quotingMode_);
        };
        const auto paramStringList = utils::transform(params_, paramToString);
        return sfun::join(paramStringList, "; ");
    }();

    return sfun::join_strings(
            name(),
            ": ",
            valueStr(value(), !params_.empty(), quotingMode_),
            paramListSeparator,
            paramListString);
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

bool operator==(const Header& lhs, const Header& rhs)
{
    return lhs.name() == rhs.name() && lhs.value() == rhs.value() && lhs.params() == rhs.params();
}

std::vector<HeaderParam> makeHeaderParams(const std::vector<HeaderParamView>& headerParamViewList)
{
    return utils::transform(
            headerParamViewList,
            [](const HeaderParamView& headerParamView)
            {
                return HeaderParam{headerParamView};
            });
}

std::vector<Header> makeHeaders(const std::vector<HeaderView>& headerViewList)
{
    return utils::transform(
            headerViewList,
            [](const HeaderView& headerView)
            {
                return Header{headerView};
            });
}

} //namespace http
