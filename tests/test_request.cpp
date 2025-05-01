#include <hot_teacup/request.h>
#include <hot_teacup/request_view.h>
#include <hot_teacup/types.h>
#include <gtest/gtest.h>
#include <functional>

TEST(RequestView, RequestViewMethodParam)
{
    auto testRequestType = [&](std::string_view typeStr, http::RequestMethod expectedMethod)
    {
        auto request = http::RequestView{typeStr, {}, {}, {}, {}, {}, {}, {}, {}};
        EXPECT_EQ(request.method(), expectedMethod);
    };

    testRequestType("GET", http::RequestMethod::Get);
    testRequestType("POST", http::RequestMethod::Post);
    testRequestType("PUT", http::RequestMethod::Put);
    testRequestType("HEAD", http::RequestMethod::Head);
    testRequestType("PATCH", http::RequestMethod::Patch);
    testRequestType("TRACE", http::RequestMethod::Trace);
    testRequestType("DELETE", http::RequestMethod::Delete);
    testRequestType("CONNECT", http::RequestMethod::Connect);
    testRequestType("OPTIONS", http::RequestMethod::Options);
}

TEST(RequestView, RequestFromRequestViewWithMethodParam)
{
    auto requestView = http::RequestView{"GET", {}, {}, {}, {}, {}, {}, {}, {}};
    auto request = http::Request{requestView};
    EXPECT_EQ(request.method(), http::RequestMethod::Get);
}

TEST(RequestView, RequestViewIpAddress)
{
    auto request = http::RequestView{{}, "127.0.0.1", {}, {}, {}, {}, {}, {}, {}};
    EXPECT_EQ(request.ipAddress(), "127.0.0.1");
}

TEST(RequestView, RequestFromRequestViewIpAddress)
{
    auto request = http::RequestView{{}, "127.0.0.1", {}, {}, {}, {}, {}, {}, {}};
    EXPECT_EQ(http::Request{request}.ipAddress(), "127.0.0.1");
}

TEST(RequestView, RequestViewDomain)
{
    auto request = http::RequestView{{}, {}, "localhost", {}, {}, {}, {}, {}, {}};
    EXPECT_EQ(request.domainName(), "localhost");
}

TEST(RequestView, RequestFromRequestViewDomain)
{
    auto request = http::RequestView{{}, {}, "localhost", {}, {}, {}, {}, {}, {}};
    EXPECT_EQ(http::Request{request}.domainName(), "localhost");
}

TEST(RequestView, RequestViewPath)
{
    auto request = http::RequestView{{}, {}, {}, "/test", {}, {}, {}, {}, {}};
    EXPECT_EQ(request.path(), "/test");
}

TEST(RequestView, RequestFromRequestViewWithPath)
{
    auto requestView = http::RequestView{{}, {}, {}, "/test", {}, {}, {}, {}, {}};
    auto request = http::Request{requestView};
    EXPECT_EQ(request.path(), "/test");
}

TEST(RequestView, Queries)
{
    const auto request = http::RequestView{"GET", {}, {}, {}, "param1=foo&param2=bar", {}, {}, {}, {}};
    const auto expectedQueries = std::vector<http::QueryView>{{"param1", "foo"}, {"param2", "bar"}};
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
    const auto requestView = http::RequestView{"GET", {}, {}, {}, "param1=foo&param2=bar", {}, {}, {}, {}};
    const auto request = http::Request{requestView};
    const auto expectedQueries = std::vector<http::Query>{{"param1", "foo"}, {"param2", "bar"}};
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
    const auto expectedQueries = std::vector<http::Query>{{"param1", "foo"}, {"param2", "bar"}};
    const auto request = http::Request{http::RequestMethod::Get, "/", expectedQueries};
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
    const auto request = http::RequestView{"GET", {}, {}, {}, {}, "param1=foo;param2=bar", {}, {}, {}};
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
    const auto requestView = http::RequestView{"GET", {}, {}, {}, {}, "param1=foo;param2=bar", {}, {}, {}};
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
    const auto request = http::Request{http::RequestMethod::Get, "/", expectedCookies};
    EXPECT_TRUE(request.hasCookie("param1"));
    EXPECT_EQ(request.cookie("param1"), "foo");
    EXPECT_TRUE(request.hasCookie("param2"));
    EXPECT_EQ(request.cookie("param2"), "bar");

    EXPECT_FALSE(request.hasCookie("param3"));
    EXPECT_EQ(request.cookie("param3"), "");
}

