#pragma once
#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <optional>

namespace http{

class HeaderParamView {
public:
    explicit HeaderParamView(std::string_view name);
    HeaderParamView(std::string_view name, std::string_view value);
    std::string_view name() const;
    std::string_view value() const;
    bool hasValue() const;

private:
    std::string_view name_;
    std::optional<std::string_view> value_;
};

class HeaderView{
public:
    HeaderView(std::string_view name, std::string_view value, std::vector<HeaderParamView> params);
    std::string_view name() const;
    std::string_view value() const;
    std::string_view param(std::string_view name) const;
    const std::vector<HeaderParamView>& params() const;
    bool hasParam(std::string_view name) const;

private:
    std::string_view name_;
    std::string_view value_;
    std::vector<HeaderParamView> params_;
};

std::optional<HeaderView> headerFromString(std::string_view);

}
