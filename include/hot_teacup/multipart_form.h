#ifndef HOT_TEACUP_MULTIPART_FORM_H
#define HOT_TEACUP_MULTIPART_FORM_H

#include "multipart_form_impl.h"
#include "multipart_form_view.h"
#include "types.h"
#include "detail/copy_on_write_interface.h"
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace http {

class MultipartFormField : public detail::ICopyOnWrite {
    struct FormFile {
        std::string fileData;
        std::string fileName;
        std::string mimeType;
    };
    class Data {
    public:
        Data(std::string name, std::string value);
        Data(std::string name, FormFile file);
        std::string_view name() const;
        std::string_view fileName() const;
        std::string_view fileType() const;
        std::string_view value() const;
        MultipartFormFieldType type() const;

    private:
        std::string name_;
        std::variant<std::string, FormFile> value_;
    };

public:
    explicit MultipartFormField(const MultipartFormFieldView&);
    MultipartFormField(std::string name, std::string value);
    MultipartFormField(std::string name, std::string fileData, std::string fileName, std::string fileType = {});
    std::string_view name() const;
    MultipartFormFieldType type() const;
    std::string_view fileName() const;
    std::string_view fileType() const;
    std::string_view value() const;

    friend bool operator==(const MultipartFormField& lhs, const MultipartFormField& rhs);

private:
    bool isView() const override;
    void makeOwnStateFromView() override;

private:
    std::variant<Data, MultipartFormFieldView> data_;
};

class MultipartForm : public detail::ICopyOnWrite {
    struct Data : detail::MultipartFormImpl<std::string, MultipartFormField> {
        using MultipartFormImpl::MultipartFormImpl;

        std::vector<MultipartFormFieldView> fields() const;
    };

public:
    explicit MultipartForm(MultipartFormView formView);
    explicit MultipartForm(std::vector<MultipartFormField> fields);

    std::vector<MultipartFormFieldView> fields() const;

    std::vector<std::string_view> paramList() const;
    std::vector<std::string_view> fileList() const;

    std::string_view param(std::string_view name, int index = 0) const;
    int paramsCount() const;
    int paramCount(std::string_view name) const;
    bool hasParam(std::string_view name) const;
    bool hasParams() const;

    std::string_view fileData(std::string_view name, int index = 0) const;
    std::string_view fileName(std::string_view name, int index = 0) const;
    std::string_view fileType(std::string_view name, int index = 0) const;
    int filesCount() const;
    int fileCount(std::string_view name) const;
    bool hasFile(std::string_view name) const;
    bool hasFiles() const;

    std::string toString() const;
    MultipartFormView toView() const;

private:
    bool isView() const override;
    void makeOwnStateFromView() override;

private:
    std::variant<Data, MultipartFormView> data_;
};

} //namespace http

#endif //HOT_TEACUP_MULTIPART_FORM_H
