#include <hot_teacup/response.h>
#include <hot_teacup/response_view.h>
#include <gtest/gtest.h>
#include <functional>

namespace {

void testResponseWithCookies(const http::Response& testResponse, const std::string& expectedResponse)
{
    {
        auto response = testResponse;
        response.addCookie(http::Cookie{"name", "foo"});
        response.addCookie(http::Cookie{"age", "77"});
        EXPECT_EQ(response.data(), expectedResponse);
    }
    {
        auto response = testResponse;
        response.addCookies({http::Cookie{"name", "foo"}, http::Cookie{"age", "77"}});
        EXPECT_EQ(response.data(), expectedResponse);
    }
}

const auto cookiesResponsePart = std::string{"Set-Cookie: name=foo\r\n"
                                             "Set-Cookie: age=77\r\n"};

void testResponseWithHeaders(const http::Response& testResponse, const std::string& expectedResponse)
{
    {
        auto response = testResponse;
        response.addHeader(http::Header{"Host", "HotTeacup"});
        response.addHeader(http::Header{"User-Agent", "gtest"});
        EXPECT_EQ(response.data(), expectedResponse);
    }
    {
        auto response = testResponse;
        response.addHeaders({http::Header{"Host", "HotTeacup"}, http::Header{"User-Agent", "gtest"}});
        EXPECT_EQ(response.data(), expectedResponse);
    }
}

const auto headersResponsePart = std::string{"Host: HotTeacup\r\n"
                                             "User-Agent: gtest\r\n"};

void testResponseWithCookiesAndHeaders(const http::Response& testResponse, const std::string& expectedResponse)
{
    auto response = testResponse;
    response.addCookie(http::Cookie{"name", "foo"});
    response.addHeader(http::Header{"Host", "HotTeacup"});
    response.addCookie(http::Cookie{"age", "77"});
    EXPECT_EQ(response.data(), expectedResponse);
}

const auto cookiesAndHeadersResponsePart = std::string{"Host: HotTeacup\r\n"
                                                       "Set-Cookie: name=foo\r\n"
                                                       "Set-Cookie: age=77\r\n"};

} //namespace

