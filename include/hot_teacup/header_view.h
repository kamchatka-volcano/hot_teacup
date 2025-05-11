#ifndef HOT_TEACUP_HEADER_VIEW_H
#define HOT_TEACUP_HEADER_VIEW_H

#include "detail/param_view.h"
#include <optional>
#include <string_view>
#include <vector>

namespace http {
namespace detail {
struct HeaderParamTag;
}
using HeaderParamView = detail::ParamView<detail::HeaderParamTag>;

class HeaderView {
public:
    HeaderView(std::string_view name, std::string_view value, std::vector<HeaderParamView> params = {});
    std::string_view name() const;
    std::string_view value() const;
    std::string_view param(std::string_view name) const;
    const std::vector<HeaderParamView>& params() const;
    bool hasParam(std::string_view name) const;

    friend bool operator==(const HeaderView& lhs, const HeaderView& rhs);
private:
    std::string_view name_;
    std::string_view value_;
    std::vector<HeaderParamView> params_;
};

std::optional<HeaderView> headerFromString(std::string_view);
std::optional<HeaderView> headerFromValueString(std::string_view name, std::string_view value);

} //namespace http

#endif //HOT_TEACUP_HEADER_VIEW_H