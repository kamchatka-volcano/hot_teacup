#ifndef HOT_TEACUP_MULTIPART_FORM_IMPL_H
#define HOT_TEACUP_MULTIPART_FORM_IMPL_H

#include "types.h"
#include <set>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace http::detail {

template<typename TName, typename TField>
class MultipartFormImpl {
protected:
    ~MultipartFormImpl() = default;

public:
    MultipartFormImpl(const std::vector<TField>& fields)
    {
        for (const auto& field : fields) {
            const auto it = fields_.find(TName{field.name()});
            if (it != fields_.end())
                it->second.push_back(field);
            else
                fields_.emplace(field.name(), std::vector{field});
        }
    }

    std::vector<std::string_view> paramList() const
    {
        auto result = std::set<std::string_view>{};
        for (const auto& [formFieldName, formFields] : fields_) {
            for (const auto& formField : formFields)
                if (formField.type() == MultipartFormFieldType::Param)
                    result.insert(formFieldName);
        }
        return {result.begin(), result.end()};
    }

    std::vector<std::string_view> fileList() const
    {
        auto result = std::set<std::string_view>{};
        for (const auto& [formFieldName, formFields] : fields_) {
            for (const auto& formField : formFields)
                if (formField.type() == MultipartFormFieldType::File)
                    result.insert(formFieldName);
        }
        return {result.begin(), result.end()};
    }

    std::string_view param(std::string_view name, int index = 0) const
    {
        auto i = 0;
        const auto itFormFields = fields_.find(TName{name});
        if (itFormFields == fields_.end())
            return {};

        for (const auto& field : itFormFields->second) {
            if (field.type() == MultipartFormFieldType::Param) {
                if (i++ == index)
                    return field.value();
            }
        }
        return {};
    }

    int paramsCount() const
    {
        auto result = 0;
        for (const auto& [formFieldName, formFields] : fields_)
            for (const auto& formField : formFields)
                if (formField.type() == MultipartFormFieldType::Param)
                    result++;
        return result;
    }

    int paramCount(std::string_view name) const
    {
        auto result = 0;
        const auto itFormFields = fields_.find(TName{name});
        if (itFormFields == fields_.end())
            return 0;

        for (const auto& field : itFormFields->second) {
            if (field.type() == MultipartFormFieldType::Param)
                result++;
        }
        return result;
    }

    bool hasParam(std::string_view name) const
    {
        const auto itFormFields = fields_.find(TName{name});
        if (itFormFields == fields_.end())
            return false;

        for (const auto& field : itFormFields->second) {
            if (field.type() == MultipartFormFieldType::Param)
                return true;
        }
        return false;
    }

    bool hasParams() const
    {
        for (const auto& [formFieldName, formFields] : fields_)
            for (const auto& formField : formFields)
                if (formField.type() == MultipartFormFieldType::Param)
                    return true;
        return false;
    }

    std::string_view fileData(std::string_view name, int index = 0) const
    {
        auto i = 0;
        const auto itFormFields = fields_.find(TName{name});
        if (itFormFields == fields_.end())
            return {};

        for (const auto& field : itFormFields->second) {
            if (field.type() == MultipartFormFieldType::File) {
                if (i++ == index)
                    return field.value();
            }
        }
        return {};
    }

    std::string_view fileName(std::string_view name, int index = 0) const
    {
        auto i = 0;
        const auto itFormFields = fields_.find(TName{name});
        if (itFormFields == fields_.end())
            return {};

        for (const auto& field : itFormFields->second) {
            if (field.type() == MultipartFormFieldType::File) {
                if (i++ == index)
                    return field.fileName();
            }
        }
        return {};
    }

    std::string_view fileType(std::string_view name, int index = 0) const
    {
        auto i = 0;
        const auto itFormFields = fields_.find(TName{name});
        if (itFormFields == fields_.end())
            return {};

        for (const auto& field : itFormFields->second) {
            if (field.type() == MultipartFormFieldType::File) {
                if (i++ == index)
                    return field.fileType();
            }
        }
        return {};
    }

    int filesCount() const
    {
        auto result = 0;
        for (const auto& [formFieldName, formFields] : fields_)
            for (const auto& formField : formFields)
                if (formField.type() == MultipartFormFieldType::File)
                    result++;
        return result;
    }

    int fileCount(std::string_view name) const
    {
        auto result = 0;
        const auto itFormFields = fields_.find(TName{name});
        if (itFormFields == fields_.end())
            return 0;

        for (const auto& field : itFormFields->second) {
            if (field.type() == MultipartFormFieldType::File)
                result++;
        }
        return result;
    }

    bool hasFile(std::string_view name) const
    {
        const auto itFormFields = fields_.find(TName{name});
        if (itFormFields == fields_.end())
            return false;

        for (const auto& field : itFormFields->second) {
            if (field.type() == MultipartFormFieldType::File)
                return true;
        }
        return false;
    }

    bool hasFiles() const
    {
        for (const auto& [formFieldName, formFields] : fields_)
            for (const auto& formField : formFields)
                if (formField.type() == MultipartFormFieldType::File)
                    return true;
        return false;
    }

protected:
    const std::unordered_map<TName, std::vector<TField>>& getFields() const
    {
        return fields_;
    }

private:
    std::unordered_map<TName, std::vector<TField>> fields_;
};

} //namespace http::detail

#endif //HOT_TEACUP_MULTIPART_FORM_IMPL_H