TEST(Response, Status)
{
    using Status = http::ResponseStatus;
    auto response = http::Response{Status::_100_Continue};
    EXPECT_EQ(response.data(), "HTTP/1.1 100 Continue\r\n\r\n");
    response = http::Response{Status::_101_Switching_Protocol};
    EXPECT_EQ(response.data(), "HTTP/1.1 101 Switching Protocol\r\n\r\n");
    response = http::Response{Status::_102_Processing};
    EXPECT_EQ(response.data(), "HTTP/1.1 102 Processing\r\n\r\n");
    response = http::Response{Status::_103_Early_Hints};
    EXPECT_EQ(response.data(), "HTTP/1.1 103 Early Hints\r\n\r\n");
    response = http::Response{Status::_200_Ok};
    EXPECT_EQ(response.data(), "HTTP/1.1 200 OK\r\n\r\n");
    response = http::Response{Status::_201_Created};
    EXPECT_EQ(response.data(), "HTTP/1.1 201 Created\r\n\r\n");
    response = http::Response{Status::_202_Accepted};
    EXPECT_EQ(response.data(), "HTTP/1.1 202 Accepted\r\n\r\n");
    response = http::Response{Status::_203_Non_Authoritative_Information};
    EXPECT_EQ(response.data(), "HTTP/1.1 203 Non-Authoritative Information\r\n\r\n");
    response = http::Response{Status::_204_No_Content};
    EXPECT_EQ(response.data(), "HTTP/1.1 204 No Content\r\n\r\n");
    response = http::Response{Status::_205_Reset_Content};
    EXPECT_EQ(response.data(), "HTTP/1.1 205 Reset Content\r\n\r\n");
    response = http::Response{Status::_206_Partial_Content};
    EXPECT_EQ(response.data(), "HTTP/1.1 206 Partial Content\r\n\r\n");
    response = http::Response{Status::_207_Multi_Status};
    EXPECT_EQ(response.data(), "HTTP/1.1 207 Multi-Status\r\n\r\n");
    response = http::Response{Status::_208_Already_Reported};
    EXPECT_EQ(response.data(), "HTTP/1.1 208 Already Reported\r\n\r\n");
    response = http::Response{Status::_226_IM_Used};
    EXPECT_EQ(response.data(), "HTTP/1.1 226 IM Used\r\n\r\n");
    response = http::Response{Status::_300_Multiple_Choice};
    EXPECT_EQ(response.data(), "HTTP/1.1 300 Multiple Choice\r\n\r\n");
    response = http::Response{Status::_301_Moved_Permanently};
    EXPECT_EQ(response.data(), "HTTP/1.1 301 Moved Permanently\r\n\r\n");
    response = http::Response{Status::_302_Found};
    EXPECT_EQ(response.data(), "HTTP/1.1 302 Found\r\n\r\n");
    response = http::Response{Status::_303_See_Other};
    EXPECT_EQ(response.data(), "HTTP/1.1 303 See Other\r\n\r\n");
    response = http::Response{Status::_304_Not_Modified};
    EXPECT_EQ(response.data(), "HTTP/1.1 304 Not Modified\r\n\r\n");
    response = http::Response{Status::_307_Temporary_Redirect};
    EXPECT_EQ(response.data(), "HTTP/1.1 307 Temporary Redirect\r\n\r\n");
    response = http::Response{Status::_308_Permanent_Redirect};
    EXPECT_EQ(response.data(), "HTTP/1.1 308 Permanent Redirect\r\n\r\n");
    response = http::Response{Status::_400_Bad_Request};
    EXPECT_EQ(response.data(), "HTTP/1.1 400 Bad Request\r\n\r\n");
    response = http::Response{Status::_401_Unauthorized};
    EXPECT_EQ(response.data(), "HTTP/1.1 401 Unauthorized\r\n\r\n");
    response = http::Response{Status::_402_Payment_Required};
    EXPECT_EQ(response.data(), "HTTP/1.1 402 Payment Required\r\n\r\n");
    response = http::Response{Status::_403_Forbidden};
    EXPECT_EQ(response.data(), "HTTP/1.1 403 Forbidden\r\n\r\n");
    response = http::Response{Status::_404_Not_Found};
    EXPECT_EQ(response.data(), "HTTP/1.1 404 Not Found\r\n\r\n");
    response = http::Response{Status::_405_Method_Not_Allowed};
    EXPECT_EQ(response.data(), "HTTP/1.1 405 Method Not Allowed\r\n\r\n");
    response = http::Response{Status::_406_Not_Acceptable};
    EXPECT_EQ(response.data(), "HTTP/1.1 406 Not Acceptable\r\n\r\n");
    response = http::Response{Status::_407_Proxy_Authentication_Required};
    EXPECT_EQ(response.data(), "HTTP/1.1 407 Proxy Authentication Required\r\n\r\n");
    response = http::Response{Status::_408_Request_Timeout};
    EXPECT_EQ(response.data(), "HTTP/1.1 408 Request Timeout\r\n\r\n");
    response = http::Response{Status::_409_Conflict};
    EXPECT_EQ(response.data(), "HTTP/1.1 409 Conflict\r\n\r\n");
    response = http::Response{Status::_410_Gone};
    EXPECT_EQ(response.data(), "HTTP/1.1 410 Gone\r\n\r\n");
    response = http::Response{Status::_411_Length_Required};
    EXPECT_EQ(response.data(), "HTTP/1.1 411 Length Required\r\n\r\n");
    response = http::Response{Status::_412_Precondition_Failed};
    EXPECT_EQ(response.data(), "HTTP/1.1 412 Precondition Failed\r\n\r\n");
    response = http::Response{Status::_413_Payload_Too_Large};
    EXPECT_EQ(response.data(), "HTTP/1.1 413 Payload Too Large\r\n\r\n");
    response = http::Response{Status::_414_URI_Too_Long};
    EXPECT_EQ(response.data(), "HTTP/1.1 414 URI Too Long\r\n\r\n");
    response = http::Response{Status::_415_Unsupported_Media_Type};
    EXPECT_EQ(response.data(), "HTTP/1.1 415 Unsupported Media Type\r\n\r\n");
    response = http::Response{Status::_416_Range_Not_Satisfiable};
    EXPECT_EQ(response.data(), "HTTP/1.1 416 Range Not Satisfiable\r\n\r\n");
    response = http::Response{Status::_417_Expectation_Failed};
    EXPECT_EQ(response.data(), "HTTP/1.1 417 Expectation Failed\r\n\r\n");
    response = http::Response{Status::_418_Im_a_teapot};
    EXPECT_EQ(response.data(), "HTTP/1.1 418 I'm a teapot\r\n\r\n");
    response = http::Response{Status::_421_Misdirected_Request};
    EXPECT_EQ(response.data(), "HTTP/1.1 421 Misdirected Request\r\n\r\n");
    response = http::Response{Status::_422_Unprocessable_Entity};
    EXPECT_EQ(response.data(), "HTTP/1.1 422 Unprocessable Entity\r\n\r\n");
    response = http::Response{Status::_423_Locked};
    EXPECT_EQ(response.data(), "HTTP/1.1 423 Locked\r\n\r\n");
    response = http::Response{Status::_424_Failed_Dependency};
    EXPECT_EQ(response.data(), "HTTP/1.1 424 Failed Dependency\r\n\r\n");
    response = http::Response{Status::_425_Too_Early};
    EXPECT_EQ(response.data(), "HTTP/1.1 425 Too Early\r\n\r\n");
    response = http::Response{Status::_426_Upgrade_Required};
    EXPECT_EQ(response.data(), "HTTP/1.1 426 Upgrade Required\r\n\r\n");
    response = http::Response{Status::_428_Precondition_Required};
    EXPECT_EQ(response.data(), "HTTP/1.1 428 Precondition Required\r\n\r\n");
    response = http::Response{Status::_429_Too_Many_Requests};
    EXPECT_EQ(response.data(), "HTTP/1.1 429 Too Many Requests\r\n\r\n");
    response = http::Response{Status::_431_Request_Header_Fields_Too_Large};
    EXPECT_EQ(response.data(), "HTTP/1.1 431 Request Header Fields Too Large\r\n\r\n");
    response = http::Response{Status::_451_Unavailable_For_Legal_Reasons};
    EXPECT_EQ(response.data(), "HTTP/1.1 451 Unavailable For Legal Reasons\r\n\r\n");
    response = http::Response{Status::_500_Internal_Server_Error};
    EXPECT_EQ(response.data(), "HTTP/1.1 500 Internal Server Error\r\n\r\n");
    response = http::Response{Status::_501_Not_Implemented};
    EXPECT_EQ(response.data(), "HTTP/1.1 501 Not Implemented\r\n\r\n");
    response = http::Response{Status::_502_Bad_Gateway};
    EXPECT_EQ(response.data(), "HTTP/1.1 502 Bad Gateway\r\n\r\n");
    response = http::Response{Status::_503_Service_Unavailable};
    EXPECT_EQ(response.data(), "HTTP/1.1 503 Service Unavailable\r\n\r\n");
    response = http::Response{Status::_504_Gateway_Timeout};
    EXPECT_EQ(response.data(), "HTTP/1.1 504 Gateway Timeout\r\n\r\n");
    response = http::Response{Status::_505_HTTP_Version_Not_Supported};
    EXPECT_EQ(response.data(), "HTTP/1.1 505 HTTP Version Not Supported\r\n\r\n");
    response = http::Response{Status::_506_Variant_Also_Negotiates};
    EXPECT_EQ(response.data(), "HTTP/1.1 506 Variant Also Negotiates\r\n\r\n");
    response = http::Response{Status::_507_Insufficient_Storage};
    EXPECT_EQ(response.data(), "HTTP/1.1 507 Insufficient Storage\r\n\r\n");
    response = http::Response{Status::_508_Loop_Detected};
    EXPECT_EQ(response.data(), "HTTP/1.1 508 Loop Detected\r\n\r\n");
    response = http::Response{Status::_510_Not_Extended};
    EXPECT_EQ(response.data(), "HTTP/1.1 510 Not Extended\r\n\r\n");
    response = http::Response{Status::_511_Network_Authentication_Required};
    EXPECT_EQ(response.data(), "HTTP/1.1 511 Network Authentication Required\r\n\r\n");

    response = http::Response{Status::_404_Not_Found, "Not Found"};
    EXPECT_EQ(response.data(), "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\nNot Found");
    response = http::Response{Status::_404_Not_Found, "Not Found", http::ContentType::PlainText};
    EXPECT_EQ(response.data(), "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found");
    response = http::Response{Status::_404_Not_Found, "Not Found", "text/csv"};
    EXPECT_EQ(response.data(), "HTTP/1.1 404 Not Found\r\nContent-Type: text/csv\r\n\r\nNot Found");
}

