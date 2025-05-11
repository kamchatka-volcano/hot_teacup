#include <hot_teacup/request_cookie.h>
#include <hot_teacup/request_cookie_view.h>
#include <hot_teacup/header.h>
#include <hot_teacup/header_view.h>
#include <gtest/gtest.h>

TEST(RequestCookie, ToString)
{
    {
        auto cookies = std::vector<http::RequestCookie>{};
        cookies.emplace_back(http::RequestCookie{"foo", "bar"});
        cookies.emplace_back(http::RequestCookie{"Hello", "world"});
        EXPECT_EQ(http::requestCookiesToHeaderValueString(cookies), "foo=bar; Hello=world");
    }
}

TEST(RequestCookieView, FromString)
{
    {
        auto cookies = http::requestCookiesFromHeaderValueString("name=foo");
        auto expectedCookies = std::vector<http::RequestCookieView>{{"name", "foo"}};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::requestCookiesFromHeaderValueString("name=foo;test=bar");
        auto expectedCookies = std::vector<http::RequestCookieView>{{"name", "foo"}, {"test", "bar"}};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::requestCookiesFromHeaderValueString("");
        auto expectedCookies = std::vector<http::RequestCookieView>{};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::requestCookiesFromHeaderValueString("=");
        auto expectedCookies = std::vector<http::RequestCookieView>{};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::requestCookiesFromHeaderValueString(";");
        auto expectedCookies = std::vector<http::RequestCookieView>{};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::requestCookiesFromHeaderValueString(";;");
        auto expectedCookies = std::vector<http::RequestCookieView>{};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::requestCookiesFromHeaderValueString("=;=;=");
        auto expectedCookies = std::vector<http::RequestCookieView>{};
        EXPECT_EQ(cookies, expectedCookies);
    }
}

TEST(RequestCookieView, CookieFormCookieView)
{
    auto cookieView = http::RequestCookieView{"foo", "bar"};
    auto cookie = http::RequestCookie{cookieView};
    EXPECT_EQ(cookie.name(), "foo");
    EXPECT_EQ(cookie.value(), "bar");
}
