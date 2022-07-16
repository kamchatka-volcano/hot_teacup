#include <hot_teacup/query_view.h>
#include <sfun/string_utils.h>
#include <algorithm>

namespace http{
namespace str = sfun::string_utils;

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
    auto queries = str::split(input, "&");
    for(const auto& query : queries){
        auto name = str::before(query, "=");
        auto value = str::after(query, "=");
        if (name.empty())
            continue;
        result.emplace_back(str::trim(name), str::trim(value));
    }
    return result;
}

}