TEST(Response, StatusCGI)
{
    using Status = http::ResponseStatus;
    auto response = http::Response{Status::_200_Ok};
    EXPECT_EQ(response.data(http::ResponseMode::Cgi), "Status: 200 OK\r\n\r\n");
    response = http::Response{Status::_404_Not_Found};
    EXPECT_EQ(response.data(http::ResponseMode::Cgi), "Status: 404 Not Found\r\n\r\n");
    response = http::Response{Status::_404_Not_Found, "Not Found"};
    EXPECT_EQ(
            response.data(http::ResponseMode::Cgi),
            "Status: 404 Not Found\r\nContent-Type: text/html\r\n\r\nNot Found");
    response = http::Response{Status::_404_Not_Found, "Not Found", http::ContentType::PlainText};
    EXPECT_EQ(
            response.data(http::ResponseMode::Cgi),
            "Status: 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found");
    response = http::Response{Status::_404_Not_Found, "Not Found", "text/csv"};
    EXPECT_EQ(
            response.data(http::ResponseMode::Cgi),
            "Status: 404 Not Found\r\nContent-Type: text/csv\r\n\r\nNot Found");
}

TEST(Response, StatusWithCookies)
{
    auto expectedResponse = "HTTP/1.1 404 Not Found\r\n" + cookiesResponsePart + "\r\n";
    auto response = http::Response{http::ResponseStatus::_404_Not_Found};
    testResponseWithCookies(response, expectedResponse);
}

