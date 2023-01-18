#ifndef HOT_TEACUP_QUERY_H
#define HOT_TEACUP_QUERY_H

#include <string>
#include <vector>

namespace http {
class QueryView;

class Query {
public:
    explicit Query(const QueryView&);
    Query(std::string name, std::string value);
    const std::string& name() const;
    const std::string& value() const;
    std::string toString() const;
    friend bool operator==(const Query& lhs, const Query& rhs);

private:
    std::string name_;
    std::string value_;
};
bool operator==(const Query& lhs, const Query& rhs);

std::string pathWithQueries(const std::string& path, const std::vector<Query>& queries);
std::string queriesToString(const std::vector<Query>& queries);
std::vector<Query> makeQueries(const std::vector<QueryView>& queryViewList);
} //namespace http

#endif //HOT_TEACUP_QUERY_H
