#ifndef HOT_TEACUP_UTILS_H
#define HOT_TEACUP_UTILS_H
#include <sfun/functional.h>
#include <algorithm>
#include <iterator>

namespace http::utils {

template<typename TRange, typename TFunc, typename TResult = sfun::callable_return_type<TFunc>>
std::vector<TResult> transform(const TRange& container, const TFunc& transformFunc)
{
    auto result = std::vector<TResult>{};
    result.reserve(std::size(container));
    std::transform(std::cbegin(container), std::cend(container), std::back_inserter(result), transformFunc);
    return result;
}

} //namespace http::utils

#endif //HOT_TEACUP_UTILS_H
