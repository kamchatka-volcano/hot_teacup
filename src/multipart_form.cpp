#include "constants.h"
#include "utils.h"
#include <hot_teacup/header.h>
#include <hot_teacup/multipart_form.h>
#include <hot_teacup/multipart_form_view.h>
#include <sfun/string_utils.h>
#include <algorithm>
#include <iterator>
#include <optional>

namespace http {

MultipartFormField::Data::Data(std::string name, std::string value)
    : name_{std::move(name)}
    , value_{std::move(value)}
{
}

MultipartFormField::Data::Data(std::string name, FormFile file)
    : name_{std::move(name)}
    , value_{std::move(file)}
{
}
std::string_view MultipartFormField::Data::name() const
{
    return name_;
}

std::string_view MultipartFormField::Data::fileName() const
{
    if (type() != MultipartFormFieldType::File)
        return {};

    return std::get<FormFile>(value_).fileName;
}

std::string_view MultipartFormField::Data::fileType() const
{
    if (type() != MultipartFormFieldType::File)
        return {};

    return std::get<FormFile>(value_).mimeType;
}

std::string_view MultipartFormField::Data::value() const
{
    if (type() == MultipartFormFieldType::Param)
        return std::get<std::string>(value_);
    else
        return std::get<FormFile>(value_).fileData;
}

MultipartFormFieldType MultipartFormField::Data::type() const
{
    return std::holds_alternative<std::string>(value_) ? MultipartFormFieldType::Param : MultipartFormFieldType::File;
}

MultipartFormField::MultipartFormField(const MultipartFormFieldView& fieldView)
    : data_{fieldView}
{
}

MultipartFormField::MultipartFormField(std::string name, std::string value)
    : data_{Data{std::move(name), std::move(value)}}
{
}

MultipartFormField::MultipartFormField(
        std::string name,
        std::string fileData,
        std::string fileName,
        std::string fileType)
    : data_{Data{std::move(name), FormFile{std::move(fileData), std::move(fileName), std::move(fileType)}}}
{
}
std::string_view MultipartFormField::name() const
{
    return std::visit(
            [](const auto& data)
            {
                return data.name();
            },
            data_);
}

MultipartFormFieldType MultipartFormField::type() const
{
    return std::visit(
            [](const auto& data)
            {
                return data.type();
            },
            data_);
}

std::string_view MultipartFormField::fileName() const
{
    return std::visit(
            [](const auto& data)
            {
                return data.fileName();
            },
            data_);
}

std::string_view MultipartFormField::fileType() const
{
    return std::visit(
            [](const auto& data)
            {
                return data.fileType();
            },
            data_);
}

std::string_view MultipartFormField::value() const
{
    return std::visit(
            [](const auto& data)
            {
                return data.value();
            },
            data_);
}

bool MultipartFormField::isView() const
{
    return std::holds_alternative<MultipartFormFieldView>(data_);
}

void MultipartFormField::makeOwnStateFromView()
{
    if (!isView())
        return;

    const auto& fieldView = std::get<MultipartFormFieldView>(data_);
    if (fieldView.type() == MultipartFormFieldType::File)
        data_ =
                Data{std::string{fieldView.name()},
                     FormFile{
                             std::string{fieldView.value()},
                             std::string{fieldView.fileName()},
                             std::string{fieldView.fileType()}}};
    else
        data_ = Data{std::string{fieldView.name()}, std::string{fieldView.value()}};
}

std::vector<MultipartFormFieldView> MultipartForm::Data::fields() const
{
    auto result = std::vector<MultipartFormFieldView>{};
    for (const auto& [name, fields] : getFields())
        for (const auto& field : fields) {
            const auto fieldView = [&]
            {
                if (field.type() == MultipartFormFieldType::Param)
                    return MultipartFormFieldView{field.name(), field.value()};

                return MultipartFormFieldView{field.name(), field.value(), field.fileName(), field.fileType()};
            }();
            result.emplace_back(fieldView);
        }
    return result;
}

MultipartForm::MultipartForm(MultipartFormView formView)
    : data_{formView}
{
}

MultipartForm::MultipartForm(std::vector<MultipartFormField> fields)
    : data_{Data{std::move(fields)}}
{
}

std::vector<MultipartFormFieldView> MultipartForm::fields() const
{
    return std::visit(
            [](const auto& data) -> std::vector<MultipartFormFieldView>
            {
                return data.fields();
            },
            data_);
}

std::vector<std::string_view> MultipartForm::paramList() const
{
    return std::visit(
            [](const auto& data) -> std::vector<std::string_view>
            {
                return data.paramList();
            },
            data_);
}

std::vector<std::string_view> MultipartForm::fileList() const
{
    return std::visit(
            [](const auto& data) -> std::vector<std::string_view>
            {
                return data.fileList();
            },
            data_);
}

std::string_view MultipartForm::param(std::string_view name, int index) const
{
    return std::visit(
            [&name, index](const auto& data) -> std::string_view
            {
                return data.param(name, index);
            },
            data_);
}

int MultipartForm::paramsCount() const
{
    return std::visit(
            [](const auto& data) -> int
            {
                return data.paramsCount();
            },
            data_);
}

int MultipartForm::paramCount(std::string_view name) const
{
    return std::visit(
            [&name](const auto& data) -> int
            {
                return data.paramCount(name);
            },
            data_);
}

bool MultipartForm::hasParam(std::string_view name) const
{
    return std::visit(
            [&name](const auto& data) -> bool
            {
                return data.hasParam(name);
            },
            data_);
}

bool MultipartForm::hasParams() const
{
    return std::visit(
            [](const auto& data) -> bool
            {
                return data.hasParams();
            },
            data_);
}

std::string_view MultipartForm::fileData(std::string_view name, int index) const
{
    return std::visit(
            [&name, index](const auto& data) -> std::string_view
            {
                return data.fileData(name, index);
            },
            data_);
}

std::string_view MultipartForm::fileName(std::string_view name, int index) const
{
    return std::visit(
            [&name, index](const auto& data) -> std::string_view
            {
                return data.fileName(name, index);
            },
            data_);
}

std::string_view MultipartForm::fileType(std::string_view name, int index) const
{
    return std::visit(
            [&name, index](const auto& data) -> std::string_view
            {
                return data.fileType(name, index);
            },
            data_);
}

int MultipartForm::filesCount() const
{
    return std::visit(
            [](const auto& data) -> int
            {
                return data.filesCount();
            },
            data_);
}

int MultipartForm::fileCount(std::string_view name) const
{
    return std::visit(
            [&name](const auto& data) -> int
            {
                return data.fileCount(name);
            },
            data_);
}

bool MultipartForm::hasFiles() const
{
    return std::visit(
            [](const auto& data) -> bool
            {
                return data.hasFiles();
            },
            data_);
}

std::string MultipartForm::toString() const
{

    const auto formFieldPairToString = [](const MultipartFormFieldView& field)
    {
        auto header = Header{"Content-Disposition", "form-data"};
        header.setQuotingMode(HeaderQuotingMode::ParamValue);
        header.setParam("name", std::string{field.name()});
        if (field.type() == MultipartFormFieldType::Param)
            return sfun::join_strings(header.toString(), "\r\n\r\n", field.value(), "\r\n");
        else {
            header.setParam("filename", std::string{field.fileName()});
            auto fileHeader = std::optional<Header>{};
            if (!field.fileType().empty())
                fileHeader = Header{"Content-Type", std::string{field.fileType()}};
            const auto fileHeaderString = fileHeader ? sfun::join_strings(fileHeader->toString(), "\r\n")
                                                     : std::string{};
            return sfun::join_strings(header.toString(), "\r\n", fileHeaderString, "\r\n", field.value(), "\r\n");
        }
    };
    const auto formFields = [this]
    {
        auto result = fields();
        std::sort(
                result.begin(),
                result.end(),
                [](const auto& lhs, const auto& rhs)
                {
                    return lhs.name() < rhs.name();
                });
        return result;
    }();
    const auto formFieldStringList = utils::transform(formFields, formFieldPairToString);
    const auto formSeparator = sfun::join_strings("--", detail::formBoundary, "\r\n");
    const auto formFieldListString = sfun::join(formFieldStringList, formSeparator);
    const auto openingBoundary = formFields.empty() ? std::string{} : formSeparator;
    const auto closingBoundary = formFields.empty() ? std::string{}
                                                    : sfun::join_strings("--", detail::formBoundary, "--\r\n");
    return sfun::join_strings(openingBoundary, formFieldListString, closingBoundary);
}

MultipartFormView MultipartForm::toView() const
{
    if (std::holds_alternative<Data>(data_)) {
        auto fieldsView = std::vector<MultipartFormFieldView>{};
        for (const auto& field : std::get<Data>(data_).fields()) {
            if (field.type() == MultipartFormFieldType::File)
                fieldsView.emplace_back(field.name(), field.value(), field.fileName(), field.fileType());
            else
                fieldsView.emplace_back(field.name(), field.value());
        }
        return MultipartFormView{fieldsView};
    }

    return std::get<MultipartFormView>(data_);
}

bool MultipartForm::isView() const
{
    return std::holds_alternative<MultipartFormView>(data_);
}

void MultipartForm::makeOwnStateFromView()
{
    if (!isView())
        return;

    const auto& formView = std::get<MultipartFormView>(data_);
    const auto toOwningState = [](const MultipartFormFieldView& fieldView)
    {
        auto field = MultipartFormField{fieldView};
        static_cast<detail::ICopyOnWrite&>(field).makeOwnStateFromView();
        return field;
    };
    data_ = Data{utils::transform(formView.fields(), toOwningState)};
}

bool operator==(const MultipartFormField& lhs, const MultipartFormField& rhs)
{
    return lhs.type() == rhs.type() && lhs.value() == rhs.value() && lhs.fileName() == rhs.fileName() &&
            lhs.fileType() == rhs.fileType();
}

} //namespace http