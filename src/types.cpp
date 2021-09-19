#include <hot_teacup/types.h>

namespace http{

RequestMethod methodFromString(const std::string& typeStr)
{
    if (typeStr == "GET")
        return RequestMethod::GET;
    else if (typeStr == "HEAD")
        return RequestMethod::HEAD;
    else if (typeStr == "POST")
        return RequestMethod::POST;
    else if (typeStr == "PUT")
        return RequestMethod::PUT;
    else if (typeStr == "DELETE")
        return RequestMethod::DELETE;
    else if (typeStr == "CONNECT")
        return RequestMethod::CONNECT;
    else if (typeStr == "OPTIONS")
        return RequestMethod::OPTIONS;
    else if (typeStr == "TRACE")
        return RequestMethod::TRACE;
    else if (typeStr == "PATCH")
        return RequestMethod::PATCH;
    else
        return RequestMethod::Unknown;
}

}
