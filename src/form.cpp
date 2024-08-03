#include "utils.h"
#include <hot_teacup/form.h>
#include <hot_teacup/form_view.h>
#include <hot_teacup/header.h>
#include <sfun/string_utils.h>
#include <algorithm>
#include <iterator>
#include <optional>

namespace http {

std::string_view FormField::Data::fileName() const
{
    if (type() == FormFieldType::File)
        return std::get<FormFile>(value_).fileName;
    else
        return valueNotFound;
}

std::string_view FormField::Data::fileType() const
{
    if (type() == FormFieldType::File) {
        const auto& res = std::get<FormFile>(value_).mimeType;
        return res ? *res : valueNotFound;
    }
    else
        return valueNotFound;
}

std::string_view FormField::Data::value() const
{
    if (type() == FormFieldType::Param)
        return std::get<std::string>(value_);
    else
        return std::get<FormFile>(value_).fileData;
}

FormFieldType FormField::Data::type() const
{
    return std::holds_alternative<std::string>(value_) ? FormFieldType::Param : FormFieldType::File;
}

bool FormField::Data::hasFile() const
{
    if (type() == FormFieldType::File && !std::get<FormFile>(value_).fileName.empty())
        return true;
    else
        return false;
}

FormField::FormField(const FormFieldView& fieldView)
    : data_{fieldView}
{
}

FormField::FormField(std::string value)
    : data_{Data{std::move(value)}}
{
}

FormField::FormField(std::string fileData, std::string fileName, std::optional<std::string> fileType)
    : data_{Data{FormFile{std::move(fileData), std::move(fileName), std::move(fileType)}}}
{
}

FormFieldType FormField::type() const
{
    return std::visit([](const auto& data){ return data.type();}, data_);
}

bool FormField::hasFile() const
{
    return std::visit([](const auto& data){ return data.hasFile();}, data_);
}

std::string_view FormField::fileName() const
{
    return std::visit([](const auto& data){ return data.fileName();}, data_);
}

std::string_view FormField::fileType() const
{
    return std::visit([](const auto& data){ return data.fileType();}, data_);
}

std::string_view FormField::value() const
{
    return std::visit([](const auto& data){ return data.value();}, data_);
}

bool operator==(const FormField& lhs, const FormField& rhs)
{
    return lhs.type() == rhs.type() && lhs.value() == rhs.value() && lhs.hasFile() == rhs.hasFile() &&
            lhs.fileName() == rhs.fileName() && lhs.fileType() == rhs.fileType();
}

std::string urlEncodedFormToString(const Form& form)
{
    const auto formFieldPairToString = [](const std::pair<std::string, FormField>& formFieldPair)
    {
        const auto& [name, formField] = formFieldPair;
        return sfun::join_strings(
                name,
                "=",
                (formField.type() == http::FormFieldType::Param ? formField.value() : formField.fileName()));
    };
    const auto formFieldStringList = utils::transform(form, formFieldPairToString);
    return sfun::join(formFieldStringList, "&");
}

std::string multipartFormToString(const Form& form, const std::string& formBoundary)
{
    const auto formFieldPairToString = [](const std::pair<std::string, FormField>& formFieldPair)
    {
        const auto& [name, field] = formFieldPair;
        auto header = http::Header{"Content-Disposition", "form-data"};
        header.setQuotingMode(http::HeaderQuotingMode::ParamValue);
        header.setParam("name", name);
        if (field.type() == FormFieldType::Param)
            return sfun::join_strings(header.toString(), "\r\n\r\n", field.value(), "\r\n");
        else {
            header.setParam("filename", std::string{field.fileName()});
            auto fileHeader = std::optional<http::Header>{};
            if (!field.fileType().empty())
                fileHeader = http::Header{"Content-Type", std::string{field.fileType()}};
            const auto fileHeaderString = fileHeader ? sfun::join_strings(fileHeader->toString(), "\r\n")
                                                     : std::string{};
            return sfun::join_strings(header.toString() , "\r\n", fileHeaderString, "\r\n", field.value(), "\r\n");
        }
    };
    const auto formFieldStringList = utils::transform(form, formFieldPairToString);
    const auto formSeparator = sfun::join_strings("--", formBoundary, "\r\n");
    const auto formFieldListString = sfun::join(formFieldStringList, formSeparator);
    const auto openingBoundary = form.empty() ? std::string{} : formSeparator;
    const auto closingBoundary = form.empty() ? std::string{} : sfun::join_strings("--", formBoundary, "--\r\n");
    return sfun::join_strings(openingBoundary, formFieldListString, closingBoundary);
}

Form makeForm(const std::map<std::string, FormFieldView>& formView)
{
    auto result = Form{};
    std::transform(
            formView.begin(),
            formView.end(),
            std::inserter(result, result.end()),
            [](const auto& fieldViewPair)
            {
                return std::pair{fieldViewPair.first, FormField{fieldViewPair.second}};
            });
    return result;
}

} //namespace http