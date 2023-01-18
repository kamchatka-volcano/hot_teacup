#include <hot_teacup/query_view.h>
#include <sfun/string_utils.h>
#include <algorithm>

namespace http{

QueryView::QueryView(std::string_view name, std::string_view value)
    : name_{name}
    , value_{value}
{
}

std::string_view QueryView::name() const
{
    return name_;
}

std::string_view QueryView::value() const
{
    return value_;
}

bool operator==(const QueryView& lhs, const QueryView& rhs)
{
    return lhs.name_ == rhs.name_ &&
           lhs.value_ == rhs.value_;
}

std::vector<QueryView> queriesFromString(std::string_view input)
{
    auto result = std::vector<QueryView>{};
    auto queries = sfun::split(input, "&");
    for(const auto& query : queries){
        auto name = sfun::before(query, "=");
        auto value = sfun::after(query, "=");
        if (name.empty())
            continue;
        result.emplace_back(sfun::trim(name), sfun::trim(value));
    }
    return result;
}

}
