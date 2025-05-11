#ifndef HOT_TEACUP_QUERY_VIEW_H
#define HOT_TEACUP_QUERY_VIEW_H

#include "detail/param_view.h"
#include <string>
#include <vector>

namespace http {
namespace detail {
struct QueryTag;
}
using QueryView = detail::ParamView<detail::QueryTag>;

std::vector<QueryView> queriesFromString(std::string_view input);
} //namespace http

#endif //HOT_TEACUP_QUERY_VIEW_H
