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
        auto header = http::Header{};
        header.fromString("Test-Header: test");
        EXPECT_EQ(header.name(), "Test-Header");
        EXPECT_EQ(header.value(), "test");
        EXPECT_TRUE(header.paramList().empty());
    }
    {
        auto header = http::Header{};
        header.fromString("Test-Header: test; name=foo; name2=bar");
        EXPECT_EQ(header.name(), "Test-Header");
        EXPECT_EQ(header.value(), "test");
        auto expectedParamList = std::vector<std::string>{"name", "name2"};
        EXPECT_EQ(header.paramList(), expectedParamList);
        ASSERT_TRUE(header.hasParam("name"));
        EXPECT_EQ(header.param("name"), "foo");
        ASSERT_TRUE(header.hasParam("name2"));
        EXPECT_EQ(header.param("name2"), "bar");
    }
}

TEST(Header, FromStringWithoutHeaderValue)
{
    {
        auto header = http::Header{};
        header.fromString("Test-Header: \"\"");
        EXPECT_EQ(header.name(), "Test-Header");
        EXPECT_EQ(header.value(), "");
        EXPECT_TRUE(header.paramList().empty());
    }
    {
        auto header = http::Header{};
        header.fromString("Test-Header: name=foo; name2=bar");
        EXPECT_EQ(header.name(), "Test-Header");
        EXPECT_EQ(header.value(), "");
        auto expectedParamList = std::vector<std::string>{"name", "name2"};
        EXPECT_EQ(header.paramList(), expectedParamList);
        ASSERT_TRUE(header.hasParam("name"));
        EXPECT_EQ(header.param("name"), "foo");
        ASSERT_TRUE(header.hasParam("name2"));
        EXPECT_EQ(header.param("name2"), "bar");
    }
}

TEST(Header, FromStringWithQuoting)
{
    {
        auto header = http::Header{};
        header.fromString("Test-Header: \"test\"");
        EXPECT_EQ(header.name(), "Test-Header");
        EXPECT_EQ(header.value(), "test");
        EXPECT_TRUE(header.paramList().empty());
    }
    {
        auto header = http::Header{};
        header.fromString("Test-Header: \"test\"; name=\"foo\"; name2=bar");
        EXPECT_EQ(header.name(), "Test-Header");
        EXPECT_EQ(header.value(), "test");
        auto expectedParamList = std::vector<std::string>{"name", "name2"};
        EXPECT_EQ(header.paramList(), expectedParamList);
        ASSERT_TRUE(header.hasParam("name"));
        EXPECT_EQ(header.param("name"), "foo");
        ASSERT_TRUE(header.hasParam("name2"));
        EXPECT_EQ(header.param("name2"), "bar");
    }
}

TEST(Header, FromStringWithoutHeaderValueWithQuoting)
{
    auto header = http::Header{};
    header.fromString("Test-Header: name=\"foo\"; name2=bar");
    EXPECT_EQ(header.name(), "Test-Header");
    EXPECT_EQ(header.value(), "");
    auto expectedParamList = std::vector<std::string>{"name", "name2"};
    EXPECT_EQ(header.paramList(), expectedParamList);
    ASSERT_TRUE(header.hasParam("name"));
    EXPECT_EQ(header.param("name"), "foo");
    ASSERT_TRUE(header.hasParam("name2"));
    EXPECT_EQ(header.param("name2"), "bar");
}
