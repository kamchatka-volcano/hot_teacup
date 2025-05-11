#include <hot_teacup/header.h>
#include <hot_teacup/header_view.h>
#include <hot_teacup/response_cookie.h>
#include <hot_teacup/response_cookie_view.h>
#include <gtest/gtest.h>

TEST(ResponseCookie, ToString)
{
    {
        auto cookie = http::ResponseCookie{"foo", "bar"};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar");
    }

    {
        auto cookie = http::ResponseCookie{"foo", "bar"};
        cookie.setMaxAge(std::chrono::seconds{10});
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=10");
    }

    {
        auto cookie = http::ResponseCookie{"foo", "bar"};
        cookie.setMaxAge(std::chrono::minutes{1});
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=60");
    }
    {
        auto cookie = http::ResponseCookie{"foo", "bar", http::CookieMaxAge{std::chrono::minutes{1}}};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=60");
    }

    {
        auto cookie = http::ResponseCookie{"foo", "bar"};
        cookie.setDomain("localhost");
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost");
    }
    {
        auto cookie = http::ResponseCookie{"foo", "bar", http::CookieDomain{"localhost"}};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost");
    }

    {
        auto cookie = http::ResponseCookie{"foo", "bar"};
        cookie.setMaxAge(std::chrono::seconds{10});
        cookie.setDomain("localhost");
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=10; Domain=localhost");
    }
    {
        auto cookie = http::ResponseCookie{
                "foo",
                "bar",
                http::CookieMaxAge{std::chrono::seconds{10}},
                http::CookieDomain{"localhost"}};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=10; Domain=localhost");
    }

    {
        auto cookie = http::ResponseCookie{"foo", "bar"};
        cookie.setPath("/test");
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Path=/test");
    }
    {
        auto cookie = http::ResponseCookie{"foo", "bar", http::CookiePath{"/test"}};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Path=/test");
    }

    {
        auto cookie = http::ResponseCookie{"foo", "bar"};
        cookie.setMaxAge(std::chrono::seconds{10});
        cookie.setPath("/test");
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=10; Path=/test");
    }

    {
        auto cookie = http::ResponseCookie{"foo", "bar"};
        cookie.setDomain("localhost");
        cookie.setPath("/test");
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost; Path=/test");
    }

    {
        auto cookie = http::ResponseCookie{"foo", "bar"};
        cookie.setMaxAge(std::chrono::seconds{10});
        cookie.setDomain("localhost");
        cookie.setPath("/test");
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Max-Age=10; Domain=localhost; Path=/test");
    }

    {
        auto cookie = http::ResponseCookie{"foo", "bar"};
        cookie.setDomain("localhost");
        cookie.setPath("/test");
        cookie.setRemoved();
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost; Path=/test; Max-Age=0");
    }

    {
        auto cookie = http::ResponseCookie{"foo", "bar"};
        cookie.setDomain("localhost");
        cookie.setPath("/test");
        cookie.setSecure();
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost; Path=/test; Secure");
    }

    {
        auto cookie = http::ResponseCookie{"foo", "bar"};
        cookie.setDomain("localhost");
        cookie.setPath("/test");
        cookie.setRemoved();
        cookie.setSecure();
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost; Path=/test; Max-Age=0; Secure");
    }
    {
        auto cookie = http::ResponseCookie{
                "foo",
                "bar",
                http::CookieDomain{"localhost"},
                http::CookiePath{"/test"},
                http::CookieIsRemoved{},
                http::CookieIsSecure{}};
        EXPECT_EQ(cookie.toString(), "Set-Cookie: foo=bar; Domain=localhost; Path=/test; Max-Age=0; Secure");
    }
}

TEST(ResponseCookieView, FromHeader)
{
    {
        auto header = http::HeaderView{
                "Set-Cookie",
                "",
                {{"foo", "bar"}, {"Max-Age", "10"}, {"Domain", "localhost"}, {"Path", "/test"}, {"Secure", ""}}};
        auto cookie = http::responseCookieFromHeader(header);
        ASSERT_TRUE(cookie);
        EXPECT_EQ(cookie->name(), "foo");
        EXPECT_EQ(cookie->value(), "bar");
        EXPECT_EQ(cookie->maxAge(), std::chrono::seconds{10});
        EXPECT_EQ(cookie->domain(), "localhost");
        EXPECT_EQ(cookie->isSecure(), true);
    }
    {
        auto header = http::HeaderView{"Set-Cookie", "", {{"foo", "bar"}, {"Max-Age", "0"}}};
        auto cookie = http::responseCookieFromHeader(header);
        ASSERT_TRUE(cookie);
        EXPECT_EQ(cookie->name(), "foo");
        EXPECT_EQ(cookie->value(), "bar");
        EXPECT_EQ(cookie->isRemoved(), true);
    }
}

TEST(ResponseCookieView, CookieFormCookieView)
{
    auto header = http::HeaderView{
            "Set-Cookie",
            "",
            {{"foo", "bar"}, {"Max-Age", "10"}, {"Domain", "localhost"}, {"Path", "/test"}, {"Secure", ""}}};
    auto cookieView = http::responseCookieFromHeader(header);
    ASSERT_TRUE(cookieView);
    auto cookie = http::ResponseCookie{*cookieView};
    EXPECT_EQ(cookie.name(), "foo");
    EXPECT_EQ(cookie.value(), "bar");
    EXPECT_EQ(cookie.maxAge(), std::chrono::seconds{10});
    EXPECT_EQ(cookie.domain(), "localhost");
    EXPECT_EQ(cookie.isSecure(), true);
}
