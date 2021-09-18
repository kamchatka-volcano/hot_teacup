#include <hot_teacup/query.h>
#include <gtest/gtest.h>

TEST(Query, ToStringSingle)
{
    auto query = http::Query{"foo", "bar"};
    EXPECT_EQ(query.toString(), "foo=bar");
}

TEST(Query, ToString)
{
    {
        auto queries = http::Queries{{"name", "test"}, {"foo", "bar"}};
        EXPECT_EQ(http::queriesToString(queries), "name=test&foo=bar");
    }
    {
        auto queries = http::Queries{{"name", "test"}, {"foo", "bar"}};
        EXPECT_EQ(http::queriesToString(queries, {"foo"}), "name=test");
    }
}

TEST(Query, PathWithQueries)
{
    {
        auto query = http::Query{"name", "test"};
        auto path = "/test/";
        EXPECT_EQ(http::pathWithQuery(path, query), "/test/?name=test");
    }
    {
        auto queries = http::Queries{{"name", "test"}, {"foo", "bar"}};
        auto path = "/test/";
        EXPECT_EQ(http::pathWithQueries(path, queries), "/test/?name=test&foo=bar");
    }
    {
        auto queries = http::Queries{{"name", "test"}, {"foo", "bar"}};
        auto path = "/test/";
        EXPECT_EQ(http::pathWithQueries(path, queries, {"name"}), "/test/?foo=bar");
    }
}

TEST(Query, FromString)
{
    {
        auto queries = http::queriesFromString("name=test&foo=bar");
        auto expectedQueries = http::Queries{{"name", "test"}, {"foo", "bar"}};
        EXPECT_EQ(queries, expectedQueries);
    }

    {
        auto queries = http::queriesFromString("");
        auto expectedQueries = http::Queries{};
        EXPECT_EQ(queries, expectedQueries);
    }

    {
        auto queries = http::queriesFromString("=");
        auto expectedQueries = http::Queries{};
        EXPECT_EQ(queries, expectedQueries);
    }
    {
        auto queries = http::queriesFromString("&");
        auto expectedQueries = http::Queries{};
        EXPECT_EQ(queries, expectedQueries);
    }
    {
        auto queries = http::queriesFromString("&&");
        auto expectedQueries = http::Queries{};
        EXPECT_EQ(queries, expectedQueries);
    }
    {
        auto queries = http::queriesFromString("=&=&=");
        auto expectedQueries = http::Queries{};
        EXPECT_EQ(queries, expectedQueries);
    }
}

