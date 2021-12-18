#pragma once
#include <string>

namespace http{

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
    PATCH,
    Unknown
};
RequestMethod methodFromString(std::string_view typeStr);

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
    return Status::Code_302_Found;
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
    return nullptr;
}

constexpr const char* contentTypeToString(ContentType type)
{
    using Type = ContentType;
    switch (type){
    case Type::HTML:
        return "text/html";
    case Type::XHTML:
        return "application/xhtml+xml";
    case Type::PlainText:
        return "text/plain";
    }
    return nullptr;
}

}

}
