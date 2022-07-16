#pragma once
#include "types.h"
#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <optional>

namespace http{
class HeaderParamView;
class HeaderView;

class HeaderParam{
public:
    explicit HeaderParam(const HeaderParamView&);
    explicit HeaderParam(std::string name);
    HeaderParam(std::string name, std::string value);
    const std::string& name() const;
    const std::string& value() const;
    std::string toString(HeaderQuotingMode quotingMode) const;

private:
    std::string name_;
    std::optional<std::string> value_;
    static inline const std::string valueNotFound;
};

class Header{
public:
    explicit Header(const HeaderView&);
    Header(std::string name, std::string value);
    void setParam(std::string name);
    void setParam(std::string name, std::string value);
    void setQuotingMode(HeaderQuotingMode mode);
    std::string toString() const;

    const std::string& name() const;
    const std::string& value() const;
    const std::string& param(std::string_view name) const;
    const std::vector<HeaderParam>& params() const;
    bool hasParam(std::string_view name) const;

private:
    std::string name_;
    std::string value_;
    std::vector<HeaderParam> params_;
    HeaderQuotingMode quotingMode_ = HeaderQuotingMode::None;
};

std::vector<HeaderParam> makeHeaderParams(const std::vector<HeaderParamView>& headerParamViewList);
std::vector<Header> makeHeaders(const std::vector<HeaderView>& headerViewList);

}
