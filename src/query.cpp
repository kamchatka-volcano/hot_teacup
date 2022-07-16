#include <hot_teacup/query.h>
#include <hot_teacup/query_view.h>
#include <sfun/string_utils.h>
#include <algorithm>
#include <utility>

namespace http{
namespace str = sfun::string_utils;

Query::Query(const QueryView& queryView)
    : name_{queryView.name()}
    , value_{queryView.value()}
{}

Query::Query(std::string name, std::string value)
    : name_{std::move(name)}
    , value_{std::move(value)}
{
}

const std::string& Query::name() const
{
    return name_;
}

const std::string& Query::value() const
{
    return value_;
}

std::string Query::toString() const
{
    return name_ + "=" + value_;
}


bool operator==(const Query& lhs, const Query& rhs)
{
    return lhs.name_ == rhs.name_ &&
           lhs.value_ == rhs.value_;
}

std::string queriesToString(const std::vector<Query>& queries, const std::vector<std::string>& queryBlackList)
{
    auto result = std::string{};
    for (const auto& query : queries){
        if (std::find(queryBlackList.begin(), queryBlackList.end(), query.name()) == queryBlackList.end())
            result += query.toString() + "&";
    }
    if (!result.empty())
        result.resize(result.size() - 1); //remove last '&'
    return result;
}

std::string pathWithQueries(const std::string& path,
                            const std::vector<Query>& queries,
                            const std::vector<std::string>& queryBlackList)
{
    if (queries.empty())
        return path;
    return path + "?" + queriesToString(queries, queryBlackList);
}

std::string pathWithQuery(const std::string& path, const Query& query)
{
    return pathWithQueries(path, {query});
}

std::vector<Query> makeQueries(const std::vector<QueryView>& queryViewList)
{
    auto result = std::vector<Query>{};
    std::transform(queryViewList.begin(), queryViewList.end(), std::back_inserter(result),
               [](const auto& queryView) {
                   return Query{queryView};
               });
    return result;
}

}
