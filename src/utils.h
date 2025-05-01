#ifndef HOT_TEACUP_UTILS_H
#define HOT_TEACUP_UTILS_H

#include <sfun/functional.h>
#include <algorithm>
#include <iterator>
#include <string_view>

namespace http::utils {

template<typename TRange, typename TFunc, typename TResult = sfun::callable_return_type<TFunc>>
std::vector<TResult> transform(const std::vector<TRange>& container, const TFunc& transformFunc)
{
    auto result = std::vector<TResult>{};
    result.reserve(std::size(container));
    std::transform(std::cbegin(container), std::cend(container), std::back_inserter(result), transformFunc);
    return result;
}

template<typename TResultContainer, typename TRange, typename TFunc>
TResultContainer transformTo(const TRange& container, const TFunc& transformFunc)
{
    auto result = TResultContainer{};
    result.reserve(std::size(container));
    std::transform(std::cbegin(container), std::cend(container), std::inserter(result, result.end()), transformFunc);
    return result;
}

std::string_view getStringLine(std::string_view input, std::size_t& pos, std::string_view lineSeparator = "\r\n");

} //namespace http::utils

#endif //HOT_TEACUP_UTILS_H
