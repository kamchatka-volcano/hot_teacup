#pragma once
#include <string_view>
#include <exception>

namespace http{

enum class ResponseMode{
    Standard,
    CGI
};

enum class ResponseStatus{
    Code_100_Continue,
    Code_101_Switching_Protocol,
    Code_102_Processing,
    Code_103_Early_Hints,
    Code_200_Ok,
    Code_201_Created,
    Code_202_Accepted,
    Code_203_Non_Authoritative_Information,
    Code_204_No_Content,
    Code_205_Reset_Content,
    Code_206_Partial_Content,
    Code_207_Multi_Status,
    Code_208_Already_Reported,
    Code_226_IM_Used,
    Code_300_Multiple_Choice,
    Code_301_Moved_Permanently,
    Code_302_Found,
    Code_303_See_Other,
    Code_304_Not_Modified,
    Code_307_Temporary_Redirect,
    Code_308_Permanent_Redirect,
    Code_400_Bad_Request,
    Code_401_Unauthorized,
    Code_402_Payment_Required,
    Code_403_Forbidden,
    Code_404_Not_Found,
    Code_405_Method_Not_Allowed,
    Code_406_Not_Acceptable,
    Code_407_Proxy_Authentication_Required,
    Code_408_Request_Timeout,
    Code_409_Conflict,
    Code_410_Gone,
    Code_411_Length_Required,
    Code_412_Precondition_Failed,
    Code_413_Payload_Too_Large,
    Code_414_URI_Too_Long,
    Code_415_Unsupported_Media_Type,
    Code_416_Range_Not_Satisfiable,
    Code_417_Expectation_Failed,
    Code_418_Im_a_teapot,
    Code_421_Misdirected_Request,
    Code_422_Unprocessable_Entity,
    Code_423_Locked,
    Code_424_Failed_Dependency,
    Code_425_Too_Early,
    Code_426_Upgrade_Required,
    Code_428_Precondition_Required,
    Code_429_Too_Many_Requests,
    Code_431_Request_Header_Fields_Too_Large,
    Code_451_Unavailable_For_Legal_Reasons,
    Code_500_Internal_Server_Error,
    Code_501_Not_Implemented,
    Code_502_Bad_Gateway,
    Code_503_Service_Unavailable,
    Code_504_Gateway_Timeout,
    Code_505_HTTP_Version_Not_Supported,
    Code_506_Variant_Also_Negotiates,
    Code_507_Insufficient_Storage,
    Code_508_Loop_Detected,
    Code_510_Not_Extended,
    Code_511_Network_Authentication_Required
};

enum class ContentType{
    HTML,
    XHTML,
    PlainText
};

enum class RedirectType{
    MovedPermanently,
    PermanentRedirect,
    Found,
    SeeOther,
    TemporaryRedirect,
    MultipleChoice,
    NotModified
};

enum class RequestMethod{
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH
};

namespace detail {
[[noreturn]]
inline void ensureNotReachable() noexcept
{
    std::terminate();
}
}

constexpr RequestMethod methodFromString(std::string_view typeStr)
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
        return {};
}

constexpr const char* methodToString(RequestMethod method)
{
    switch (method)
    {
    case RequestMethod::GET:
        return "GET";
    case RequestMethod::HEAD:
        return "HEAD";
    case RequestMethod::POST:
        return "POST";
    case RequestMethod::PUT:
        return "PUT";
    case RequestMethod::DELETE:
        return "DELETE";
    case RequestMethod::CONNECT:
        return "CONNECT";
    case RequestMethod::OPTIONS:
        return "OPTIONS";
    case RequestMethod::TRACE:
        return "TRACE";
    case RequestMethod::PATCH:
        return "PATCH";
    }
    detail::ensureNotReachable();
}

enum class FormFieldType {
    Param,
    File
};

enum class FormType{
    UrlEncoded,
    Multipart
};

enum class HeaderQuotingMode{
    None,
    HeaderValue,
    ParamValue,
    AllValues
};

