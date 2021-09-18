#pragma once
#include <string>
#include <map>
#include <vector>

namespace http{

class Header{

public:
    enum class QuotingMode{
        None,
        HeaderValue,
        ParamValue,
        AllValues
    };

public:
    Header();
    Header(std::string name, std::string value);
    void setParam(std::string name, std::string value);
    void setQuotingMode(QuotingMode mode);
    void fromString(const std::string&);
    std::string toString() const;

    const std::string& name() const;
    const std::string& value() const;
    std::vector<std::string> paramList() const;
    const std::string& param(const std::string& paramName) const;
    bool hasParam(const std::string& paramName) const;

private:
    std::string valueStr(const std::string& value, bool hasParams) const;
    std::string paramValueStr(const std::string& value) const;

private:
    struct Param{
      std::string name;
      std::string value;
    };

    std::string name_;
    std::string value_;
    std::vector<Param> params_;
    QuotingMode quotingMode_ = QuotingMode::None;
};

using Headers = std::vector<Header>;

}