TEST(Request, Headers)
{
    const auto expectedHeaders = std::vector<http::Header>{{"param1", "foo"}, {"param2", "bar"}};
    const auto request = http::Request{http::RequestMethod::Get, "/", expectedHeaders};
    EXPECT_TRUE(request.hasHeader("param1"));
    EXPECT_EQ(request.header("param1"), "foo");
    EXPECT_TRUE(request.hasHeader("param2"));
    EXPECT_EQ(request.header("param2"), "bar");

    EXPECT_FALSE(request.hasHeader("param3"));
    EXPECT_EQ(request.header("param3"), "");
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

    static const auto contentHeaderValue =
            std::string_view{"multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx"};
    const auto request = http::RequestView{"GET", {}, {}, {}, {}, {}, contentHeaderValue, formData, {}};
    const auto expectedParamList = std::vector<std::string_view>{"param1", "param2"};
    ASSERT_TRUE(request.multipartForm().has_value());
    const auto requestForm = request.multipartForm().value();
    EXPECT_EQ(requestForm.paramList(), expectedParamList);
    EXPECT_TRUE(requestForm.hasParam("param1"));
    EXPECT_EQ(requestForm.param("param1"), "foo");
    EXPECT_TRUE(requestForm.hasParam("param2"));
    EXPECT_EQ(requestForm.param("param2"), "bar ");

    EXPECT_TRUE(requestForm.hasFiles());
    EXPECT_FALSE(requestForm.hasParam("param3"));
    EXPECT_EQ(requestForm.param("param3"), "");
    EXPECT_TRUE(requestForm.hasFile("param3"));
    EXPECT_EQ(requestForm.fileData("param3"), "test-gif-data");
    EXPECT_EQ(requestForm.fileName("param3"), "test.gif");
    EXPECT_EQ(requestForm.fileType("param3"), "image/gif");
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

    const auto requestView = http::RequestView{
            "GET",
            {},
            {},
            {},
            {},
            {},
            "multipart/form-data; boundary=----WebKitFormBoundaryHQl9TEASIs9QyFWx",
            formData,
            {}};
    const auto request = http::Request{requestView};
    const auto expectedParamList = std::vector<std::string_view>{"param1", "param2"};
    ASSERT_TRUE(request.multipartForm().has_value());
    const auto requestForm = request.multipartForm().value();
    EXPECT_EQ(requestForm.paramList(), expectedParamList);
    EXPECT_TRUE(requestForm.hasParam("param1"));
    EXPECT_EQ(requestForm.param("param1"), "foo");
    EXPECT_TRUE(requestForm.hasParam("param2"));
    EXPECT_EQ(requestForm.param("param2"), "bar ");

    EXPECT_TRUE(requestForm.hasFiles());
    EXPECT_FALSE(requestForm.hasParam("param3"));
    EXPECT_EQ(requestForm.param("param3"), "");
    EXPECT_TRUE(requestForm.hasFile("param3"));
    EXPECT_EQ(requestForm.fileData("param3"), "test-gif-data");
    EXPECT_EQ(requestForm.fileName("param3"), "test.gif");
    EXPECT_EQ(requestForm.fileType("param3"), "image/gif");
}

TEST(Request, MultipartFormWithFile)
{
    auto form = http::MultipartForm{
            {{"param1", "foo"}, {"param2", "bar "}, {"param3", "test-gif-data", "test.gif", "image/gif"}}};
    const auto request = http::Request{http::RequestMethod::Post, "/", form};

    const auto expectedParamList = std::vector<std::string_view>{"param1", "param2"};
    ASSERT_TRUE(request.multipartForm().has_value());
    const auto requestForm = request.multipartForm().value();
    EXPECT_EQ(requestForm.paramList(), expectedParamList);
    EXPECT_TRUE(requestForm.hasParam("param1"));
    EXPECT_EQ(requestForm.param("param1"), "foo");
    EXPECT_TRUE(requestForm.hasParam("param2"));
    EXPECT_EQ(requestForm.param("param2"), "bar ");

    EXPECT_TRUE(requestForm.hasFiles());
    EXPECT_FALSE(requestForm.hasParam("param3"));
    EXPECT_EQ(requestForm.param("param3"), "");
    EXPECT_TRUE(requestForm.hasFile("param3"));
    EXPECT_EQ(requestForm.fileData("param3"), "test-gif-data");
    EXPECT_EQ(requestForm.fileName("param3"), "test.gif");
    EXPECT_EQ(requestForm.fileType("param3"), "image/gif");
}