namespace detail{
constexpr ResponseStatus redirectTypeStatus(RedirectType redirectType)
{
    using Type = RedirectType;
    using Status = ResponseStatus;
    switch(redirectType){
    case Type::MovedPermanently:
        return Status::Code_301_Moved_Permanently;
    case Type::PermanentRedirect:
        return Status::Code_308_Permanent_Redirect;
    case Type::Found:
        return Status::Code_302_Found;
    case Type::SeeOther:
        return Status::Code_303_See_Other;
    case Type::TemporaryRedirect:
        return Status::Code_307_Temporary_Redirect;
    case Type::MultipleChoice:
        return Status::Code_300_Multiple_Choice;
    case Type::NotModified:
        return Status::Code_304_Not_Modified;
    }
    detail::ensureNotReachable();
}

constexpr const char* statusToString(ResponseStatus status)
{
    using Status = ResponseStatus;
    switch(status){
    case Status::Code_100_Continue: return "100 Continue";
    case Status::Code_101_Switching_Protocol: return "101 Switching Protocol";
    case Status::Code_102_Processing: return "102 Processing";
    case Status::Code_103_Early_Hints: return "103 Early Hints";
    case Status::Code_200_Ok: return "200 OK";
    case Status::Code_201_Created: return "201 Created";
    case Status::Code_202_Accepted: return "202 Accepted";
    case Status::Code_203_Non_Authoritative_Information: return "203 Non-Authoritative Information";
    case Status::Code_204_No_Content: return "204 No Content";
    case Status::Code_205_Reset_Content: return "205 Reset Content";
    case Status::Code_206_Partial_Content: return "206 Partial Content";
    case Status::Code_207_Multi_Status: return "207 Multi-Status";
    case Status::Code_208_Already_Reported: return "208 Already Reported";
    case Status::Code_226_IM_Used: return "226 IM Used";
    case Status::Code_300_Multiple_Choice: return "300 Multiple Choice";
    case Status::Code_301_Moved_Permanently: return "301 Moved Permanently";
    case Status::Code_302_Found: return "302 Found";
    case Status::Code_303_See_Other: return "303 See Other";
    case Status::Code_304_Not_Modified: return "304 Not Modified";
    case Status::Code_307_Temporary_Redirect: return "307 Temporary Redirect";
    case Status::Code_308_Permanent_Redirect: return "308 Permanent Redirect";
    case Status::Code_400_Bad_Request: return "400 Bad Request";
    case Status::Code_401_Unauthorized: return "401 Unauthorized";
    case Status::Code_402_Payment_Required: return "402 Payment Required";
    case Status::Code_403_Forbidden: return "403 Forbidden";
    case Status::Code_404_Not_Found: return "404 Not Found";
    case Status::Code_405_Method_Not_Allowed: return "405 Method Not Allowed";
    case Status::Code_406_Not_Acceptable: return "406 Not Acceptable";
    case Status::Code_407_Proxy_Authentication_Required: return "407 Proxy Authentication Required";
    case Status::Code_408_Request_Timeout: return "408 Request Timeout";
    case Status::Code_409_Conflict: return "409 Conflict";
    case Status::Code_410_Gone: return "410 Gone";
    case Status::Code_411_Length_Required: return "411 Length Required";
    case Status::Code_412_Precondition_Failed: return "412 Precondition Failed";
    case Status::Code_413_Payload_Too_Large: return "413 Payload Too Large";
    case Status::Code_414_URI_Too_Long: return "414 URI Too Long";
    case Status::Code_415_Unsupported_Media_Type: return "415 Unsupported Media Type";
    case Status::Code_416_Range_Not_Satisfiable: return "416 Range Not Satisfiable";
    case Status::Code_417_Expectation_Failed: return "417 Expectation Failed";
    case Status::Code_418_Im_a_teapot: return "418 I'm a teapot";
    case Status::Code_421_Misdirected_Request: return "421 Misdirected Request";
    case Status::Code_422_Unprocessable_Entity: return "422 Unprocessable Entity";
    case Status::Code_423_Locked: return "423 Locked";
    case Status::Code_424_Failed_Dependency: return "424 Failed Dependency";
    case Status::Code_425_Too_Early: return "425 Too Early";
    case Status::Code_426_Upgrade_Required: return "426 Upgrade Required";
    case Status::Code_428_Precondition_Required: return "428 Precondition Required";
    case Status::Code_429_Too_Many_Requests: return "429 Too Many Requests";
    case Status::Code_431_Request_Header_Fields_Too_Large: return "431 Request Header Fields Too Large";
    case Status::Code_451_Unavailable_For_Legal_Reasons: return "451 Unavailable For Legal Reasons";
    case Status::Code_500_Internal_Server_Error: return "500 Internal Server Error";
    case Status::Code_501_Not_Implemented: return "501 Not Implemented";
    case Status::Code_502_Bad_Gateway: return "502 Bad Gateway";
    case Status::Code_503_Service_Unavailable: return "503 Service Unavailable";
    case Status::Code_504_Gateway_Timeout: return "504 Gateway Timeout";
    case Status::Code_505_HTTP_Version_Not_Supported: return "505 HTTP Version Not Supported";
    case Status::Code_506_Variant_Also_Negotiates: return "506 Variant Also Negotiates";
    case Status::Code_507_Insufficient_Storage: return "507 Insufficient Storage";
    case Status::Code_508_Loop_Detected: return "508 Loop Detected";
    case Status::Code_510_Not_Extended: return "510 Not Extended";
    case Status::Code_511_Network_Authentication_Required: return "511 Network Authentication Required";
    }
    detail::ensureNotReachable();
}

constexpr ResponseStatus statusFromCode(int statusCode)
{
    using Status = ResponseStatus;
    switch (statusCode) {
        case 100:
            return Status::Code_100_Continue;
        case 101:
            return Status::Code_101_Switching_Protocol;
        case 102:
            return Status::Code_102_Processing;
        case 103:
            return Status::Code_103_Early_Hints;
        case 200:
            return Status::Code_200_Ok;
        case 201:
            return Status::Code_201_Created;
        case 202:
            return Status::Code_202_Accepted;
        case 203:
            return Status::Code_203_Non_Authoritative_Information;
        case 204:
            return Status::Code_204_No_Content;
        case 205:
            return Status::Code_205_Reset_Content;
        case 206:
            return Status::Code_206_Partial_Content;
        case 207:
            return Status::Code_207_Multi_Status;
        case 208:
            return Status::Code_208_Already_Reported;
        case 226:
            return Status::Code_226_IM_Used;
        case 300:
            return Status::Code_300_Multiple_Choice;
        case 301:
            return Status::Code_301_Moved_Permanently;
        case 302:
            return Status::Code_302_Found;
        case 303:
            return Status::Code_303_See_Other;
        case 304:
            return Status::Code_304_Not_Modified;
        case 307:
            return Status::Code_307_Temporary_Redirect;
        case 308:
            return Status::Code_308_Permanent_Redirect;
        case 400:
            return Status::Code_400_Bad_Request;
        case 401:
            return Status::Code_401_Unauthorized;
        case 402:
            return Status::Code_402_Payment_Required;
        case 403:
            return Status::Code_403_Forbidden;
        case 404:
            return Status::Code_404_Not_Found;
        case 405:
            return Status::Code_405_Method_Not_Allowed;
        case 406:
            return Status::Code_406_Not_Acceptable;
        case 407:
            return Status::Code_407_Proxy_Authentication_Required;
        case 408:
            return Status::Code_408_Request_Timeout;
        case 409:
            return Status::Code_409_Conflict;
        case 410:
            return Status::Code_410_Gone;
        case 411:
            return Status::Code_411_Length_Required;
        case 412:
            return Status::Code_412_Precondition_Failed;
        case 413:
            return Status::Code_413_Payload_Too_Large;
        case 414:
            return Status::Code_414_URI_Too_Long;
        case 415:
            return Status::Code_415_Unsupported_Media_Type;
        case 416:
            return Status::Code_416_Range_Not_Satisfiable;
        case 417:
            return Status::Code_417_Expectation_Failed;
        case 418:
            return Status::Code_418_Im_a_teapot;
        case 421:
            return Status::Code_421_Misdirected_Request;
        case 422:
            return Status::Code_422_Unprocessable_Entity;
        case 423:
            return Status::Code_423_Locked;
        case 424:
            return Status::Code_424_Failed_Dependency;
        case 425:
            return Status::Code_425_Too_Early;
        case 426:
            return Status::Code_426_Upgrade_Required;
        case 428:
            return Status::Code_428_Precondition_Required;
        case 429:
            return Status::Code_429_Too_Many_Requests;
        case 431:
            return Status::Code_431_Request_Header_Fields_Too_Large;
        case 451:
            return Status::Code_451_Unavailable_For_Legal_Reasons;
        case 500:
            return Status::Code_500_Internal_Server_Error;
        case 501:
            return Status::Code_501_Not_Implemented;
        case 502:
            return Status::Code_502_Bad_Gateway;
        case 503:
            return Status::Code_503_Service_Unavailable;
        case 504:
            return Status::Code_504_Gateway_Timeout;
        case 505:
            return Status::Code_505_HTTP_Version_Not_Supported;
        case 506:
            return Status::Code_506_Variant_Also_Negotiates;
        case 507:
            return Status::Code_507_Insufficient_Storage;
        case 508:
            return Status::Code_508_Loop_Detected;
        case 510:
            return Status::Code_510_Not_Extended;
        case 511:
            return Status::Code_511_Network_Authentication_Required;
        default:
            return {};
    }
}


constexpr const char* contentTypeToString(ContentType type)
{
    switch (type){
    case ContentType::HTML:
        return "text/html";
    case ContentType::XHTML:
        return "application/xhtml+xml";
    case ContentType::PlainText:
        return "text/plain";
    }
    detail::ensureNotReachable();
}

}

}
