#ifndef HOT_TEACUP_QUERY_H
#define HOT_TEACUP_QUERY_H

#include "query_view.h"
#include "detail/copy_on_write_interface.h"
#include <string>
#include <variant>
#include <vector>

namespace http {
class QueryView;

class Query : public detail::ICopyOnWrite {
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
    bool isView() const override;
    void makeOwnStateFromView() override;

private:
    std::variant<Data, QueryView> data_;
};

using Queries = std::vector<Query>;

std::string pathWithQueries(const std::string& path, const std::vector<Query>& queries);
std::string queriesToString(const std::vector<Query>& queries);
std::vector<Query> makeQueries(const std::vector<QueryView>& queryViewList);
} //namespace http

#endif //HOT_TEACUP_QUERY_H
