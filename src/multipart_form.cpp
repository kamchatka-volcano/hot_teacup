#include <hot_teacup/multipart_form.h>

#include "constants.h"
#include "utils.h"
#include <hot_teacup/header.h>
#include <hot_teacup/multipart_form_view.h>
#include <sfun/string_utils.h>
#include <algorithm>
#include <iterator>
#include <optional>

namespace http {

std::string_view MultipartFormField::name() const
{
    return name_;
}

std::string_view MultipartFormField::fileName() const
{
    if (type() != MultipartFormFieldType::File)
        return {};

    return std::get<FormFile>(value_).fileName;
}

std::string_view MultipartFormField::fileType() const
{
    if (type() != MultipartFormFieldType::File)
        return {};

    return std::get<FormFile>(value_).mimeType;
}

std::string_view MultipartFormField::value() const
{
    if (type() == MultipartFormFieldType::Param)
        return std::get<std::string>(value_);
    else
        return std::get<FormFile>(value_).fileData;
}

MultipartFormFieldType MultipartFormField::type() const
{
    return std::holds_alternative<std::string>(value_) ? MultipartFormFieldType::Param : MultipartFormFieldType::File;
}

MultipartFormField::MultipartFormField(const MultipartFormFieldView& fieldView)
    : name_{std::string{fieldView.name()}}
    , value_{[&fieldView]() -> std::variant<std::string, FormFile>
             {
                 if (fieldView.type() == MultipartFormFieldType::File) {
                     return FormFile{
                             std::string{fieldView.value()},
                             std::string{fieldView.fileName()},
                             std::string{fieldView.fileType()}};
                 }
                 return std::string{fieldView.value()};
             }()}
{
}

MultipartFormField::MultipartFormField(std::string name, std::string value)
    : name_{std::move(name)}
    , value_{std::move(value)}
{
}

MultipartFormField::MultipartFormField(
        std::string name,
        std::string fileData,
        std::string fileName,
        std::string fileType)
    : name_{std::move(name)}
    , value_{FormFile{std::move(fileData), std::move(fileName), std::move(fileType)}}
{
}

MultipartForm::MultipartForm(const MultipartFormView& formView)
    : MultipartForm({utils::transform(
              formView.fields(),
              [](const MultipartFormFieldView& fieldView)
              {
                  return MultipartFormField{fieldView};
              })})
{
}

namespace {

std::vector<const MultipartFormField*> makeFormFieldsList(
        const std::unordered_map<std::string, std::vector<MultipartFormField>>& fieldsMap)
{
    auto result = std::vector<const MultipartFormField*>{};
    for (const auto& [name, fields] : fieldsMap) {
        for (const auto& field : fields) {
            result.emplace_back(&field);
        }
    }
    std::stable_sort(
            result.begin(),
            result.end(),
            [](const auto lhs, const auto rhs)
            {
                return lhs->name() < rhs->name();
            });
    return result;
}

}

std::string MultipartForm::toString() const
{

    const auto formFieldToString = [](const MultipartFormField* field)
    {
        auto header = Header{"Content-Disposition", "form-data"};
        header.setQuotingMode(HeaderQuotingMode::ParamValue);
        header.setParam("name", std::string{field->name()});
        if (field->type() == MultipartFormFieldType::Param)
            return sfun::join_strings(header.toString(), "\r\n\r\n", field->value(), "\r\n");

        header.setParam("filename", std::string{field->fileName()});
        auto fileHeader = std::optional<Header>{};
        if (!field->fileType().empty())
            fileHeader = Header{"Content-Type", std::string{field->fileType()}};
        const auto fileHeaderString = fileHeader ? sfun::join_strings(fileHeader->toString(), "\r\n")
                                                 : std::string{};
        return sfun::join_strings(header.toString(), "\r\n", fileHeaderString, "\r\n", field->value(), "\r\n");

    };

    const auto formFields = makeFormFieldsList(getFields());
    const auto formFieldStringList = utils::transform(formFields, formFieldToString);
    const auto formSeparator = sfun::join_strings("--", detail::formBoundary, "\r\n");
    const auto formFieldListString = sfun::join(formFieldStringList, formSeparator);
    const auto openingBoundary = formFields.empty() ? std::string{} : formSeparator;
    const auto closingBoundary = formFields.empty() ? std::string{}
                                                    : sfun::join_strings("--", detail::formBoundary, "--\r\n");
    return sfun::join_strings(openingBoundary, formFieldListString, closingBoundary);
}

MultipartFormView MultipartForm::toView() const
{
    auto fieldsView = std::vector<MultipartFormFieldView>{};
    const auto formFields = makeFormFieldsList(getFields());
    for (const auto& field : formFields) {
        if (field->type() == MultipartFormFieldType::File)
            fieldsView.emplace_back(field->name(), field->value(), field->fileName(), field->fileType());
        else
            fieldsView.emplace_back(field->name(), field->value());
    }
    return MultipartFormView{fieldsView};
}

bool operator==(const MultipartFormField& lhs, const MultipartFormField& rhs)
{
    return lhs.type() == rhs.type() && lhs.value() == rhs.value() && lhs.fileName() == rhs.fileName() &&
            lhs.fileType() == rhs.fileType();
}

} //namespace http