#ifndef HOT_TEACUP_HEADER_H
#define HOT_TEACUP_HEADER_H

#include "header_view.h"
#include "types.h"
#include "detail/param.h"
#include "detail/view_or_owner_interface.h"
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace http {
namespace detail {
struct HeaderParamTag;
}
using HeaderParam = detail::Param<detail::HeaderParamTag>;
std::string headerParamToString(const HeaderParam&, HeaderQuotingMode quotingMode);

class Header : public detail::IViewOrOwner {
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

} //namespace http

#endif //HOT_TEACUP_HEADER_H