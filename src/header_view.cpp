#include <hot_teacup/header_view.h>
#include <sfun/string_utils.h>
#include <stdexcept>
#include <utility>

namespace http {

HeaderParamView::HeaderParamView(std::string_view name)
    : name_{name}
{
}

HeaderParamView::HeaderParamView(std::string_view name, std::string_view value)
    : name_{name}
    , value_{value}
{
}

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
    for (const auto& param : params_)
        if (param.name() == name)
            return param.value();
    throw std::out_of_range{"Header doesn't contain param '" + std::string{name} + "'"};
}

bool HeaderView::hasParam(std::string_view name) const
{
    for (const auto& param : params_)
        if (param.name() == name)
            return true;
    return false;
}

namespace {
std::string_view unquoted(std::string_view str)
{
    if (sfun::starts_with(str, "\""))
        str.remove_prefix(1);
    if (sfun::ends_with(str, "\""))
        str.remove_suffix(1);
    return str;
}

std::optional<HeaderParamView> makeParam(std::string_view paramPart)
{
    if (paramPart.find('=') == std::string::npos)
        return {};
    const auto name = sfun::trim_front(sfun::before(paramPart, "=").value());
    const auto value = unquoted(sfun::after(paramPart, "=").value());
    return HeaderParamView{name, value};
}

} //namespace

std::optional<HeaderView> headerFromString(std::string_view input)
{
    const auto parts = sfun::split(input, ";", false);
    if (parts.empty())
        return {};

    if (parts[0].find(':') == std::string::npos)
        return {};

    const auto name = sfun::trim(sfun::before(parts[0], ":").value());
    const auto value = unquoted(sfun::trim_front(sfun::after(parts[0], ":").value()));
    if (name.empty())
        return {};

    const auto valueIsParam = (value.find('=') != std::string::npos);
    if (valueIsParam) {
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

    auto params = std::vector<HeaderParamView>{};
    for (auto i = 1u; i < parts.size(); ++i)
        if (const auto param = makeParam(parts[i]))
            params.emplace_back(*param);
    return HeaderView{name, value, std::move(params)};

}

std::string_view HeaderView::name() const
{
    return name_;
}

std::string_view HeaderView::value() const
{
    return value_;
}

} //namespace http
