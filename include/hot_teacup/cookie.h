#pragma once
#include "header.h"
#include <string>
#include <vector>

namespace http{

class Cookie{

public:
    Cookie(std::string name,
           std::string value,
           int maxAgeSec = -1,
           std::string domain = {},
           std::string path = {});

    const std::string& name() const;
    const std::string& value() const;

    void setDomain(std::string domain);
    void setPath(std::string path);
    void setMaxAge(int maxAgeSec);

    void secure();
    void remove();

    std::string toString() const;

    bool operator==(const Cookie& other) const;

private:
    Header header_;
    std::string name_;
    std::string value_;
};

using Cookies = std::vector<Cookie>;
Cookies cookiesFromString(const std::string& input);

}

