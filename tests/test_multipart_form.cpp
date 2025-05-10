#include <hot_teacup/header_view.h>
#include <hot_teacup/multipart_form.h>
#include <hot_teacup/multipart_form_view.h>
#include <gtest/gtest.h>

TEST(MultipartFormView, WithoutFileFromString)
{
    const auto contentType = http::HeaderView{
            "Content-Type",
            "multipart/form-data",
            {{"boundary", "----WebKitFormBoundaryHQl9TEASIs9QyFWx"}}};
    const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    const auto formResult = http::multipartFormViewFromString(contentType, formData);
    ASSERT_TRUE(formResult.has_value());
    const auto& form = formResult.value();
    ASSERT_EQ(form.paramsCount(), 2);
    EXPECT_EQ(form.paramCount("param1"), 1);
    EXPECT_EQ(form.param("param1"), "foo");
    EXPECT_EQ(form.paramCount("param2"), 1);
    EXPECT_EQ(form.param("param2"), "bar ");
}

TEST(MultipartForm, WithoutFileToString)
{
    const auto expectedMultipartFormData = "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
                                           "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                                           "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
                                           "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                                           "------hot_teacupFormBoundaryBEx29lOmZnk=--\r\n";

    auto form = http::MultipartForm{{
            {"param1", "foo"},
            {"param2", "bar "},
    }};

    EXPECT_EQ(form.toString(), expectedMultipartFormData);
}

TEST(MultipartFormView, WithEmptyFileFromString)
{
    const auto formContentType = http::HeaderView{
            "Content-Type",
            "multipart/form-data",
            {{"boundary", "----hot_teacupFormBoundaryBEx29lOmZnk="}}};
    const auto formData = "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
                          "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                          "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
                          "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                          "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
                          "Content-Disposition: form-data; name=\"param3\"; filename=\"\"\r\n"
                          "Content-Type: application/octet-stream\r\n\r\n\r\n"
                          "------hot_teacupFormBoundaryBEx29lOmZnk=--\r\n";

    const auto formResult = http::multipartFormViewFromString(formContentType, formData);
    ASSERT_TRUE(formResult.has_value());
    const auto& form = formResult.value();

    ASSERT_EQ(form.paramsCount(), 2);
    ASSERT_EQ(form.filesCount(), 1);

    EXPECT_EQ(form.paramCount("param1"), 1);
    EXPECT_EQ(form.param("param1"), "foo");
    EXPECT_EQ(form.paramCount("param2"), 1);
    EXPECT_EQ(form.param("param2"), "bar ");
    EXPECT_EQ(form.fileCount("param3"), 1);
    EXPECT_EQ(form.fileData("param3"), "");
    EXPECT_EQ(form.hasFile("param3"), true);
}

TEST(MultipartForm, WithEmptyFileToString)
{
    const auto expectedMultipartFormData = "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
                                           "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                                           "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
                                           "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                                           "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
                                           "Content-Disposition: form-data; name=\"param3\"; filename=\"\"\r\n"
                                           "Content-Type: application/octet-stream\r\n\r\n\r\n"
                                           "------hot_teacupFormBoundaryBEx29lOmZnk=--\r\n";

    auto form = http::MultipartForm{{
            {"param1", "foo"},
            {"param2", "bar "},
            {"param3", "", "", "application/octet-stream"},
    }};
    EXPECT_EQ(form.toString(), expectedMultipartFormData);
}

TEST(MultipartFormView, WithFileFromString)
{
    const auto contentType = http::HeaderView{
            "Content-Type",
            "multipart/form-data",
            {{"boundary", "----WebKitFormBoundaryHQl9TEASIs9QyFWx"}}};
    const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param3\"; filename=\"test.gif\"\r\n"
                          "Content-Type: image/gif\r\n\r\ntest-gif-data\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    const auto formResult = http::multipartFormViewFromString(contentType, formData);
    ASSERT_TRUE(formResult.has_value());
    const auto& form = formResult.value();

    ASSERT_EQ(form.paramsCount(), 2);
    ASSERT_EQ(form.filesCount(), 1);
    EXPECT_EQ(form.paramCount("param1"), 1);
    EXPECT_EQ(form.param("param1"), "foo");
    EXPECT_EQ(form.paramCount("param2"), 1);
    EXPECT_EQ(form.param("param2"), "bar ");
    EXPECT_EQ(form.fileCount("param3"), 1);
    EXPECT_EQ(form.fileName("param3"), "test.gif");
    EXPECT_EQ(form.fileType("param3"), "image/gif");
    EXPECT_EQ(form.fileData("param3"), "test-gif-data");
}

