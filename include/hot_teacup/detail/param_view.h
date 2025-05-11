#ifndef HOT_TEACUP_PARAM_VIEW_H
#define HOT_TEACUP_PARAM_VIEW_H

#include <optional>
#include <string_view>

namespace http::detail {

template<typename TTag>
class ParamView {

public:
    ParamView(std::string_view name)
        : name_{name}
    {
    }

    ParamView(std::string_view name, std::string_view value)
        : name_{name}
        , value_{value}
    {
    }

    std::string_view name() const
    {
        return name_;
    }

    std::string_view value() const
    {
        return value_.value_or(std::string_view{});
    }

    bool hasValue() const
    {
        return value_.has_value();
    }

    friend bool operator==(const ParamView& lhs, const ParamView& rhs)
    {
        return lhs.name_ == rhs.name_ && lhs.value_ == rhs.value_;
    }

private:
    std::string_view name_;
    std::optional<std::string_view> value_;
};

} //namespace http::detail

#endif //HOT_TEACUP_PARAM_VIEW_H
