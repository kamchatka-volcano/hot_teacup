#include <hot_teacup/form.h>
#include <hot_teacup/form_view.h>
#include <hot_teacup/header.h>
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
//    if (fieldView.hasFile())
//        value_ = FormFile{
//                std::string{fieldView.value()},
//                std::string{fieldView.fileName()},
//                std::string{fieldView.fileType()}};
//    else
//        value_ = std::string{fieldView.value()};
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

std::string urlEncodedFormToString(const Form& form)
{
    auto result = std::string{};
    for (const auto& [name, field] : form) {
        if (!result.empty())
            result += "&";
        result += name + "=" +
                (field.type() == http::FormFieldType::Param ? std::string{field.value()}
                                                            : std::string{field.fileName()});
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
            result += header.toString() + "\r\n\r\n" + std::string{field.value()} + "\r\n";
        else {
            header.setParam("filename", std::string{field.fileName()});
            auto fileHeader = std::optional<http::Header>{};
            if (!field.fileType().empty())
                fileHeader = http::Header{"Content-Type", std::string{field.fileType()}};

            result += header.toString() + "\r\n";
            if (fileHeader)
                result += fileHeader->toString() + "\r\n";
            result += "\r\n";
            result += std::string{field.value()} + "\r\n";
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