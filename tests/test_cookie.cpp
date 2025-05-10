#include <hot_teacup/cookie.h>
#include <hot_teacup/cookie_view.h>
#include <hot_teacup/header.h>
#include <hot_teacup/header_view.h>
#include <gtest/gtest.h>

TEST(Cookie, ToString)
{
    {
        auto cookie = http::Cookie{"foo", "bar"};
        EXPECT_EQ(cookie.toString(), "Cookie: foo=bar");
    }

    {
        auto cookies = std::vector<http::Cookie>{};
        cookies.emplace_back(http::Cookie{"foo", "bar"});
        cookies.emplace_back(http::Cookie{"Hello", "world"});
        EXPECT_EQ(cookiesToString(cookies), "foo=bar; Hello=world");
    }
}

TEST(CookieView, FromString)
{
    {
        auto cookies = http::cookiesFromString("name=foo");
        auto expectedCookies = std::vector<http::CookieView>{{"name", "foo"}};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::cookiesFromString("name=foo;test=bar");
        auto expectedCookies = std::vector<http::CookieView>{{"name", "foo"}, {"test", "bar"}};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::cookiesFromString("");
        auto expectedCookies = std::vector<http::CookieView>{};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::cookiesFromString("=");
        auto expectedCookies = std::vector<http::CookieView>{};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::cookiesFromString(";");
        auto expectedCookies = std::vector<http::CookieView>{};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::cookiesFromString(";;");
        auto expectedCookies = std::vector<http::CookieView>{};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::cookiesFromString("=;=;=");
        auto expectedCookies = std::vector<http::CookieView>{};
        EXPECT_EQ(cookies, expectedCookies);
    }
}

TEST(CookieView, FromHeader)
{
    {
        auto header = http::HeaderView{
                "Set-Cookie",
                "",
                {{"foo", "bar"}, {"Max-Age", "10"}, {"Domain", "localhost"}, {"Path", "/test"}, {"Secure", ""}}};
        auto cookie = http::cookieFromHeader(header);
        ASSERT_TRUE(cookie);
        EXPECT_EQ(cookie->name(), "foo");
        EXPECT_EQ(cookie->value(), "bar");
    }
    {
        auto header = http::HeaderView{"Set-Cookie", "", {{"foo", "bar"}, {"Max-Age", "0"}}};
        auto cookie = http::cookieFromHeader(header);
        ASSERT_TRUE(cookie);
        EXPECT_EQ(cookie->name(), "foo");
        EXPECT_EQ(cookie->value(), "bar");
    }
}

TEST(CookieView, CookieFormCookieView)
{
    auto header = http::HeaderView{
            "Set-Cookie",
            "",
            {{"foo", "bar"}, {"Max-Age", "10"}, {"Domain", "localhost"}, {"Path", "/test"}, {"Secure", ""}}};
    auto cookieView = http::cookieFromHeader(header);
    ASSERT_TRUE(cookieView);
    auto cookie = http::Cookie{*cookieView};
    EXPECT_EQ(cookie.name(), "foo");
    EXPECT_EQ(cookie.value(), "bar");
}
