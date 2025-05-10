#include <hot_teacup/url_encoded_form.h>

#include "url_encoder.h"
#include "utils.h"
#include <sfun/string_utils.h>
#include <algorithm>

namespace http {

UrlEncodedForm::UrlEncodedForm(const UrlEncodedFormView& formView)
    : UrlEncodedFormImpl{utils::transform(
              formView.fields(),
              [](const UrlEncodedFormFieldView& field)
              {
                  return UrlEncodedFormField{std::string{field.name}, std::string{field.value}};
              })}
{
}

namespace {

std::vector<UrlEncodedFormFieldView> makeFieldList(
        const std::unordered_map<std::string, std::vector<std::string>>& fieldsMap)
{
    auto result = std::vector<UrlEncodedFormFieldView>{};
    for (const auto& [name, values] : fieldsMap) {
        for (const auto& value : values)
            result.emplace_back(UrlEncodedFormFieldView{name, value});
    }
    return result;
}

} //namespace

UrlEncodedFormView UrlEncodedForm::toView() const
{
    return UrlEncodedFormView{makeFieldList(getFields())};
}

std::string UrlEncodedForm::toString() const
{
    const auto formFieldPairToString = [](const UrlEncodedFormFieldView& formField)
    {
        const auto& [name, value] = formField;
        return sfun::join_strings(name, "=", value);
    };
    const auto formFields = [this]
    {
        auto result = makeFieldList(getFields());
        std::stable_sort(
                result.begin(),
                result.end(),
                [](const auto& lhs, const auto& rhs)
                {
                    return lhs.name < rhs.name;
                });
        return result;
    }();
    const auto formFieldStringList = utils::transform(formFields, formFieldPairToString);
    return sfun::join(formFieldStringList, "&");
}

namespace {
std::tuple<std::string, std::string> parseUrlEncodedParamString(std::string_view paramStr)
{
    const auto namePart = sfun::before(paramStr, "=");
    if (!namePart.has_value())
        return {};

    auto name = url_encoder::decode(sfun::trim(namePart.value()));
    if (name.empty())
        return {};

    auto val = url_encoder::decode(sfun::after(paramStr, "=").value());
    return {std::move(name), std::move(val)};
}

std::vector<UrlEncodedFormField> parseUrlEncodedFields(std::string_view input)
{
    auto pos = std::size_t{0u};
    auto result = std::vector<UrlEncodedFormField>{};
    do {
        auto param = utils::getStringLine(input, pos, "&");
        auto [paramName, paramValue] = parseUrlEncodedParamString(param);
        if (paramName.empty())
            continue;
        result.emplace_back(UrlEncodedFormField{std::move(paramName), std::move(paramValue)});
    }
    while (pos < input.size());
    return result;
}

} //namespace

std::optional<UrlEncodedForm> urlEncodedFormFromString(std::string_view requestBody)
{
    const auto fields = parseUrlEncodedFields(requestBody);
    if (fields.empty())
        return std::nullopt;

    return UrlEncodedForm{fields};
}

} //namespace http