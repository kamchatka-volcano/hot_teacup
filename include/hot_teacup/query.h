#ifndef HOT_TEACUP_QUERY_H
#define HOT_TEACUP_QUERY_H

#include "detail/param.h"
#include <string>
#include <vector>

namespace http {
namespace detail {
struct QueryTag;
}
using Query = detail::Param<detail::QueryTag>;

std::string pathWithQueries(const std::string& path, const std::vector<Query>& queries);
std::string queryToString(const Query&);
std::string queriesToString(const std::vector<Query>&);
} //namespace http

#endif //HOT_TEACUP_QUERY_H
