#ifndef HOT_TEACUP_TRAIT_UTILS_H
#define HOT_TEACUP_TRAIT_UTILS_H
#include <type_traits>

namespace http::detail {
template<typename T, typename... TArgs>
struct is_element_of;

template<typename T>
struct is_element_of<T> : std::false_type {};

template<typename T, typename TFirst, typename... TRest>
struct is_element_of<T, TFirst, TRest...>
    : std::conditional_t<std::is_same_v<T, TFirst>, std::true_type, is_element_of<T, TRest...>> {};

template<typename T, typename... TArgs>
constexpr auto is_element_of_v = is_element_of<T, TArgs...>::value;

template<typename...>
struct has_duplicate : std::false_type {};

template<typename TFirst, typename... TRest>
struct has_duplicate<TFirst, TRest...>
    : std::conditional_t<is_element_of<TFirst, TRest...>::value, std::true_type, has_duplicate<TRest...>> {};

template<typename... TArgs>
constexpr auto has_duplicate_v = has_duplicate<TArgs...>::value;


template <typename T, typename = void>
struct decay_to_string_view {
    using type = std::decay_t<T>;
};

template <typename T>
struct decay_to_string_view<T, std::enable_if_t<std::is_convertible_v<T, std::string_view>>> {
    using type = std::string_view;
};

template <typename T>
using decay_to_string_view_t = typename decay_to_string_view<T>::type;


} //namespace http::detail

#endif //HOT_TEACUP_TRAIT_UTILS_H
