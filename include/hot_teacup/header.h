#ifndef HOT_TEACUP_HEADER_H
#define HOT_TEACUP_HEADER_H

#include "header_view.h"
#include "types.h"
#include "detail/copy_on_write_interface.h"
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace http {
class HeaderParamView;
class HeaderView;

class HeaderParam : public detail::ICopyOnWrite {
    struct Data {
        std::string_view name() const;
        std::string_view value() const;
        bool hasValue() const;
        std::string name_;
        std::optional<std::string> value_;
    };

public:
    explicit HeaderParam(const HeaderParamView&);
    explicit HeaderParam(std::string name, std::optional<std::string> value = {});
    std::string_view name() const;
    std::string_view value() const;
    bool hasValue() const;
    std::string toString(HeaderQuotingMode quotingMode) const;

    friend bool operator==(const HeaderParam& lhs, const HeaderParam& rhs);

private:
    bool isView() const override;
    void makeOwnStateFromView() override;

private:
    std::variant<Data, HeaderParamView> data_;
};

class Header : public detail::ICopyOnWrite {
    struct Data{
        std::string_view name() const;
        std::string_view value() const;
        std::string name_;
        std::string value_;
    };
public:
    explicit Header(const HeaderView&);
    Header(std::string name, std::string value, std::vector<HeaderParam> params = {});
    void setParam(std::string name, std::optional<std::string> value = {});
    void setParams(const std::vector<HeaderParam>& params);
    void setQuotingMode(HeaderQuotingMode mode);

    std::string_view name() const;
    std::string_view value() const;
    std::string_view param(std::string_view name) const;
    bool hasParam(std::string_view name) const;
    const std::vector<HeaderParam>& params() const;

    std::string toString() const;
    HeaderView toView() const;

    friend bool operator==(const Header& lhs, const Header& rhs);

private:
    bool isView() const override;
    void makeOwnStateFromView() override;

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