#include <hot_teacup/form.h>
#include <hot_teacup/form_view.h>
#include <gtest/gtest.h>

TEST(FormView, WithoutFileFromString)
{
    const auto formContentType = "multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
    const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    const auto form = http::formFromString(formContentType, formData);

    ASSERT_EQ(form.size(), 2);
    EXPECT_EQ(form.count("param1"), 1);
    EXPECT_EQ(form.at("param1").value(), "foo");
    EXPECT_EQ(form.at("param1").type(), http::FormFieldType::Param);
    EXPECT_EQ(form.count("param2"), 1);
    EXPECT_EQ(form.at("param2").value(), "bar ");
    EXPECT_EQ(form.at("param2").type(), http::FormFieldType::Param);
}

TEST(Form, WithoutFileToString)
{
    const auto expectedUrlEncodedFormData = "param1=foo&param2=bar ";
    const auto expectedMultipartFormData =
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
            "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
            "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    auto form = http::Form{};
    form["param1"] = http::FormField{"foo"};
    form["param2"] = http::FormField{"bar "};

    EXPECT_EQ(http::urlEncodedFormToString(form), expectedUrlEncodedFormData);
    EXPECT_EQ(http::multipartFormToString(form, "----WebKitFormBoundaryHQl9TEASIs9QyFWx"), expectedMultipartFormData);
}

TEST(FormView, WithEmptyFileFromString)
{
    const auto formContentType = "multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
    const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param3\"; filename=\"\"\r\n"
                          "Content-Type: application/octet-stream\r\n\r\n\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    const auto form = http::formFromString(formContentType, formData);

    ASSERT_EQ(form.size(), 3);
    EXPECT_EQ(form.count("param1"), 1);
    EXPECT_EQ(form.at("param1").value(), "foo");
    EXPECT_EQ(form.at("param1").type(), http::FormFieldType::Param);
    EXPECT_EQ(form.count("param2"), 1);
    EXPECT_EQ(form.at("param2").value(), "bar ");
    EXPECT_EQ(form.at("param2").type(), http::FormFieldType::Param);
    EXPECT_EQ(form.count("param3"), 1);
    EXPECT_EQ(form.at("param3").value(), "");
    EXPECT_EQ(form.at("param3").type(), http::FormFieldType::File);
    EXPECT_EQ(form.at("param3").hasFile(), false);
}

TEST(Form, WithEmptyFileToString)
{
    const auto expectedUrlEncodedFormData = "param1=foo&param2=bar &param3=";
    const auto expectedMultipartFormData =
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
            "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
            "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
            "Content-Disposition: form-data; name=\"param3\"; filename=\"\"\r\n"
            "Content-Type: application/octet-stream\r\n\r\n\r\n"
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    auto form = http::Form{};
    form["param1"] = http::FormField{"foo"};
    form["param2"] = http::FormField{"bar "};
    form["param3"] = http::FormField{"", "", "application/octet-stream"};

    EXPECT_EQ(http::urlEncodedFormToString(form), expectedUrlEncodedFormData);
    EXPECT_EQ(http::multipartFormToString(form, "----WebKitFormBoundaryHQl9TEASIs9QyFWx"), expectedMultipartFormData);
}



TEST(FormView, WithFileFromString)
{
    const auto formContentType = "multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
    const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param3\"; filename=\"test.gif\"\r\n"
                          "Content-Type: image/gif\r\n\r\ntest-gif-data\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    const auto form = http::formFromString(formContentType, formData);

    ASSERT_EQ(form.size(), 3);
    EXPECT_EQ(form.count("param1"), 1);
    EXPECT_EQ(form.at("param1").value(), "foo");
    EXPECT_EQ(form.at("param1").type(), http::FormFieldType::Param);
    EXPECT_EQ(form.count("param2"), 1);
    EXPECT_EQ(form.at("param2").value(), "bar ");
    EXPECT_EQ(form.at("param2").type(), http::FormFieldType::Param);
    EXPECT_EQ(form.count("param3"), 1);
    EXPECT_EQ(form.at("param3").type(), http::FormFieldType::File);
    EXPECT_EQ(form.at("param3").hasFile(), true);
    EXPECT_EQ(form.at("param3").fileName(), "test.gif");
    EXPECT_EQ(form.at("param3").fileType(), "image/gif");
    EXPECT_EQ(form.at("param3").value(), "test-gif-data");
}

TEST(FormView, FormFromFormView)
{
    const auto formContentType = "multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
    const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param3\"; filename=\"test.gif\"\r\n"
                          "Content-Type: image/gif\r\n\r\ntest-gif-data\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    const auto formView = http::formFromString(formContentType, formData);
    const auto form = http::makeForm(formView);
    ASSERT_EQ(form.size(), 3);
    EXPECT_EQ(form.count("param1"), 1);
    EXPECT_EQ(form.at("param1").value(), "foo");
    EXPECT_EQ(form.at("param1").type(), http::FormFieldType::Param);
    EXPECT_EQ(form.count("param2"), 1);
    EXPECT_EQ(form.at("param2").value(), "bar ");
    EXPECT_EQ(form.at("param2").type(), http::FormFieldType::Param);
    EXPECT_EQ(form.count("param3"), 1);
    EXPECT_EQ(form.at("param3").type(), http::FormFieldType::File);
    EXPECT_EQ(form.at("param3").hasFile(), true);
    EXPECT_EQ(form.at("param3").fileName(), "test.gif");
    EXPECT_EQ(form.at("param3").fileType(), "image/gif");
    EXPECT_EQ(form.at("param3").value(), "test-gif-data");

}

