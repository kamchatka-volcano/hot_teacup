#pragma once
#include <vector>
#include <string>

namespace http{

class Query{
public:
    Query(std::string name, std::string value);
    const std::string& name() const;
    const std::string& value() const;
    std::string toString() const;
    bool operator==(const Query& other) const;

private:
    std::string name_;
    std::string value_;
};

using Queries = std::vector<Query>;

Queries queriesFromString(const std::string& input);
std::string queriesToString(const Queries& queries,
                            const std::vector<std::string>& queryBlackList = {});
std::string pathWithQueries(const std::string& path,
                            const Queries& queries,
                            const std::vector<std::string>& queryBlackList = {});
std::string pathWithQuery(const std::string& path, const Query& query);
}

