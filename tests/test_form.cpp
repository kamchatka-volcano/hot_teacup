#include <hot_teacup/form.h>
#include <gtest/gtest.h>

TEST(Form, WithoutFile)
{
    const auto formContentType = "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
    const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    const auto form = http::formFromString(formContentType, formData);

    ASSERT_EQ(form.size(), 2);
    EXPECT_EQ(form.at(0).name(), "param1");
    EXPECT_EQ(form.at(0).value(), "foo");
    EXPECT_EQ(form.at(0).type(), http::FormField::Type::Param);
    EXPECT_EQ(form.at(1).name(), "param2");
    EXPECT_EQ(form.at(1).value(), "bar ");
    EXPECT_EQ(form.at(0).type(), http::FormField::Type::Param);
}

TEST(Form, WithEmptyFile)
{
    const auto formContentType = "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
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
    EXPECT_EQ(form.at(0).name(), "param1");
    EXPECT_EQ(form.at(0).value(), "foo");
    EXPECT_EQ(form.at(0).type(), http::FormField::Type::Param);
    EXPECT_EQ(form.at(1).name(), "param2");
    EXPECT_EQ(form.at(1).value(), "bar ");
    EXPECT_EQ(form.at(1).type(), http::FormField::Type::Param);
    EXPECT_EQ(form.at(2).name(), "param3");
    EXPECT_EQ(form.at(2).value(), "");
    EXPECT_EQ(form.at(2).type(), http::FormField::Type::File);
    EXPECT_EQ(form.at(2).hasFile(), false);
}


TEST(Form, WithFile)
{
    const auto formContentType = "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
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
    EXPECT_EQ(form.at(0).name(), "param1");
    EXPECT_EQ(form.at(0).value(), "foo");
    EXPECT_EQ(form.at(0).type(), http::FormField::Type::Param);
    EXPECT_EQ(form.at(1).name(), "param2");
    EXPECT_EQ(form.at(1).value(), "bar ");
    EXPECT_EQ(form.at(1).type(), http::FormField::Type::Param);
    EXPECT_EQ(form.at(2).name(), "param3");
    EXPECT_EQ(form.at(2).type(), http::FormField::Type::File);
    EXPECT_EQ(form.at(2).hasFile(), true);
    EXPECT_EQ(form.at(2).fileName(), "test.gif");
    EXPECT_EQ(form.at(2).fileType(), "image/gif");
    EXPECT_EQ(form.at(2).value(), "test-gif-data");
}

TEST(Form, WithoutName)
{
    {
        const auto formContentType = "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
        const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                              "Content-Disposition: form-data;\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

        const auto form = http::formFromString(formContentType, formData);
        ASSERT_EQ(form.size(), 0);
    }
    {
        const auto formContentType = "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
        const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                              "Content-Disposition: form-data; =\"param1\"\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

        const auto form = http::formFromString(formContentType, formData);
        ASSERT_EQ(form.size(), 0);
    }
}

TEST(Form, WithoutBoundary)
{
    {
        const auto formContentType = "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
        const auto formData = "------WebKitFormBoundaryHello\r\n"
                              "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n";
        const auto form = http::formFromString(formContentType, formData);
        ASSERT_EQ(form.size(), 0);
    }
    {
        const auto formContentType = "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
        const auto formData = "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";
        const auto form = http::formFromString(formContentType, formData);
        ASSERT_EQ(form.size(), 0);
    }
    {
        const auto formContentType = "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
        const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx"
                              "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                              "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";
        const auto form = http::formFromString(formContentType, formData);
        ASSERT_EQ(form.size(), 0);
    }
    {
        const auto formContentType = "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
        const auto formData = "";
        const auto form = http::formFromString(formContentType, formData);
        ASSERT_EQ(form.size(), 0);
    }
}

TEST(Form, UrlEncoded)
{
    const auto formContentType = "Content-Type: application/x-www-form-urlencoded";
    const auto formData = "param1=foo&param2=bar&flag&param4=";

    const auto form = http::formFromString(formContentType, formData);
    ASSERT_EQ(form.size(), 3);
    EXPECT_EQ(form.at(0).name(), "param1");
    EXPECT_EQ(form.at(0).value(), "foo");
    EXPECT_EQ(form.at(1).name(), "param2");
    EXPECT_EQ(form.at(1).value(), "bar");
    EXPECT_EQ(form.at(2).name(), "param4");
    EXPECT_EQ(form.at(2).value(), "");
}

TEST(Form, UrlEncodedWithoutName)
{
    const auto formContentType = "Content-Type: application/x-www-form-urlencoded";
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
        EXPECT_EQ(form.at(0).name(), "param2");
        EXPECT_EQ(form.at(0).value(), "bar");
    }
}


