#include <hot_teacup/header.h>
#include <gtest/gtest.h>

TEST(Header, ToString)
{
    {
        auto header = http::Header{"Test-Header", "test"};
        EXPECT_EQ(header.toString(), "Test-Header: test");
    }
    {
        auto header = http::Header{"Test-Header", "test"};
        header.setParam("name", "foo");
        header.setParam("name2", "bar");
        EXPECT_EQ(header.toString(), "Test-Header: test; name=foo; name2=bar");
    }
}

TEST(Header, ToStringWithoutHeaderValue)
{
    {
        auto header = http::Header{"Test-Header", ""};
        EXPECT_EQ(header.toString(), "Test-Header: \"\"");
    }
    {
        auto header = http::Header{"Test-Header", ""};
        header.setParam("name", "foo");
        header.setParam("name2", "bar");
        EXPECT_EQ(header.toString(), "Test-Header: name=foo; name2=bar");
    }
}

TEST(Header, ToStringQuotingMode)
{
    {
        auto header = http::Header{"Test-Header", "test"};
        header.setParam("name", "foo");
        header.setParam("name2", "bar");
        header.setQuotingMode(http::Header::QuotingMode::ParamValue);
        EXPECT_EQ(header.toString(), "Test-Header: test; name=\"foo\"; name2=\"bar\"");
    }
    {
        auto header = http::Header{"Test-Header", "test"};
        header.setParam("name", "foo");
        header.setParam("name2", "bar");
        header.setQuotingMode(http::Header::QuotingMode::HeaderValue);
        EXPECT_EQ(header.toString(), "Test-Header: \"test\"; name=foo; name2=bar");
    }
    {
        auto header = http::Header{"Test-Header", "test"};
        header.setParam("name", "foo");
        header.setParam("name2", "bar");
        header.setQuotingMode(http::Header::QuotingMode::AllValues);
        EXPECT_EQ(header.toString(), "Test-Header: \"test\"; name=\"foo\"; name2=\"bar\"");
    }
}
TEST(Header, ToStringWithoutHeaderValueQuotingMode){
    {
        auto header = http::Header{"Test-Header", ""};
        header.setParam("name", "foo");
        header.setParam("name2", "bar");
        header.setQuotingMode(http::Header::QuotingMode::ParamValue);
        EXPECT_EQ(header.toString(), "Test-Header: name=\"foo\"; name2=\"bar\"");
    }
    {
        auto header = http::Header{"Test-Header", ""};
        header.setParam("name", "foo");
        header.setParam("name2", "bar");
        header.setQuotingMode(http::Header::QuotingMode::HeaderValue);
        EXPECT_EQ(header.toString(), "Test-Header: name=foo; name2=bar");
    }
    {
        auto header = http::Header{"Test-Header", "test"};
        header.setParam("name", "foo");
        header.setParam("name2", "bar");
        header.setQuotingMode(http::Header::QuotingMode::AllValues);
        EXPECT_EQ(header.toString(), "Test-Header: \"test\"; name=\"foo\"; name2=\"bar\"");
    }
}

TEST(Header, FromString)
{
    {
        auto header = http::headerFromString("Test-Header: test");
        ASSERT_TRUE(header.has_value());
        EXPECT_EQ(header->name(), "Test-Header");
        EXPECT_EQ(header->value(), "test");
        EXPECT_TRUE(header->params().empty());
    }
    {
        auto header = http::headerFromString("Test-Header: test; name=foo; name2=bar");
        ASSERT_TRUE(header.has_value());
        EXPECT_EQ(header->name(), "Test-Header");
        EXPECT_EQ(header->value(), "test");
        EXPECT_EQ(header->params().size(), 2);
        ASSERT_TRUE(header->hasParam("name"));
        EXPECT_EQ(header->param("name"), "foo");
        ASSERT_TRUE(header->hasParam("name2"));
        EXPECT_EQ(header->param("name2"), "bar");
    }
}

TEST(Header, FromStringWithoutHeaderValue)
{
    {
        auto header = http::headerFromString("Test-Header: \"\"");
        ASSERT_TRUE(header.has_value());
        EXPECT_EQ(header->name(), "Test-Header");
        EXPECT_EQ(header->value(), "");
        EXPECT_TRUE(header->params().empty());
    }
    {
        auto header = http::headerFromString("Test-Header: name=foo; name2=bar");
        ASSERT_TRUE(header.has_value());
        EXPECT_EQ(header->name(), "Test-Header");
        EXPECT_EQ(header->value(), "");
        EXPECT_EQ(header->params().size(), 2);
        ASSERT_TRUE(header->hasParam("name"));
        EXPECT_EQ(header->param("name"), "foo");
        ASSERT_TRUE(header->hasParam("name2"));
        EXPECT_EQ(header->param("name2"), "bar");
    }
}

TEST(Header, FromStringWithQuoting)
{
    {
        auto header = http::headerFromString("Test-Header: \"test\"");
        ASSERT_TRUE(header.has_value());
        EXPECT_EQ(header->name(), "Test-Header");
        EXPECT_EQ(header->value(), "test");
        EXPECT_TRUE(header->params().empty());
    }
    {
        auto header = http::headerFromString("Test-Header: \"test\"; name=\"foo\"; name2=bar");
        ASSERT_TRUE(header.has_value());
        EXPECT_EQ(header->name(), "Test-Header");
        EXPECT_EQ(header->value(), "test");
        EXPECT_EQ(header->params().size(), 2);
        ASSERT_TRUE(header->hasParam("name"));
        EXPECT_EQ(header->param("name"), "foo");
        ASSERT_TRUE(header->hasParam("name2"));
        EXPECT_EQ(header->param("name2"), "bar");
    }
}

TEST(Header, FromStringWithoutHeaderValueWithQuoting)
{
    auto header = http::headerFromString("Test-Header: name=\"foo\"; name2=bar");
    ASSERT_TRUE(header.has_value());
    EXPECT_EQ(header->name(), "Test-Header");
    EXPECT_EQ(header->value(), "");
    EXPECT_EQ(header->params().size(), 2);
    ASSERT_TRUE(header->hasParam("name"));
    EXPECT_EQ(header->param("name"), "foo");
    ASSERT_TRUE(header->hasParam("name2"));
    EXPECT_EQ(header->param("name2"), "bar");
}

TEST(Header, FromStringWithoutHeaderName)
{
    {
    auto header = http::headerFromString(": name=\"foo\"; name2=bar");
    ASSERT_FALSE(header.has_value());
    }
    {
        auto header = http::headerFromString(":");
        ASSERT_FALSE(header.has_value());
    }
    {
        auto header = http::headerFromString(" \t : name=\"foo\"; name2=bar");
        ASSERT_FALSE(header.has_value());
    }
    {
        auto header = http::headerFromString("");
        ASSERT_FALSE(header.has_value());
    }
    {
        auto header = http::headerFromString("Hello world!");
        ASSERT_FALSE(header.has_value());
    }
}
