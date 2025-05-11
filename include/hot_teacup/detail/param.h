#ifndef HOT_TEACUP_PARAM_H
#define HOT_TEACUP_PARAM_H

#include "param_view.h"
#include "view_or_owner_interface.h"
#include <string>
#include <string_view>
#include <variant>

namespace http::detail {

template<typename TTag>
class Param : public IViewOrOwner {
    struct Data {
        std::string_view name() const
        {
            return name_;
        }

        std::string_view value() const
        {
            if (!value_.has_value())
                return std::string_view{};

            return value_.value();
        }

        bool hasValue() const
        {
            return value_.has_value();
        }

        std::string name_;
        std::optional<std::string> value_;
    };

public:
    explicit Param(const ParamView<TTag>& paramView)
        : data_{paramView}
    {
    }

    Param(std::string name)
        : data_{Data{std::move(name), std::nullopt}}
    {
    }

    Param(std::string name, std::string value)
        : data_{Data{std::move(name), std::move(value)}}
    {
    }

    std::string_view name() const
    {
        return std::visit(
                [](const auto& data)
                {
                    return data.name();
                },
                data_);
    }

    std::string_view value() const
    {
        return std::visit(
                [](const auto& data)
                {
                    return data.value();
                },
                data_);
    }

    bool hasValue() const
    {
        return std::visit(
                [](const auto& data)
                {
                    return data.hasValue();
                },
                data_);
    }

    friend bool operator==(const Param& lhs, const Param& rhs)
    {
        return lhs.name() == rhs.name() && lhs.value() == rhs.value();
    }

private:
    bool isView() const override
    {
        return std::holds_alternative<ParamView<TTag>>(data_);
    }

    void makeOwnStateFromView() override
    {
        if (!isView())
            return;

        const auto& paramView = std::get<ParamView<TTag>>(data_);
        if (!paramView.hasValue())
            data_ = Data{std::string{paramView.name()}, std::nullopt};
        else
            data_ = Data{std::string{paramView.name()}, std::string{paramView.value()}};
    }

private:
    std::variant<Data, ParamView<TTag>> data_;
};

} //namespace http::detail

#endif //HOT_TEACUP_PARAM_H