// TEST(MultipartFormView, FormFromFormView)
// {
//     const auto contentType = http::HeaderView{
//             "Content-Type",
//             "multipart/form-data",
//             {{"boundary", "----WebKitFormBoundaryHQl9TEASIs9QyFWx"}}};
//     const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
//                           "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
//                           "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
//                           "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
//                           "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
//                           "Content-Disposition: form-data; name=\"param3\"; filename=\"test.gif\"\r\n"
//                           "Content-Type: image/gif\r\n\r\ntest-gif-data\r\n"
//                           "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";
//
//     const auto formViewResult = http::multipartFormViewFromString(contentType, formData);
//     ASSERT_TRUE(formViewResult.has_value());
//     const auto form = http::MultipartForm(formViewResult.value());
//
//     ASSERT_EQ(form.paramsCount(), 2);
//     ASSERT_EQ(form.filesCount(), 1);
//     EXPECT_EQ(form.paramCount("param1"), 1);
//     EXPECT_EQ(form.param("param1"), "foo");
//     EXPECT_EQ(form.paramCount("param2"), 1);
//     EXPECT_EQ(form.param("param2"), "bar ");
//     EXPECT_EQ(form.fileCount("param3"), 1);
//     EXPECT_EQ(form.fileName("param3"), "test.gif");
//     EXPECT_EQ(form.fileType("param3"), "image/gif");
//     EXPECT_EQ(form.fileData("param3"), "test-gif-data");
// }

TEST(MultipartForm, WithFileToString)
{
    const auto expectedMultipartFormData = "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
                                           "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                                           "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
                                           "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                                           "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
                                           "Content-Disposition: form-data; name=\"param3\"; filename=\"test.gif\"\r\n"
                                           "Content-Type: image/gif\r\n\r\ntest-gif-data\r\n"
                                           "------hot_teacupFormBoundaryBEx29lOmZnk=--\r\n";

    auto form = http::MultipartForm{{
            {"param1", "foo"},
            {"param2", "bar "},
            {"param3", "test-gif-data", "test.gif", "image/gif"},
    }};
    EXPECT_EQ(form.toString(), expectedMultipartFormData);
}

TEST(MultipartForm, WithFileWithoutFileTypeToString)
{
    const auto expectedMultipartFormData =
            "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
            "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
            "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
            "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
            "------hot_teacupFormBoundaryBEx29lOmZnk=\r\n"
            "Content-Disposition: form-data; name=\"param3\"; filename=\"test.gif\"\r\n\r\ntest-gif-data\r\n"
            "------hot_teacupFormBoundaryBEx29lOmZnk=--\r\n";

    auto form = http::MultipartForm{{{"param1", "foo"}, {"param2", "bar "}, {"param3", "test-gif-data", "test.gif"}}};
    EXPECT_EQ(form.toString(), expectedMultipartFormData);
}

TEST(MultipartFormView, WithoutNameFromString)
{
    {
        const auto contentType = http::HeaderView{
                "Content-Type",
                "multipart/form-data",
                {{"boundary", "----WebKitFormBoundaryHQl9TEASIs9QyFWx"}}};
        const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                              "Content-Disposition: form-data;\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

        const auto formResult = http::multipartFormViewFromString(contentType, formData);
        ASSERT_FALSE(formResult.has_value());
    }
    {
        const auto contentType = http::HeaderView{"Content-Type", "multipart/form-data", {{"boundary", "----WebKitFormBoundaryHQl9TEASIs9QyFWx"}}};
        const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                              "Content-Disposition: form-data; =\"param1\"\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

        const auto formResult = http::multipartFormViewFromString(contentType, formData);
        ASSERT_FALSE(formResult.has_value());
    }
}

TEST(MultipartFormView, WithoutBoundaryFromString)
{
    {
        const auto contentType = http::HeaderView{"Content-Type", "multipart/form-data", {{"boundary", "----WebKitFormBoundaryHQl9TEASIs9QyFWx"}}};
        const auto formData = "------WebKitFormBoundaryHello\r\n"
                              "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n";
        const auto formResult = http::multipartFormViewFromString(contentType, formData);
        ASSERT_FALSE(formResult.has_value());
    }
    {
        const auto contentType = http::HeaderView{"Content-Type", "multipart/form-data", {{"boundary", "----WebKitFormBoundaryHQl9TEASIs9QyFWx"}}};
        const auto formData = "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";
        const auto formResult = http::multipartFormViewFromString(contentType, formData);
        ASSERT_FALSE(formResult.has_value());
    }
    {
        const auto contentType = http::HeaderView{"Content-Type", "multipart/form-data", {{"boundary", "----WebKitFormBoundaryHQl9TEASIs9QyFWx"}}};
        const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx"
                              "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";
        const auto formResult = http::multipartFormViewFromString(contentType, formData);
        ASSERT_FALSE(formResult.has_value());
    }
    {
        const auto contentType = http::HeaderView{
                "Content-Type",
                "multipart/form-data",
                {{"boundary", "----WebKitFormBoundaryHQl9TEASIs9QyFWx"}}};
        const auto formData = "";
        const auto formResult = http::multipartFormViewFromString(contentType, formData);
        ASSERT_FALSE(formResult.has_value());
    }
}
