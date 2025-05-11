#include <hot_teacup/header.h>

#include "utils.h"
#include <hot_teacup/header_view.h>
#include <sfun/string_utils.h>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace http {

std::string_view Header::Data::name() const
{
    return name_;
}
std::string_view Header::Data::value() const
{
    return value_;
}

namespace {
std::vector<HeaderParam> makeHeaderParams(const std::vector<HeaderParamView>& headerParamViewList)
{
    return utils::transform(
            headerParamViewList,
            [](const HeaderParamView& headerParamView)
            {
                return HeaderParam{headerParamView};
            });
}
} //namespace

Header::Header(const HeaderView& headerView)
    : data_{headerView}
    , params_{makeHeaderParams(headerView.params())}
{
}

Header::Header(std::string name, std::string value, std::vector<HeaderParam> params)
    : data_{Data{std::move(name), std::move(value)}}
    , params_(std::move(params))
{
}

namespace {
HeaderParam makeHeaderParam(std::string name, std::optional<std::string> value)
{
    return value.has_value() ? HeaderParam{std::move(name), std::move(value.value())} : HeaderParam{std::move(name)};
}
} //namespace

void Header::setParam(std::string name, std::optional<std::string> value)
{
    if (isView())
        makeOwnStateFromView();

    if (name.empty())
        return;

    for (auto& param : params_)
        if (param.name() == name) {
            param = makeHeaderParam(std::move(name), std::move(value));
            return;
        }
    params_.emplace_back(makeHeaderParam(std::move(name), std::move(value)));
}

void Header::setParams(const std::vector<HeaderParam>& params)
{
    if (isView())
        makeOwnStateFromView();

    params_ = params;
    for (auto& param : params_)
        static_cast<IViewOrOwner&>(param).makeOwnStateFromView();
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

    return {};
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
    const auto paramListSeparator = [&]() -> std::string_view
    {
        if (!value().empty() && !params_.empty())
            return "; ";
        return {};
    }();

    const auto paramListString = [&]
    {
        const auto paramToString = [&](const HeaderParam& param)
        {
            return headerParamToString(param, quotingMode_);
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

void Header::makeOwnStateFromView()
{
    if (!isView())
        return;

    const auto& headerView = std::get<HeaderView>(data_);
    data_ = Data{std::string{headerView.name()}, std::string{headerView.value()}};
    for (auto& param : params_)
        static_cast<IViewOrOwner&>(param).makeOwnStateFromView();
}

bool operator==(const Header& lhs, const Header& rhs)
{
    return lhs.name() == rhs.name() && lhs.value() == rhs.value() && lhs.params() == rhs.params();
}

std::string headerParamToString(const HeaderParam& param, HeaderQuotingMode quotingMode)
{
    if (!param.hasValue())
        return std::string{param.name()};

    switch (quotingMode) {
    case HeaderQuotingMode::ParamValue:
    case HeaderQuotingMode::AllValues:
        return sfun::join_strings(param.name(), "=", "\"", param.value(), "\"");
    default:
        return sfun::join_strings(param.name(), "=", param.value());
    }
}

} //namespace http
