#pragma once
#include <vector>
#include <string>

namespace http{

class QueryView{
public:
    QueryView(std::string_view name, std::string_view value);
    std::string_view name() const;
    std::string_view value() const;
    friend bool operator==(const QueryView& lhs, const QueryView& rhs);

private:
    std::string_view name_;
    std::string_view value_;
};

bool operator==(const QueryView& lhs, const QueryView& rhs);
std::vector<QueryView> queriesFromString(std::string_view input);
}