TEST(Form, WithFileToString)
{
    const auto expectedUrlEncodedFormData = "param1=foo&param2=bar &param3=test.gif";
    const auto expectedMultipartFormData =
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
            "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
            "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
            "Content-Disposition: form-data; name=\"param3\"; filename=\"test.gif\"\r\n"
            "Content-Type: image/gif\r\n\r\ntest-gif-data\r\n"
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    auto form = http::Form{};
    form["param1"] = http::FormField{"foo"};
    form["param2"] = http::FormField{"bar "};
    form["param3"] = http::FormField{"test-gif-data", "test.gif", "image/gif"};

    EXPECT_EQ(http::urlEncodedFormToString(form), expectedUrlEncodedFormData);
    EXPECT_EQ(http::multipartFormToString(form, "----WebKitFormBoundaryHQl9TEASIs9QyFWx"), expectedMultipartFormData);
}

TEST(Form, WithFileWithoutFileTypeToString)
{
    const auto expectedUrlEncodedFormData = "param1=foo&param2=bar &param3=test.gif";
    const auto expectedMultipartFormData =
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
            "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
            "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
            "Content-Disposition: form-data; name=\"param3\"; filename=\"test.gif\"\r\n\r\ntest-gif-data\r\n"
            "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    auto form = http::Form{};
    form["param1"] = http::FormField{"foo"};
    form["param2"] = http::FormField{"bar "};
    form["param3"] = http::FormField{"test-gif-data", "test.gif"};

    EXPECT_EQ(http::urlEncodedFormToString(form), expectedUrlEncodedFormData);
    EXPECT_EQ(http::multipartFormToString(form, "----WebKitFormBoundaryHQl9TEASIs9QyFWx"), expectedMultipartFormData);
}


TEST(FormView, WithoutNameFromString)
{
    {
        const auto formContentType = "multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
        const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                              "Content-Disposition: form-data;\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

        const auto form = http::formFromString(formContentType, formData);
        ASSERT_EQ(form.size(), 0);
    }
    {
        const auto formContentType = "multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
        const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                              "Content-Disposition: form-data; =\"param1\"\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

        const auto form = http::formFromString(formContentType, formData);
        ASSERT_EQ(form.size(), 0);
    }
}

TEST(FormView, WithoutBoundaryFromString)
{
    {
        const auto formContentType = "multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
        const auto formData = "------WebKitFormBoundaryHello\r\n"
                              "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n";
        const auto form = http::formFromString(formContentType, formData);
        ASSERT_EQ(form.size(), 0);
    }
    {
        const auto formContentType = "multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
        const auto formData = "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";
        const auto form = http::formFromString(formContentType, formData);
        ASSERT_EQ(form.size(), 0);
    }
    {
        const auto formContentType = "multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
        const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx"
                              "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";
        const auto form = http::formFromString(formContentType, formData);
        ASSERT_EQ(form.size(), 0);
    }
    {
        const auto formContentType = "multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
        const auto formData = "";
        const auto form = http::formFromString(formContentType, formData);
        ASSERT_EQ(form.size(), 0);
    }
}

TEST(FormView, UrlEncodedFromString)
{
    const auto formContentType = "application/x-www-form-urlencoded";
    const auto formData = "param1=foo&param2=bar&flag&param4=";

    const auto form = http::formFromString(formContentType, formData);
    ASSERT_EQ(form.size(), 3);
    EXPECT_EQ(form.count("param1"), 1);
    EXPECT_EQ(form.at("param1").value(), "foo");
    EXPECT_EQ(form.count("param2"), 1);
    EXPECT_EQ(form.at("param2").value(), "bar");
    EXPECT_EQ(form.count("param4"), 1);
    EXPECT_EQ(form.at("param4").value(), "");
}

TEST(FormView, FormFromUrlEncodedFormView)
{
    const auto formContentType = "application/x-www-form-urlencoded";
    const auto formData = "param1=foo&param2=bar&flag&param4=";

    const auto formView = http::formFromString(formContentType, formData);
    const auto form = http::makeForm(formView);
    ASSERT_EQ(form.size(), 3);
    EXPECT_EQ(form.count("param1"), 1);
    EXPECT_EQ(form.at("param1").value(), "foo");
    EXPECT_EQ(form.count("param2"), 1);
    EXPECT_EQ(form.at("param2").value(), "bar");
    EXPECT_EQ(form.count("param4"), 1);
    EXPECT_EQ(form.at("param4").value(), "");
}

TEST(FormView, UrlEncodedWithoutNameFromString)
{
    const auto formContentType = "application/x-www-form-urlencoded";
    {
        const auto formData = "=";
        const auto form = http::formFromString(formContentType, formData);

        ASSERT_EQ(form.size(), 0);
    }
    {
        const auto formData = "";
        const auto form = http::formFromString(formContentType, formData);

        ASSERT_EQ(form.size(), 0);
    }
    {
        const auto formData = "  \t =foo";
        const auto form = http::formFromString(formContentType, formData);

        ASSERT_EQ(form.size(), 0);
    }
    {
        const auto formData = "  \t =foo&param2=bar";
        const auto form = http::formFromString(formContentType, formData);

        ASSERT_EQ(form.size(), 1);
        EXPECT_EQ(form.count("param2"), 1);
        EXPECT_EQ(form.at("param2").value(), "bar");
    }
}


