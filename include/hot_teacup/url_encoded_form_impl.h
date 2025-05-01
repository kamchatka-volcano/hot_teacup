#ifndef HOT_TEACUP_URL_ENCODED_FORM_IMPL_H
#define HOT_TEACUP_URL_ENCODED_FORM_IMPL_H

#include <set>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace http::detail {
template<typename TStr, typename TField>
class UrlEncodedFormImpl {
protected:
    ~UrlEncodedFormImpl() = default;

public:
    UrlEncodedFormImpl(const std::vector<TField>& fields)
    {
        for (const auto& field : fields) {
            const auto it = fields_.find(TStr{field.name});
            if (it != fields_.end())
                it->second.push_back(field.value);
            else
                fields_.emplace(field.name, std::vector{field.value});
        }
    }

    std::vector<std::string_view> paramList() const
    {
        auto result = std::set<std::string_view>{};
        for (const auto& [formFieldName, _] : fields_)
            result.insert(formFieldName);
        return {result.begin(), result.end()};
    }

    std::string_view param(std::string_view name, int index = 0) const
    {
        const auto it = fields_.find(TStr{name});
        if (it == fields_.end())
            return {};

        if (index < 0 || index >= static_cast<int>(it->second.size()))
            return {};
        return it->second.at(index);
    }

    int paramsCount() const
    {
        auto result = 0;
        for (const auto& [formFieldName, formFieldValues] : fields_)
            result += static_cast<int>(formFieldValues.size());
        return result;
    }

    int paramCount(std::string_view name) const
    {
        const auto it = fields_.find(TStr{name});
        if (it == fields_.end())
            return 0;

        return static_cast<int>(it->second.size());
    }

    bool hasParam(std::string_view name) const
    {
        return paramCount(name) != 0;
    }

protected:
    const std::unordered_map<TStr, std::vector<TStr>>& getFields() const
    {
        return fields_;
    }

private:
    std::unordered_map<TStr, std::vector<TStr>> fields_;
};

} //namespace http::detail

#endif //HOT_TEACUP_URL_ENCODED_FORM_IMPL_H
