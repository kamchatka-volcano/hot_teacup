#ifndef HOT_TEACUP_QUERY_H
#define HOT_TEACUP_QUERY_H

#include "query_view.h"
#include <string>
#include <vector>
#include <variant>

namespace http {
class QueryView;

class Query {
    struct Data{
        std::string_view name() const;
        std::string_view value() const;
        std::string name_;
        std::string value_;
    };

public:
    explicit Query(const QueryView&);
    Query(std::string name, std::string value);
    std::string_view name() const;
    std::string_view value() const;
    std::string toString() const;
    friend bool operator==(const Query& lhs, const Query& rhs);

private:
    std::variant<Data, QueryView> data_;
};

std::string pathWithQueries(const std::string& path, const std::vector<Query>& queries);
std::string queriesToString(const std::vector<Query>& queries);
std::vector<Query> makeQueries(const std::vector<QueryView>& queryViewList);
} //namespace http

#endif //HOT_TEACUP_QUERY_H
