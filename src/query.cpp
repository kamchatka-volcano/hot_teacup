#include <hot_teacup/query.h>
#include <hot_teacup/query_view.h>
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
    return std::string{name()} + "=" + std::string{value()};
}

bool operator==(const Query& lhs, const Query& rhs)
{
    return lhs.name() == rhs.name() && lhs.value() == rhs.value();
}

std::string queriesToString(const std::vector<Query>& queries)
{
    auto result = std::string{};
    for (const auto& query : queries) {
        result += query.toString() + "&";
    }
    if (!result.empty())
        result.resize(result.size() - 1); //remove last '&'
    return result;
}

std::string pathWithQueries(const std::string& path, const std::vector<Query>& queries)
{
    if (queries.empty())
        return path;
    return path + "?" + queriesToString(queries);
}

std::vector<Query> makeQueries(const std::vector<QueryView>& queryViewList)
{
    auto result = std::vector<Query>{};
    std::transform(
            queryViewList.begin(),
            queryViewList.end(),
            std::back_inserter(result),
            [](const auto& queryView)
            {
                return Query{queryView};
            });
    return result;
}


} //namespace http
