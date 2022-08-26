#include <hot_teacup/header_view.h>
#include <sfun/string_utils.h>
#include <stdexcept>
#include <utility>

namespace http{
namespace str = sfun::string_utils;

HeaderParamView::HeaderParamView(std::string_view name)
    : name_{name}
{}

HeaderParamView::HeaderParamView(std::string_view name, std::string_view value)
    : name_{name}
    , value_{value}
{}

std::string_view HeaderParamView::name() const
{
    return name_;
}

std::string_view HeaderParamView::value() const
{
    return value_.value_or(std::string_view{});
}

bool HeaderParamView::hasValue() const
{
    return value_.has_value();
}

HeaderView::HeaderView(std::string_view name, std::string_view value, std::vector<HeaderParamView> params)
    : name_{name}
    , value_{value}
    , params_{std::move(params)}
{
}

const std::vector<HeaderParamView>& HeaderView::params() const
{
    return params_;
}

std::string_view HeaderView::param(std::string_view name) const
{
    for (const auto& param: params_)
        if (param.name() == name)
            return param.value();
    throw std::out_of_range{"Header doesn't contain param '" + std::string{name} + "'"};
}

bool HeaderView::hasParam(std::string_view name) const
{
    for (const auto& param: params_)
        if (param.name() == name)
            return true;
    return false;
}

namespace {
std::string_view unquoted(std::string_view str)
{
    if (str::startsWith(str, "\""))
        str.remove_prefix(1);
    if (str::endsWith(str, "\""))
        str.remove_suffix(1);
    return str;
}

std::optional<HeaderParamView> makeParam(std::string_view paramPart)
{
    if (paramPart.find('=') == std::string::npos)
        return {};
    auto name = str::trimFront(str::before(paramPart,"="));
    auto value = unquoted(str::after(paramPart,"="));
    return HeaderParamView{name, value};
}

}

std::optional<HeaderView> headerFromString(std::string_view input)
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
        auto params = std::vector<HeaderParamView>{};
        auto param = makeParam(value);
        if (param)
            params.emplace_back(*param);
        for (auto i = 1u; i < parts.size(); ++i) {
            param = makeParam(parts[i]);
            if (param)
                params.emplace_back(*param);
        }
        return HeaderView{name, {}, std::move(params)};
    }
    else{
        auto params = std::vector<HeaderParamView>{};
        for (auto i = 1u; i < parts.size(); ++i) {
            auto param = makeParam(parts[i]);
            if (param)
                params.emplace_back(*param);
        }
        return HeaderView{name, value, std::move(params)};
    }
}

std::string_view HeaderView::name() const
{
    return name_;
}

std::string_view HeaderView::value() const
{
    return value_;
}

}
