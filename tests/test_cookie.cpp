#include <hot_teacup/cookie.h>
#include <hot_teacup/header.h>
#include <gtest/gtest.h>

TEST(Cookie, ToString)
{
    {
        auto cookie = http::Cookie{"foo", "bar"};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar");
    }

    {
        auto cookie = http::Cookie{"foo", "bar"};
        cookie.setMaxAge(std::chrono::seconds{10});
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=10");
    }

    {
        auto cookie = http::Cookie{"foo", "bar"};
        cookie.setMaxAge(std::chrono::minutes{1});
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=60");
    }

    {
        auto cookie = http::Cookie{"foo", "bar"};
        cookie.setDomain("localhost");
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost");
    }

    {
        auto cookie = http::Cookie{"foo", "bar"};
        cookie.setMaxAge(std::chrono::seconds{10});
        cookie.setDomain("localhost");
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=10; Domain=localhost");
    }

    {
        auto cookie = http::Cookie{"foo", "bar"};
        cookie.setPath("/test");
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Path=/test");
    }

    {
        auto cookie = http::Cookie{"foo", "bar"};
        cookie.setMaxAge(std::chrono::seconds{10});
        cookie.setPath("/test");
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=10; Path=/test");
    }

    {
        auto cookie = http::Cookie{"foo", "bar"};
        cookie.setDomain("localhost");
        cookie.setPath("/test");
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost; Path=/test");
    }

    {
        auto cookie = http::Cookie{"foo", "bar"};
        cookie.setMaxAge(std::chrono::seconds{10});
        cookie.setDomain("localhost");
        cookie.setPath("/test");
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=10; Domain=localhost; Path=/test");
    }

    {
        auto cookie = http::Cookie{"foo", "bar"};
        cookie.setDomain("localhost");
        cookie.setPath("/test");
        cookie.remove();
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost; Path=/test; Max-Age=0");
    }

    {
        auto cookie = http::Cookie{"foo", "bar"};
        cookie.setDomain("localhost");
        cookie.setPath("/test");
        cookie.secure();
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost; Path=/test; Secure");
    }

    {
        auto cookie = http::Cookie{"foo", "bar"};
        cookie.setDomain("localhost");
        cookie.setPath("/test");
        cookie.remove();
        cookie.secure();
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost; Path=/test; Max-Age=0; Secure");
    }

    {
        auto cookies = http::Cookies{};
        cookies.emplace_back(http::Cookie{"foo", "bar"});
        cookies.emplace_back(http::Cookie{"Hello", "world"});
        EXPECT_EQ(cookiesToString(cookies), "foo=bar; Hello=world");
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

TEST(Cookie, FromHeader)
{
   {
        auto header = http::Header{"Set-Cookie", ""};
        header.setParam("foo", "bar");
        header.setParam("Max-Age", "10");
        header.setParam("Domain", "localhost");
        header.setParam("Path", "/test");
        header.setParam("Secure", "");
        auto cookie = http::cookieFromHeader(header);
        ASSERT_TRUE(cookie);
        EXPECT_EQ(cookie->name(), "foo");
        EXPECT_EQ(cookie->value(), "bar");
        EXPECT_EQ(cookie->maxAge(), std::chrono::seconds{10});
        EXPECT_EQ(cookie->domain(), "localhost");
        EXPECT_EQ(cookie->isSecure(), true);
    }
    {
        auto header = http::Header{"Set-Cookie", ""};
        header.setParam("foo", "bar");
        header.setParam("Max-Age", "0");
        auto cookie = http::cookieFromHeader(header);
        ASSERT_TRUE(cookie);
        EXPECT_EQ(cookie->name(), "foo");
        EXPECT_EQ(cookie->value(), "bar");
        EXPECT_EQ(cookie->isRemoved(), true);
    }
}
