#include <hot_teacup/query.h>
#include <hot_teacup/query_view.h>
#include <gtest/gtest.h>

TEST(Query, ToStringSingle)
{
    auto query = http::Query{"foo", "bar"};
    EXPECT_EQ(http::queryToString(query), "foo=bar");
}

TEST(Query, ToString)
{
    {
        auto queries = std::vector<http::Query>{{"name", "test"}, {"foo", "bar"}};
        EXPECT_EQ(http::queriesToString(queries), "name=test&foo=bar");
    }
}

TEST(Query, PathWithQueries)
{
    {
        auto query = http::Query{"name", "test"};
        auto path = "/test/";
        EXPECT_EQ(http::pathWithQueries(path, {query}), "/test/?name=test");
    }
    {
        auto queries = std::vector<http::Query>{{"name", "test"}, {"foo", "bar"}};
        auto path = "/test/";
        EXPECT_EQ(http::pathWithQueries(path, queries), "/test/?name=test&foo=bar");
    }
}

TEST(QueryView, FromString)
{
    {
        auto queries = http::queriesFromString("name=test&foo=bar");
        auto expectedQueries = std::vector<http::QueryView>{{"name", "test"}, {"foo", "bar"}};
        EXPECT_EQ(queries, expectedQueries);
    }

    {
        auto queries = http::queriesFromString("");
        auto expectedQueries = std::vector<http::QueryView>{};
        EXPECT_EQ(queries, expectedQueries);
    }

    {
        auto queries = http::queriesFromString("=");
        auto expectedQueries = std::vector<http::QueryView>{};
        EXPECT_EQ(queries, expectedQueries);
    }
    {
        auto queries = http::queriesFromString("&");
        auto expectedQueries = std::vector<http::QueryView>{};
        EXPECT_EQ(queries, expectedQueries);
    }
    {
        auto queries = http::queriesFromString("&&");
        auto expectedQueries = std::vector<http::QueryView>{};
        EXPECT_EQ(queries, expectedQueries);
    }
    {
        auto queries = http::queriesFromString("=&=&=");
        auto expectedQueries = std::vector<http::QueryView>{};
        EXPECT_EQ(queries, expectedQueries);
    }
}

namespace {
std::vector<http::Query> makeQueries(const std::vector<http::QueryView>& queryViewList)
{
    auto result = std::vector<http::Query>{};
    for (const auto& queryView : queryViewList) {
        result.emplace_back(http::Query{queryView});
    }
    return result;
}
} //namespace

TEST(QueryView, QueryFromQueryView)
{
    auto queries = http::queriesFromString("name=test&foo=bar");
    auto expectedQueries = std::vector<http::Query>{{"name", "test"}, {"foo", "bar"}};
    EXPECT_EQ(makeQueries(queries), expectedQueries);
}
