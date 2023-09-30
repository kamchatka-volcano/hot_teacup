#ifndef HOT_TEACUP_QUERY_VIEW_H
#define HOT_TEACUP_QUERY_VIEW_H

#include <string>
#include <vector>

namespace http {

class QueryView {
public:
    QueryView(std::string_view name, std::string_view value);
    std::string_view name() const;
    std::string_view value() const;
    friend bool operator==(const QueryView& lhs, const QueryView& rhs);

private:
    std::string_view name_;
    std::string_view value_;
};

std::vector<QueryView> queriesFromString(std::string_view input);
} //namespace http

#endif //HOT_TEACUP_QUERY_VIEW_H
