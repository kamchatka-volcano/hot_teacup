#include <hot_teacup/cookie.h>
#include <gtest/gtest.h>

TEST(Cookie, ToString)
{
    {
        auto cookie = http::Cookie{"foo", "bar"};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Version=1");
    }

    {
        auto cookie = http::Cookie{"foo", "bar", 0};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=0; Version=1");
    }

    {
        auto cookie = http::Cookie{"foo", "bar", -1, "localhost"};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost; Version=1");
    }

    {
        auto cookie = http::Cookie{"foo", "bar", 0, "localhost"};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=0; Domain=localhost; Version=1");
    }

    {
        auto cookie = http::Cookie{"foo", "bar", -1, {}, "/test"};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Path=/test; Version=1");
    }

    {
        auto cookie = http::Cookie{"foo", "bar", 0, {}, "/test"};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=0; Path=/test; Version=1");
    }

    {
        auto cookie = http::Cookie{"foo", "bar", -1, "localhost", "/test"};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost; Path=/test; Version=1");
    }

    {
        auto cookie = http::Cookie{"foo", "bar", 0, "localhost", "/test"};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=0; Domain=localhost; Path=/test; Version=1");
    }

    {
        auto cookie = http::Cookie{"foo", "bar", 0, "localhost", "/test"};
        cookie.remove();
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=0; Domain=localhost; Path=/test; Version=1; Expires=Fri, 01-Jan-1971 01:00:00 GMT");
    }

    {
        auto cookie = http::Cookie{"foo", "bar", 0, "localhost", "/test"};
        cookie.secure();
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=0; Domain=localhost; Path=/test; Version=1; Secure");
    }

    {
        auto cookie = http::Cookie{"foo", "bar", 0, "localhost", "/test"};
        cookie.remove();
        cookie.secure();
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=0; Domain=localhost; Path=/test; Version=1; Expires=Fri, 01-Jan-1971 01:00:00 GMT; Secure");
    }
}

TEST(Cookie, FromString)
{
    {
        auto cookies = http::cookiesFromString("name=foo");
        auto expectedCookies = http::Cookies{{"name", "foo"}};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::cookiesFromString("name=foo;test=bar");
        auto expectedCookies = http::Cookies{{"name", "foo"}, {"test", "bar"}};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::cookiesFromString("");
        auto expectedCookies = http::Cookies{};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::cookiesFromString("=");
        auto expectedCookies = http::Cookies{};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::cookiesFromString(";");
        auto expectedCookies = http::Cookies{};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::cookiesFromString(";;");
        auto expectedCookies = http::Cookies{};
        EXPECT_EQ(cookies, expectedCookies);
    }
    {
        auto cookies = http::cookiesFromString("=;=;=");
        auto expectedCookies = http::Cookies{};
        EXPECT_EQ(cookies, expectedCookies);
    }

}

