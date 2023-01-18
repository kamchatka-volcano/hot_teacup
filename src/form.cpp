#include <hot_teacup/form.h>
#include <hot_teacup/form_view.h>
#include <hot_teacup/header.h>
#include <algorithm>
#include <iterator>
#include <optional>

namespace http {

FormField::FormField(const FormFieldView& fieldView)
{
    if (fieldView.hasFile())
        value_ = FormFile{
                std::string{fieldView.value()},
                std::string{fieldView.fileName()},
                std::string{fieldView.fileType()}};
    else
        value_ = std::string{fieldView.value()};
}

FormField::FormField(std::string value)
    : value_{std::move(value)}
{
}

FormField::FormField(std::string fileData, std::string fileName, std::optional<std::string> fileType)
    : value_{FormFile{std::move(fileData), std::move(fileName), std::move(fileType)}}
{
}

FormFieldType FormField::type() const
{
    return std::holds_alternative<std::string>(value_) ? FormFieldType::Param : FormFieldType::File;
}

bool FormField::hasFile() const
{
    if (type() == FormFieldType::File && !std::get<FormFile>(value_).fileName.empty())
        return true;
    else
        return false;
}

const std::string& FormField::fileName() const
{
    if (type() == FormFieldType::File)
        return std::get<FormFile>(value_).fileName;
    else
        return valueNotFound;
}

const std::string& FormField::fileType() const
{
    if (type() == FormFieldType::File) {
        const auto& res = std::get<FormFile>(value_).mimeType;
        return res ? *res : valueNotFound;
    }
    else
        return valueNotFound;
}

const std::string& FormField::value() const
{
    if (type() == FormFieldType::Param)
        return std::get<std::string>(value_);
    else
        return std::get<FormFile>(value_).fileData;
}

std::string urlEncodedFormToString(const Form& form)
{
    auto result = std::string{};
    for (const auto& [name, field] : form) {
        if (!result.empty())
            result += "&";
        result += name + "=" + (field.type() == http::FormFieldType::Param ? field.value() : field.fileName());
    }
    return result;
}

std::string multipartFormToString(const Form& form, const std::string& formBoundary)
{
    auto result = std::string{};
    for (const auto& [name, field] : form) {
        result += "--" + formBoundary + "\r\n";
        auto header = http::Header{"Content-Disposition", "form-data"};
        header.setQuotingMode(http::HeaderQuotingMode::ParamValue);
        header.setParam("name", name);
        if (field.type() == FormFieldType::Param)
            result += header.toString() + "\r\n\r\n" + field.value() + "\r\n";
        else {
            header.setParam("filename", field.fileName());
            auto fileHeader = std::optional<http::Header>{};
            if (!field.fileType().empty())
                fileHeader = http::Header{"Content-Type", field.fileType()};

            result += header.toString() + "\r\n";
            if (fileHeader)
                result += fileHeader->toString() + "\r\n";
            result += "\r\n";
            result += field.value() + "\r\n";
        }
    }
    if (!form.empty())
        result += "--" + formBoundary + "--\r\n";
    return result;
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