TEST(Response, StatusWithHeaders)
{
    auto expectedResponse = "HTTP/1.1 404 Not Found\r\n" + headersResponsePart + "\r\n";
    auto response = http::Response{http::ResponseStatus::_404_Not_Found};
    testResponseWithHeaders(response, expectedResponse);
}

TEST(Response, StatusWithCookiesAndHeaders)
{
    auto expectedResponse = "HTTP/1.1 404 Not Found\r\n" + cookiesAndHeadersResponsePart + "\r\n";
    auto response = http::Response{http::ResponseStatus::_404_Not_Found};
    testResponseWithCookiesAndHeaders(response, expectedResponse);
}

TEST(Response, Text)
{
    auto response = http::Response{"Hello world"};
    EXPECT_EQ(response.data(), "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nHello world");
    response = http::Response{"Hello world", http::ContentType::Html};
    EXPECT_EQ(response.data(), "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nHello world");
    response = http::Response{"Hello world", http::ContentType::Xhtml};
    EXPECT_EQ(response.data(), "HTTP/1.1 200 OK\r\nContent-Type: application/xhtml+xml\r\n\r\nHello world");
    response = http::Response{"Hello world", http::ContentType::PlainText};
    EXPECT_EQ(response.data(), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello world");
}

TEST(Response, TextWithCookies)
{
    auto expectedResponse = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n" +
            cookiesResponsePart + "\r\nHello world";
    auto response = http::Response{"Hello world", http::ContentType::Html};
    testResponseWithCookies(response, expectedResponse);
}

TEST(Response, TextWithHeaders)
{
    auto expectedResponse = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n" +
            headersResponsePart + "\r\nHello world";
    auto response = http::Response{"Hello world", http::ContentType::Html};
    testResponseWithHeaders(response, expectedResponse);
}

TEST(Response, TextWithCookiesAndHeaders)
{
    auto expectedResponse = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n" +
            cookiesAndHeadersResponsePart + "\r\nHello world";
    auto response = http::Response{"Hello world", http::ContentType::Html};
    testResponseWithCookiesAndHeaders(response, expectedResponse);
}

TEST(Response, Redirect)
{
    auto response = http::Response{"/", http::RedirectType::Found};
    EXPECT_EQ(response.data(), "HTTP/1.1 302 Found\r\nLocation: /\r\n\r\n");
    response = http::Response{"/", http::RedirectType::SeeOther};
    EXPECT_EQ(response.data(), "HTTP/1.1 303 See Other\r\nLocation: /\r\n\r\n");
    response = http::Response{"/", http::RedirectType::NotModified};
    EXPECT_EQ(response.data(), "HTTP/1.1 304 Not Modified\r\nLocation: /\r\n\r\n");
    response = http::Response{"/", http::RedirectType::MultipleChoice};
    EXPECT_EQ(response.data(), "HTTP/1.1 300 Multiple Choice\r\nLocation: /\r\n\r\n");
    response = http::Response{"/", http::RedirectType::MovedPermanently};
    EXPECT_EQ(response.data(), "HTTP/1.1 301 Moved Permanently\r\nLocation: /\r\n\r\n");
    response = http::Response{"/", http::RedirectType::PermanentRedirect};
    EXPECT_EQ(response.data(), "HTTP/1.1 308 Permanent Redirect\r\nLocation: /\r\n\r\n");
    response = http::Response{"/", http::RedirectType::TemporaryRedirect};
    EXPECT_EQ(response.data(), "HTTP/1.1 307 Temporary Redirect\r\nLocation: /\r\n\r\n");

    auto response2 = http::Response{http::Redirect{"/"}};
    EXPECT_EQ(response2.data(), "HTTP/1.1 302 Found\r\nLocation: /\r\n\r\n");
    response2 = http::Redirect{"/", http::RedirectType::SeeOther};
    EXPECT_EQ(response2.data(), "HTTP/1.1 303 See Other\r\nLocation: /\r\n\r\n");
}

TEST(Response, RedirectWithCookies)
{
    auto expectedResponse = "HTTP/1.1 302 Found\r\nLocation: /\r\n" + cookiesResponsePart + "\r\n";
    auto response = http::Response{"/", http::RedirectType::Found};
    testResponseWithCookies(response, expectedResponse);
}

TEST(Response, RedirectWithHeaders)
{
    auto expectedResponse = "HTTP/1.1 302 Found\r\nLocation: /\r\n" + headersResponsePart + "\r\n";
    auto response = http::Response{"/", http::RedirectType::Found};
    testResponseWithHeaders(response, expectedResponse);
}

TEST(Response, RedirectWithCookiesAndHeaders)
{
    auto expectedResponse = "HTTP/1.1 302 Found\r\nLocation: /\r\n" + cookiesAndHeadersResponsePart + "\r\n";
    auto response = http::Response{"/", http::RedirectType::Found};
    testResponseWithCookiesAndHeaders(response, expectedResponse);
}

TEST(ResponseView, HttpResponseFromString)
{
    auto responseString = std::string{"HTTP/1.1 302 Found\r\nLocation: /\r\n"
                                      "\r\n"};
    auto response = http::responseFromString(responseString);
    ASSERT_TRUE(response);
    EXPECT_EQ(response->status(), http::ResponseStatus::_302_Found);
    EXPECT_EQ(response->headers().at(0).name(), "Location");
    EXPECT_EQ(response->headers().at(0).value(), "/");
    EXPECT_EQ(response->body(), "");
}

TEST(ResponseView, CgiResponseFromString)
{
    auto responseString = std::string{"Status: 302 Found\r\nLocation: /\r\n"
                                      "\r\n"};
    auto response = http::responseFromString(responseString, http::ResponseMode::Cgi);
    ASSERT_TRUE(response);
    EXPECT_EQ(response->status(), http::ResponseStatus::_302_Found);
    EXPECT_EQ(response->headers().at(0).name(), "Location");
    EXPECT_EQ(response->headers().at(0).value(), "/");
    EXPECT_EQ(response->body(), "");
}

TEST(ResponseView, ResponseFromString2)
{
    auto responseString = std::string{"HTTP/1.1 200 OK\r\nSet-Cookie: id=hello; Max-Age=60\r\nLocation: /\r\n"
                                      "\r\nHello world"};
    auto response = http::responseFromString(responseString);
    ASSERT_TRUE(response);
    EXPECT_EQ(response->status(), http::ResponseStatus::_200_Ok);
    EXPECT_EQ(response->cookies().at(0).name(), "id");
    EXPECT_EQ(response->cookies().at(0).value(), "hello");
    EXPECT_EQ(response->cookies().at(0).maxAge(), std::chrono::minutes{1});
    EXPECT_EQ(response->headers().at(0).name(), "Location");
    EXPECT_EQ(response->headers().at(0).value(), "/");
    EXPECT_EQ(response->body(), "Hello world");
}

TEST(ResponseView, ResponseFromResponseView)
{
    auto responseString = std::string{"HTTP/1.1 200 OK\r\nSet-Cookie: id=hello; Max-Age=60\r\nLocation: /\r\n"
                                      "\r\nHello world"};
    auto responseView = http::responseFromString(responseString);
    ASSERT_TRUE(responseView);
    auto response = http::Response(*responseView);
    EXPECT_EQ(response.status(), http::ResponseStatus::_200_Ok);
    EXPECT_EQ(response.cookies().at(0).name(), "id");
    EXPECT_EQ(response.cookies().at(0).value(), "hello");
    EXPECT_EQ(response.cookies().at(0).maxAge(), std::chrono::minutes{1});
    EXPECT_EQ(response.headers().at(0).name(), "Location");
    EXPECT_EQ(response.headers().at(0).value(), "/");
    EXPECT_EQ(response.body(), "Hello world");
}

TEST(ResponseView, ResponseFromStringPartialStatus)
{
    auto responseString = std::string{"HTTP/1.1 302 \r\nLocation: /\r\n"
                                      "\r\n"};
    auto response = http::responseFromString(responseString);
    ASSERT_TRUE(response);
    EXPECT_EQ(response->status(), http::ResponseStatus::_302_Found);
    EXPECT_EQ(response->headers().at(0).name(), "Location");
    EXPECT_EQ(response->headers().at(0).value(), "/");
    EXPECT_EQ(response->body(), "");
}

TEST(ResponseView, ResponseFromStringPartialStatus2)
{
    auto responseString = std::string{"HTTP/1.1 302\r\nLocation: /\r\n"
                                      "\r\n"};
    auto response = http::responseFromString(responseString);
    ASSERT_TRUE(response);
    EXPECT_EQ(response->status(), http::ResponseStatus::_302_Found);
    EXPECT_EQ(response->headers().at(0).name(), "Location");
    EXPECT_EQ(response->headers().at(0).value(), "/");
    EXPECT_EQ(response->body(), "");
}