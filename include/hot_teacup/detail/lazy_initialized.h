#ifndef HOT_TEACUP_LAZY_INITIALIZED_H
#define HOT_TEACUP_LAZY_INITIALIZED_H
#include <functional>
#include <optional>

namespace http::detail {

template<typename TParent, auto initializer, typename TResult>
class LazyInitialized {
public:
    const TResult& get(const TParent& parent) const
    {
        if (!value)
            value = std::invoke(initializer, parent);
        return *value;
    }

    TResult& get(TParent& parent)
    {
        if (!value)
            value = std::invoke(initializer, parent);
        return *value;
    }

private:
    mutable std::optional<TResult> value;
};

} //namespace http::detail

#endif //HOT_TEACUP_LAZY_INITIALIZED_H
