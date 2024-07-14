#include "utils.h"
#include <hot_teacup/query.h>
#include <hot_teacup/query_view.h>
#include <sfun/string_utils.h>
#include <algorithm>
#include <iterator>
#include <utility>

namespace http {

std::string_view Query::Data::name() const
{
    return name_;
}
std::string_view Query::Data::value() const
{
    return value_;
}

Query::Query(const QueryView& queryView)
    : data_{queryView}
{
}

Query::Query(std::string name, std::string value)
    : data_{Data{std::move(name), std::move(value)}}
{
}

std::string_view Query::name() const
{
    return std::visit([](const auto& data){ return data.name();}, data_);
}

std::string_view Query::value() const
{
    return std::visit([](const auto& data){ return data.value();}, data_);
}

std::string Query::toString() const
{
    return sfun::join_strings(name(), "=", value());
}

bool operator==(const Query& lhs, const Query& rhs)
{
    return lhs.name() == rhs.name() && lhs.value() == rhs.value();
}

std::string queriesToString(const std::vector<Query>& queries)
{
    auto result = std::string{};
    const auto queryToString = [](const Query& query){
        return query.toString();
    };
    const auto queryStringList = utils::transform(queries, queryToString);
    return sfun::join(queryStringList, "&");
}

std::string pathWithQueries(const std::string& path, const std::vector<Query>& queries)
{
    if (queries.empty())
        return path;
    return sfun::join_strings(path, "?", queriesToString(queries));
}

std::vector<Query> makeQueries(const std::vector<QueryView>& queryViewList)
{
    return utils::transform(
            queryViewList,
            [](const QueryView& queryView)
            {
                return Query{queryView};
            });
}


} //namespace http
