#ifndef HOT_TEACUP_HEADER_H
#define HOT_TEACUP_HEADER_H

#include "header_view.h"
#include "types.h"
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <variant>

namespace http {
class HeaderParamView;
class HeaderView;

class HeaderParam {
    struct Data{
        std::string_view name() const;
        std::string_view value() const;
        bool hasValue() const;
        std::string name_;
        std::optional<std::string> value_;
    };

public:
    explicit HeaderParam(const HeaderParamView&);
    explicit HeaderParam(std::string name);
    HeaderParam(std::string name, std::string value);
    std::string_view name() const;
    std::string_view value() const;
    bool hasValue() const;
    std::string toString(HeaderQuotingMode quotingMode) const;

    friend bool operator==(const HeaderParam& lhs, const HeaderParam& rhs);

private:
    static inline const std::string valueNotFound;

private:
    std::variant<Data, HeaderParamView> data_;

};

class Header {
    struct Data{
        std::string_view name() const;
        std::string_view value() const;
        std::string name_;
        std::string value_;
    };
public:
    explicit Header(const HeaderView&);
    Header(std::string name, std::string value);
    void setParam(std::string name);
    void setParam(std::string name, std::string value);
    void setQuotingMode(HeaderQuotingMode mode);
    std::string toString() const;

    std::string_view name() const;
    std::string_view value() const;
    std::string_view param(std::string_view name) const;
    bool hasParam(std::string_view name) const;

    const std::vector<HeaderParam>& params() const;

    friend bool operator==(const Header& lhs, const Header& rhs);

private:
    bool isView() const;
    
private:
    std::variant<Data, HeaderView> data_;
    std::vector<HeaderParam> params_;
    HeaderQuotingMode quotingMode_ = HeaderQuotingMode::None;
};

using Headers = std::vector<Header>;

std::vector<HeaderParam> makeHeaderParams(const std::vector<HeaderParamView>& headerParamViewList);
std::vector<Header> makeHeaders(const std::vector<HeaderView>& headerViewList);

} //namespace http

#endif //HOT_TEACUP_HEADER_H