#include <hot_teacup/query.h>

#include "utils.h"
#include <sfun/string_utils.h>

namespace http {

std::string queryToString(const Query& query)
{
    if (!query.hasValue())
        return std::string{query.name()};

    return sfun::join_strings(query.name(), "=", query.value());
}

std::string queriesToString(const std::vector<Query>& queries)
{
    auto result = std::string{};
    const auto queryStringList = utils::transform(queries, queryToString);
    return sfun::join(queryStringList, "&");
}

std::string pathWithQueries(const std::string& path, const std::vector<Query>& queries)
{
    if (queries.empty())
        return path;
    return sfun::join_strings(path, "?", queriesToString(queries));
}

} //namespace http
