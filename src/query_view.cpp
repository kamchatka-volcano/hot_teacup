#include <hot_teacup/query_view.h>

#include <sfun/string_utils.h>

namespace http {

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
                result.emplace_back(name);
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
