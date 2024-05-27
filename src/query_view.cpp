#include <hot_teacup/query_view.h>
#include <sfun/string_utils.h>
#include <algorithm>

namespace http {

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
    return lhs.name_ == rhs.name_ && lhs.value_ == rhs.value_;
}

std::vector<QueryView> queriesFromString(std::string_view input)
{
    auto result = std::vector<QueryView>{};
    const auto queries = sfun::split(input, "&");
    for (const auto& query : queries) {
        const auto namePart = sfun::before(query, "=");
        const auto valuePart = sfun::after(query, "=");
        if (!namePart.has_value()) {
            const auto name = sfun::trim(query);
            if (!name.empty())
                result.emplace_back(name, "");
        }
        else {
            const auto name = sfun::trim(namePart.value());
            if (!name.empty())
                result.emplace_back(name, sfun::trim(valuePart.value_or("")));
        }
    }
    return result;
}

} //namespace http
