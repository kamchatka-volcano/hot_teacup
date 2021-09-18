#include <hot_teacup/request.h>
#include <hot_teacup/types.h>
#include <gtest/gtest.h>
#include <functional>


TEST(Request, RequestMethodParam)
{
    auto testRequestType = [&](const std::string& typeStr, http::RequestMethod expectedMethod)
    {
        auto request = http::Request{typeStr};
        EXPECT_EQ(request.method(), expectedMethod);
    };

    testRequestType("GET", http::RequestMethod::GET);
    testRequestType("POST", http::RequestMethod::POST);
    testRequestType("PUT", http::RequestMethod::PUT);
    testRequestType("HEAD", http::RequestMethod::HEAD);
    testRequestType("PATCH", http::RequestMethod::PATCH);
    testRequestType("TRACE", http::RequestMethod::TRACE);
    testRequestType("DELETE", http::RequestMethod::DELETE);
    testRequestType("CONNECT", http::RequestMethod::CONNECT);
    testRequestType("OPTIONS", http::RequestMethod::OPTIONS);
    testRequestType("FOO", http::RequestMethod::Unknown);
}

TEST(Request, Queries)
{
    const auto request = http::Request{"GET", "param1=foo&param2=bar"};
    const auto expectedQueries = http::Queries{{"param1", std::string{"foo"}}, {"param2", std::string{"bar"}}};
    EXPECT_EQ(request.queries(), expectedQueries);
    EXPECT_TRUE(request.hasQuery("param1"));
    EXPECT_EQ(request.query("param1"), "foo");
    EXPECT_TRUE(request.hasQuery("param2"));
    EXPECT_EQ(request.query("param2"), "bar");

    EXPECT_FALSE(request.hasQuery("param3"));
    EXPECT_EQ(request.query("param3"), "");
}

TEST(Request, Cookies)
{
    const auto request = http::Request{"GET", "", "param1=foo;param2=bar"};
    const auto expectedCookieList = std::vector<std::string>{"param1", "param2"};
    EXPECT_EQ(request.cookieList(), expectedCookieList);
    EXPECT_TRUE(request.hasCookie("param1"));
    EXPECT_EQ(request.cookie("param1"), "foo");
    EXPECT_TRUE(request.hasCookie("param2"));
    EXPECT_EQ(request.cookie("param2"), "bar");

    EXPECT_FALSE(request.hasCookie("param3"));
    EXPECT_EQ(request.cookie("param3"), "");
}

TEST(Request, MultipartFormWithoutFile)
{
    const auto formContentType = "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx";
    const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    const auto request = http::Request{"GET", {}, {}, formContentType, formData};
    const auto expectedFormFieldList = std::vector<std::string>{"param1", "param2"};

    EXPECT_EQ(request.formFieldList(), expectedFormFieldList);
    EXPECT_TRUE(request.hasFormField("param1"));
    EXPECT_EQ(request.formField("param1"), "foo");
    EXPECT_TRUE(request.hasFormField("param2"));
    EXPECT_EQ(request.formField("param2"), "bar ");

    EXPECT_FALSE(request.hasFiles());
    EXPECT_FALSE(request.hasFormField("param3"));
    EXPECT_EQ(request.formField("param3"), "");
    EXPECT_FALSE(request.hasFile("param3"));
    EXPECT_EQ(request.fileData("param3"), "");
    EXPECT_EQ(request.fileName("param3"), "");
    EXPECT_EQ(request.fileType("param3"), "");
}

TEST(Request, MultipartFormWithEmptyFile)
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

    const auto request = http::Request{"GET", {}, {}, formContentType, formData};
    const auto expectedFormFieldList = std::vector<std::string>{"param1", "param2"};
    EXPECT_EQ(request.formFieldList(), expectedFormFieldList);
    EXPECT_TRUE(request.hasFormField("param1"));
    EXPECT_EQ(request.formField("param1"), "foo");
    EXPECT_TRUE(request.hasFormField("param2"));
    EXPECT_EQ(request.formField("param2"), "bar ");

    EXPECT_FALSE(request.hasFiles());
    EXPECT_FALSE(request.hasFormField("param3"));
    EXPECT_EQ(request.formField("param3"), "");
    EXPECT_FALSE(request.hasFile("param3"));
    EXPECT_EQ(request.fileData("param3"), "");
    EXPECT_EQ(request.fileName("param3"), "");
    EXPECT_EQ(request.fileType("param3"), "");
}

TEST(Request, MultipartFormWithFile)
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

    const auto request = http::Request{"GET", {}, {}, formContentType, formData};
    const auto expectedFormFieldList = std::vector<std::string>{"param1", "param2"};
    EXPECT_EQ(request.formFieldList(), expectedFormFieldList);
    EXPECT_TRUE(request.hasFormField("param1"));
    EXPECT_EQ(request.formField("param1"), "foo");
    EXPECT_TRUE(request.hasFormField("param2"));
    EXPECT_EQ(request.formField("param2"), "bar ");

    EXPECT_TRUE(request.hasFiles());
    EXPECT_FALSE(request.hasFormField("param3"));
    EXPECT_EQ(request.formField("param3"), "");
    EXPECT_TRUE(request.hasFile("param3"));
    EXPECT_EQ(request.fileData("param3"), "test-gif-data");
    EXPECT_EQ(request.fileName("param3"), "test.gif");
    EXPECT_EQ(request.fileType("param3"), "image/gif");
}

TEST(Request, UrlEncodedWithoutFile)
{
    const auto formContentType = "Content-Type: application/x-www-form-urlencoded";
    const auto formData = "param1=foo&param2=bar&flag&param4=";

    const auto request = http::Request{"GET", {}, {}, formContentType, formData};
    auto expectedFormFieldList = std::vector<std::string>{"param1", "param2", "param4"};
    EXPECT_EQ(request.formFieldList(), expectedFormFieldList);
    EXPECT_TRUE(request.hasFormField("param1"));
    EXPECT_EQ(request.formField("param1"), "foo");
    EXPECT_TRUE(request.hasFormField("param2"));
    EXPECT_EQ(request.formField("param2"), "bar");
    EXPECT_TRUE(request.hasFormField("param4"));
    EXPECT_EQ(request.formField("param4"), "");

    EXPECT_FALSE(request.hasFiles());
    EXPECT_FALSE(request.hasFormField("param3"));
    EXPECT_EQ(request.formField("param3"), "");
    EXPECT_FALSE(request.hasFile("param3"));
    EXPECT_EQ(request.fileData("param3"), "");
    EXPECT_EQ(request.fileName("param3"), "");
    EXPECT_EQ(request.fileType("param3"), "");
}

