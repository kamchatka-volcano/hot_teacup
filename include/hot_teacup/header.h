#pragma once
#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <optional>

namespace http{

class Header{
public:
    enum class QuotingMode {
        None,
        HeaderValue,
        ParamValue,
        AllValues
    };

private:
    class Param {
    public:
        explicit Param(std::string name);
        Param(std::string name, std::string value);
        const std::string& name() const;
        const std::string& value() const;
        std::string toString(QuotingMode quotingMode) const;

    private:
        std::string name_;
        std::string value_;
        bool hasValue_;
    };

public:
    Header(std::string name, std::string value);
    void setParam(std::string name);
    void setParam(std::string name, std::string value);
    void setQuotingMode(QuotingMode mode);
    std::string toString() const;

    const std::string& name() const;
    const std::string& value() const;
    const std::string& param(std::string_view name) const;
    const std::vector<Param>& params() const;
    bool hasParam(std::string_view name) const;

private:
    std::string name_;
    std::string value_;
    std::vector<Param> params_;
    QuotingMode quotingMode_ = QuotingMode::None;
};

using Headers = std::vector<Header>;
std::optional<Header> headerFromString(std::string_view);

}