TEST(RequestView, UrlEncodedForm)
{
    const auto formData = "param1=foo&param2=bar&flag&param4=";

    const auto request =
            http::RequestView{"GET", {}, {}, {}, {}, {}, "application/x-www-form-urlencoded", formData, {}};
    auto expectedParamList = std::vector<std::string_view>{"param1", "param2", "param4"};

    ASSERT_TRUE(request.urlEncodedForm().has_value());
    const auto requestForm = request.urlEncodedForm().value();

    EXPECT_EQ(requestForm.paramList(), expectedParamList);
    EXPECT_TRUE(requestForm.hasParam("param1"));
    EXPECT_EQ(requestForm.param("param1"), "foo");
    EXPECT_TRUE(requestForm.hasParam("param2"));
    EXPECT_EQ(requestForm.param("param2"), "bar");
    EXPECT_TRUE(requestForm.hasParam("param4"));
    EXPECT_EQ(requestForm.param("param4"), "");
}

TEST(Request, UrlEncodedForm)
{
    const auto form = http::UrlEncodedForm{{{"param1", "foo"}, {"param2", "bar"}, {"param4", ""}}};

    const auto request = http::Request{http::RequestMethod::Post, "/", form};

    auto expectedParamList = std::vector<std::string_view>{"param1", "param2", "param4"};
    ASSERT_TRUE(request.urlEncodedForm().has_value());
    const auto requestForm = request.urlEncodedForm().value();

    EXPECT_EQ(requestForm.paramList(), expectedParamList);
    EXPECT_TRUE(requestForm.hasParam("param1"));
    EXPECT_EQ(requestForm.param("param1"), "foo");
    EXPECT_TRUE(requestForm.hasParam("param2"));
    EXPECT_EQ(requestForm.param("param2"), "bar");
    EXPECT_TRUE(requestForm.hasParam("param4"));
    EXPECT_EQ(requestForm.param("param4"), "");
}

TEST(Request, ToFcgiDataWithPathOnly)
{
    const auto request = http::Request{http::RequestMethod::Get, "/"};
    const auto fcgiData = request.toFcgiData();

    EXPECT_EQ(fcgiData.params.size(), 2);
    EXPECT_TRUE(fcgiData.stdIn.empty());
    ASSERT_TRUE(fcgiData.params.count("REQUEST_METHOD"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_METHOD"), "GET");
    ASSERT_TRUE(fcgiData.params.count("REQUEST_URI"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_URI"), "/");
}

TEST(Request, ToFcgiDataWithQueries)
{
    const auto request = http::Request{http::RequestMethod::Get, "/", std::vector<http::Query>{{"id", "100"}}};
    const auto fcgiData = request.toFcgiData();

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
    const auto request = http::Request{http::RequestMethod::Get, "/", std::vector<http::Cookie>{{"id", "100"}}};
    const auto fcgiData = request.toFcgiData();

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
    const auto form = http::UrlEncodedForm{{{"id", "100"}, {"name", "foo"}}};
    const auto request = http::Request{http::RequestMethod::Get, "/", form};
    const auto fcgiData = request.toFcgiData();

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
    const auto form = http::MultipartForm{{{"id", "100"}, {"name", "foo"}}};
    const auto request = http::Request{http::RequestMethod::Get, "/", form};
    const auto fcgiData = request.toFcgiData();
    const auto expectedFormData = std::string{
            "------hot_teacupFormBoundaryBEx29lOmZnk=\r\nContent-Disposition: form-data; name=\"id\"\r\n\r\n100\r\n"
            "------hot_teacupFormBoundaryBEx29lOmZnk=\r\nContent-Disposition: form-data; name=\"name\"\r\n\r\nfoo\r\n"
            "------hot_teacupFormBoundaryBEx29lOmZnk=--\r\n"};

    EXPECT_EQ(fcgiData.stdIn, expectedFormData);
    EXPECT_EQ(fcgiData.params.size(), 3);
    ASSERT_TRUE(fcgiData.params.count("REQUEST_METHOD"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_METHOD"), "GET");
    ASSERT_TRUE(fcgiData.params.count("REQUEST_URI"));
    EXPECT_EQ(fcgiData.params.at("REQUEST_URI"), "/");
    ASSERT_TRUE(fcgiData.params.count("CONTENT_TYPE"));
    EXPECT_EQ(
            fcgiData.params.at("CONTENT_TYPE"),
            "multipart/form-data; boundary=----hot_teacupFormBoundaryBEx29lOmZnk=");
}

TEST(Request, CopyInternalStateOnMutation)
{
    std::string path = "/test";
    const auto requestView = http::RequestView{"GET", {}, {}, path, "param1=foo&param2=bar", {}, {}, {}, {}};
    auto request = http::Request{requestView};
    EXPECT_EQ(request.path(), "/test");
    path = "/1111";
    EXPECT_EQ(request.path(), "/1111");
    request.setIpAddress("127.0.0.1"); // Method modifying the object should create the copy of the internal state
    path = "/2222";
    EXPECT_EQ(request.path(), "/1111");
}
