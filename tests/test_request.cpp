#include <hot_teacup/request.h>
#include <hot_teacup/request_view.h>
#include <hot_teacup/types.h>
#include <gtest/gtest.h>
#include <functional>


TEST(RequestView, RequestMethodParam)
{
    auto testRequestType = [&](std::string_view typeStr, http::RequestMethod expectedMethod)
    {
        auto request = http::RequestView{typeStr, {}, {}, {}, {}, {}, {}, {}};
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
}

TEST(RequestView, RequestFromRequestViewWithMethodParam)
{
    auto requestView = http::RequestView{"GET", {}, {}, {}, {}, {}, {}, {}};
    auto request = http::Request{requestView};
    EXPECT_EQ(request.method(), http::RequestMethod::GET);
}

TEST(RequestView, RequestIpAddress)
{
    auto request = http::RequestView{{}, "127.0.0.1", {}, {}, {}, {}, {}, {}};
    EXPECT_EQ(request.ipAddress(), "127.0.0.1");
}

TEST(RequestView, RequestDomain)
{
    auto request = http::RequestView{{}, {}, "localhost", {}, {}, {}, {}, {}};
    EXPECT_EQ(request.domainName(), "localhost");
}

TEST(RequestView, RequestPath)
{
    auto request = http::RequestView{{}, {}, {}, "/test", {}, {}, {}, {}};
    EXPECT_EQ(request.path(), "/test");
}

TEST(RequestView, RequestFromRequestViewWithPath)
{
    auto requestView = http::RequestView{{}, {}, {}, "/test", {}, {}, {}, {}};
    auto request = http::Request{requestView};
    EXPECT_EQ(request.path(), "/test");
}

TEST(RequestView, Queries)
{
    const auto request = http::RequestView{"GET", {}, {}, {}, "param1=foo&param2=bar", {}, {}, {}};
    const auto expectedQueries = std::vector<http::QueryView>{{"param1", "foo"},
                                               {"param2", "bar"}};
    EXPECT_EQ(request.queries(), expectedQueries);
    EXPECT_TRUE(request.hasQuery("param1"));
    EXPECT_EQ(request.query("param1"), "foo");
    EXPECT_TRUE(request.hasQuery("param2"));
    EXPECT_EQ(request.query("param2"), "bar");

    EXPECT_FALSE(request.hasQuery("param3"));
    EXPECT_EQ(request.query("param3"), "");
}

TEST(RequestView, RequestFromRequestViewWithQueries)
{
    const auto requestView = http::RequestView{"GET", {}, {}, {}, "param1=foo&param2=bar", {}, {}, {}};
    const auto request = http::Request{requestView};
    const auto expectedQueries = std::vector<http::Query>{{"param1", "foo"},
                                               {"param2", "bar"}};
    EXPECT_EQ(request.queries(), expectedQueries);
    EXPECT_TRUE(request.hasQuery("param1"));
    EXPECT_EQ(request.query("param1"), "foo");
    EXPECT_TRUE(request.hasQuery("param2"));
    EXPECT_EQ(request.query("param2"), "bar");

    EXPECT_FALSE(request.hasQuery("param3"));
    EXPECT_EQ(request.query("param3"), "");
}

TEST(Request, Queries)
{
    const auto expectedQueries = std::vector<http::Query>{{"param1", "foo"},
                                               {"param2", "bar"}};
    const auto request = http::Request{http::RequestMethod::GET, "/", expectedQueries, {}, {}};
    EXPECT_EQ(request.queries(), expectedQueries);
    EXPECT_TRUE(request.hasQuery("param1"));
    EXPECT_EQ(request.query("param1"), "foo");
    EXPECT_TRUE(request.hasQuery("param2"));
    EXPECT_EQ(request.query("param2"), "bar");

    EXPECT_FALSE(request.hasQuery("param3"));
    EXPECT_EQ(request.query("param3"), "");
}

TEST(RequestView, Cookies)
{
    const auto request = http::RequestView{"GET", {}, {}, {}, {}, "param1=foo;param2=bar", {}, {}};
    const auto expectedCookieList = std::vector<std::string>{"param1", "param2"};
    EXPECT_TRUE(request.hasCookie("param1"));
    EXPECT_EQ(request.cookie("param1"), "foo");
    EXPECT_TRUE(request.hasCookie("param2"));
    EXPECT_EQ(request.cookie("param2"), "bar");

    EXPECT_FALSE(request.hasCookie("param3"));
    EXPECT_EQ(request.cookie("param3"), "");
}

TEST(RequestView, RequestFromRequestViewWithCookies)
{
    const auto requestView = http::RequestView{"GET", {}, {}, {}, {}, "param1=foo;param2=bar", {}, {}};
    const auto request = http::Request{requestView};
    const auto expectedCookieList = std::vector<std::string>{"param1", "param2"};
    EXPECT_TRUE(request.hasCookie("param1"));
    EXPECT_EQ(request.cookie("param1"), "foo");
    EXPECT_TRUE(request.hasCookie("param2"));
    EXPECT_EQ(request.cookie("param2"), "bar");

    EXPECT_FALSE(request.hasCookie("param3"));
    EXPECT_EQ(request.cookie("param3"), "");
}


TEST(Request, Cookies)
{
    const auto expectedCookies = std::vector<http::Cookie>{{"param1", "foo"}, {"param2", "bar"}};
    const auto request = http::Request{http::RequestMethod::GET, "/", {}, expectedCookies};
    EXPECT_TRUE(request.hasCookie("param1"));
    EXPECT_EQ(request.cookie("param1"), "foo");
    EXPECT_TRUE(request.hasCookie("param2"));
    EXPECT_EQ(request.cookie("param2"), "bar");

    EXPECT_FALSE(request.hasCookie("param3"));
    EXPECT_EQ(request.cookie("param3"), "");
}

TEST(RequestView, MultipartFormWithFile)
{
    const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param3\"; filename=\"test.gif\"\r\n"
                          "Content-Type: image/gif\r\n\r\ntest-gif-data\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    const auto request = http::RequestView{"GET", {}, {}, {}, {}, {}, "multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx", formData};
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

TEST(RequestView, FormFromMultipartFormViewWithFile)
{
    const auto formData = "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param1\"\r\n\r\nfoo\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param2\"\r\n\r\nbar \r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx\r\n"
                          "Content-Disposition: form-data; name=\"param3\"; filename=\"test.gif\"\r\n"
                          "Content-Type: image/gif\r\n\r\ntest-gif-data\r\n"
                          "------WebKitFormBoundaryHQl9TEASIs9QyFWx--\r\n";

    const auto requestView = http::RequestView{"GET", {}, {}, {}, {}, {}, "multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx", formData};
    const auto request = http::Request{requestView};
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

TEST(Request, MultipartFormWithFile)
{
    auto form = http::Form{
        {"param1", http::FormField{"foo"}},
        {"param2", http::FormField{"bar "}},
        {"param3", http::FormField{"test-gif-data", "test.gif", "image/gif"}}};
    const auto request = http::Request{http::RequestMethod::POST, "/", {}, {}, form};

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

TEST(RequestView, UrlEncodedForm)
{
    const auto formData = "param1=foo&param2=bar&flag&param4=";

    const auto request = http::RequestView{"GET", {}, {}, {}, {}, {},
                                       "application/x-www-form-urlencoded", formData};
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


TEST(Request, UrlEncodedForm)
{
    const auto form = http::Form{{"param1", http::FormField{"foo"}},
                                 {"param2", http::FormField{"bar"}},
                                 {"param4", http::FormField{}}};

    const auto request = http::Request{http::RequestMethod::POST, "/", {}, {}, form};

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

TEST(Request, ToFcgiDataWithPathOnly)
{
    const auto request = http::Request{http::RequestMethod::GET, "/"};
    const auto fcgiData = request.toFcgiData(http::FormType::UrlEncoded);

    EXPECT_EQ(fcgiData.params.size(), 2);
    EXPECT_TRUE(fcgiData.stdIn.empty());
    ASSERT_TRUE(fcgiData.params.count("REQUEST_METHOD"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_METHOD"), "GET");
    ASSERT_TRUE(fcgiData.params.count("REQUEST_URI"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_URI"), "/");
}

TEST(Request, ToFcgiDataWithPathOnlyMultipart)
{
    const auto request = http::Request{http::RequestMethod::GET, "/"};
    const auto fcgiData = request.toFcgiData(http::FormType::Multipart);

    EXPECT_EQ(fcgiData.params.size(), 2);
    EXPECT_TRUE(fcgiData.stdIn.empty());
    ASSERT_TRUE(fcgiData.params.count("REQUEST_METHOD"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_METHOD"), "GET");
    ASSERT_TRUE(fcgiData.params.count("REQUEST_URI"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_URI"), "/");
}

TEST(Request, ToFcgiDataWithQueries)
{
    const auto request = http::Request{http::RequestMethod::GET, "/", {{"id", "100"}}};
    const auto fcgiData = request.toFcgiData(http::FormType::UrlEncoded);

    EXPECT_EQ(fcgiData.params.size(), 3);
    EXPECT_TRUE(fcgiData.stdIn.empty());
    ASSERT_TRUE(fcgiData.params.count("REQUEST_METHOD"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_METHOD"), "GET");
    ASSERT_TRUE(fcgiData.params.count("REQUEST_URI"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_URI"), "/");
    ASSERT_TRUE(fcgiData.params.count("QUERY_STRING"));
    EXPECT_EQ(fcgiData.params.at("QUERY_STRING"), "id=100");
}

TEST(Request, ToFcgiDataWithCookies)
{
    const auto request = http::Request{http::RequestMethod::GET, "/", {}, {{"id", "100"}}};
    const auto fcgiData = request.toFcgiData(http::FormType::UrlEncoded);

    EXPECT_EQ(fcgiData.params.size(), 3);
    EXPECT_TRUE(fcgiData.stdIn.empty());
    ASSERT_TRUE(fcgiData.params.count("REQUEST_METHOD"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_METHOD"), "GET");
    ASSERT_TRUE(fcgiData.params.count("REQUEST_URI"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_URI"), "/");
    ASSERT_TRUE(fcgiData.params.count("HTTP_COOKIE"));
    EXPECT_EQ(fcgiData.params.at("HTTP_COOKIE"), "id=100");
}

TEST(Request, ToFcgiDataWithUrlEncodedForm)
{
    const auto form = http::Form{{"id", http::FormField{"100"}},
                                 {"name", http::FormField{"foo"}}};
    const auto request = http::Request{http::RequestMethod::GET, "/", {}, {}, form};
    const auto fcgiData = request.toFcgiData(http::FormType::UrlEncoded);


    EXPECT_FALSE(fcgiData.stdIn.empty());
    EXPECT_EQ(fcgiData.stdIn, "id=100&name=foo");
    EXPECT_EQ(fcgiData.params.size(), 3);
    ASSERT_TRUE(fcgiData.params.count("REQUEST_METHOD"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_METHOD"), "GET");
    ASSERT_TRUE(fcgiData.params.count("REQUEST_URI"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_URI"), "/");
    ASSERT_TRUE(fcgiData.params.count("CONTENT_TYPE"));
    EXPECT_EQ(fcgiData.params.at("CONTENT_TYPE"), "application/x-www-form-urlencoded");
}

TEST(Request, ToFcgiDataWithMultipartForm)
{
    const auto form = http::Form{{"id", http::FormField{"100"}},
                                 {"name", http::FormField{"foo"}}};
    const auto request = http::Request{http::RequestMethod::GET, "/", {}, {}, form};
    const auto fcgiData = request.toFcgiData(http::FormType::Multipart);
    const auto expectedFormData =
            std::string{"------asyncgiFormBoundary\r\nContent-Disposition: form-data; name=\"id\"\r\n\r\n100\r\n"
                        "------asyncgiFormBoundary\r\nContent-Disposition: form-data; name=\"name\"\r\n\r\nfoo\r\n"
                        "------asyncgiFormBoundary--\r\n"};

    EXPECT_EQ(fcgiData.stdIn, expectedFormData);
    EXPECT_EQ(fcgiData.params.size(), 3);
    ASSERT_TRUE(fcgiData.params.count("REQUEST_METHOD"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_METHOD"), "GET");
    ASSERT_TRUE(fcgiData.params.count("REQUEST_URI"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_URI"), "/");
    ASSERT_TRUE(fcgiData.params.count("CONTENT_TYPE"));
    EXPECT_EQ(fcgiData.params.at("CONTENT_TYPE"), "multipart/form-data; boundary=----asyncgiFormBoundary");
}

