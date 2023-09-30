#ifndef HOT_TEACUP_TYPES_H
#define HOT_TEACUP_TYPES_H

#include <exception>
#include <string_view>

namespace http {

enum class ResponseMode {
    Http,
    Cgi
};

enum class ResponseStatus {
    _100_Continue,
    _101_Switching_Protocol,
    _102_Processing,
    _103_Early_Hints,
    _200_Ok,
    _201_Created,
    _202_Accepted,
    _203_Non_Authoritative_Information,
    _204_No_Content,
    _205_Reset_Content,
    _206_Partial_Content,
    _207_Multi_Status,
    _208_Already_Reported,
    _226_IM_Used,
    _300_Multiple_Choice,
    _301_Moved_Permanently,
    _302_Found,
    _303_See_Other,
    _304_Not_Modified,
    _307_Temporary_Redirect,
    _308_Permanent_Redirect,
    _400_Bad_Request,
    _401_Unauthorized,
    _402_Payment_Required,
    _403_Forbidden,
    _404_Not_Found,
    _405_Method_Not_Allowed,
    _406_Not_Acceptable,
    _407_Proxy_Authentication_Required,
    _408_Request_Timeout,
    _409_Conflict,
    _410_Gone,
    _411_Length_Required,
    _412_Precondition_Failed,
    _413_Payload_Too_Large,
    _414_URI_Too_Long,
    _415_Unsupported_Media_Type,
    _416_Range_Not_Satisfiable,
    _417_Expectation_Failed,
    _418_Im_a_teapot,
    _421_Misdirected_Request,
    _422_Unprocessable_Entity,
    _423_Locked,
    _424_Failed_Dependency,
    _425_Too_Early,
    _426_Upgrade_Required,
    _428_Precondition_Required,
    _429_Too_Many_Requests,
    _431_Request_Header_Fields_Too_Large,
    _451_Unavailable_For_Legal_Reasons,
    _500_Internal_Server_Error,
    _501_Not_Implemented,
    _502_Bad_Gateway,
    _503_Service_Unavailable,
    _504_Gateway_Timeout,
    _505_HTTP_Version_Not_Supported,
    _506_Variant_Also_Negotiates,
    _507_Insufficient_Storage,
    _508_Loop_Detected,
    _510_Not_Extended,
    _511_Network_Authentication_Required
};

enum class ContentType {
    Html,
    Xhtml,
    PlainText,
    Json
};

enum class RedirectType {
    MovedPermanently,
    PermanentRedirect,
    Found,
    SeeOther,
    TemporaryRedirect,
    MultipleChoice,
    NotModified
};

enum class RequestMethod {
    Get,
    Head,
    Post,
    Put,
    Delete,
    Connect,
    Options,
    Trace,
    Patch
};

namespace detail {
[[noreturn]] inline void ensureNotReachable() noexcept
{
    std::terminate();
}
} //namespace detail

constexpr RequestMethod methodFromString(std::string_view typeStr)
{
    if (typeStr == "GET")
        return RequestMethod::Get;
    else if (typeStr == "HEAD")
        return RequestMethod::Head;
    else if (typeStr == "POST")
        return RequestMethod::Post;
    else if (typeStr == "PUT")
        return RequestMethod::Put;
    else if (typeStr == "DELETE")
        return RequestMethod::Delete;
    else if (typeStr == "CONNECT")
        return RequestMethod::Connect;
    else if (typeStr == "OPTIONS")
        return RequestMethod::Options;
    else if (typeStr == "TRACE")
        return RequestMethod::Trace;
    else if (typeStr == "PATCH")
        return RequestMethod::Patch;
    else
        return {};
}

constexpr const char* methodToString(RequestMethod method)
{
    switch (method) {
    case RequestMethod::Get:
        return "GET";
    case RequestMethod::Head:
        return "HEAD";
    case RequestMethod::Post:
        return "POST";
    case RequestMethod::Put:
        return "PUT";
    case RequestMethod::Delete:
        return "DELETE";
    case RequestMethod::Connect:
        return "CONNECT";
    case RequestMethod::Options:
        return "OPTIONS";
    case RequestMethod::Trace:
        return "TRACE";
    case RequestMethod::Patch:
        return "PATCH";
    }
    detail::ensureNotReachable();
}

enum class FormFieldType {
    Param,
    File
};

enum class FormType {
    UrlEncoded,
    Multipart
};

enum class HeaderQuotingMode {
    None,
    HeaderValue,
    ParamValue,
    AllValues
};

namespace detail {
constexpr ResponseStatus redirectTypeStatus(RedirectType redirectType)
{
    using Type = RedirectType;
    using Status = ResponseStatus;
    switch (redirectType) {
    case Type::MovedPermanently:
        return Status::_301_Moved_Permanently;
    case Type::PermanentRedirect:
        return Status::_308_Permanent_Redirect;
    case Type::Found:
        return Status::_302_Found;
    case Type::SeeOther:
        return Status::_303_See_Other;
    case Type::TemporaryRedirect:
        return Status::_307_Temporary_Redirect;
    case Type::MultipleChoice:
        return Status::_300_Multiple_Choice;
    case Type::NotModified:
        return Status::_304_Not_Modified;
    }
    detail::ensureNotReachable();
}

constexpr const char* statusToString(ResponseStatus status)
{
    using Status = ResponseStatus;
    switch (status) {
    case Status::_100_Continue:
        return "100 Continue";
    case Status::_101_Switching_Protocol:
        return "101 Switching Protocol";
    case Status::_102_Processing:
        return "102 Processing";
    case Status::_103_Early_Hints:
        return "103 Early Hints";
    case Status::_200_Ok:
        return "200 OK";
    case Status::_201_Created:
        return "201 Created";
    case Status::_202_Accepted:
        return "202 Accepted";
    case Status::_203_Non_Authoritative_Information:
        return "203 Non-Authoritative Information";
    case Status::_204_No_Content:
        return "204 No Content";
    case Status::_205_Reset_Content:
        return "205 Reset Content";
    case Status::_206_Partial_Content:
        return "206 Partial Content";
    case Status::_207_Multi_Status:
        return "207 Multi-Status";
    case Status::_208_Already_Reported:
        return "208 Already Reported";
    case Status::_226_IM_Used:
        return "226 IM Used";
    case Status::_300_Multiple_Choice:
        return "300 Multiple Choice";
    case Status::_301_Moved_Permanently:
        return "301 Moved Permanently";
    case Status::_302_Found:
        return "302 Found";
    case Status::_303_See_Other:
        return "303 See Other";
    case Status::_304_Not_Modified:
        return "304 Not Modified";
    case Status::_307_Temporary_Redirect:
        return "307 Temporary Redirect";
    case Status::_308_Permanent_Redirect:
        return "308 Permanent Redirect";
    case Status::_400_Bad_Request:
        return "400 Bad Request";
    case Status::_401_Unauthorized:
        return "401 Unauthorized";
    case Status::_402_Payment_Required:
        return "402 Payment Required";
    case Status::_403_Forbidden:
        return "403 Forbidden";
    case Status::_404_Not_Found:
        return "404 Not Found";
    case Status::_405_Method_Not_Allowed:
        return "405 Method Not Allowed";
    case Status::_406_Not_Acceptable:
        return "406 Not Acceptable";
    case Status::_407_Proxy_Authentication_Required:
        return "407 Proxy Authentication Required";
    case Status::_408_Request_Timeout:
        return "408 Request Timeout";
    case Status::_409_Conflict:
        return "409 Conflict";
    case Status::_410_Gone:
        return "410 Gone";
    case Status::_411_Length_Required:
        return "411 Length Required";
    case Status::_412_Precondition_Failed:
        return "412 Precondition Failed";
    case Status::_413_Payload_Too_Large:
        return "413 Payload Too Large";
    case Status::_414_URI_Too_Long:
        return "414 URI Too Long";
    case Status::_415_Unsupported_Media_Type:
        return "415 Unsupported Media Type";
    case Status::_416_Range_Not_Satisfiable:
        return "416 Range Not Satisfiable";
    case Status::_417_Expectation_Failed:
        return "417 Expectation Failed";
    case Status::_418_Im_a_teapot:
        return "418 I'm a teapot";
    case Status::_421_Misdirected_Request:
        return "421 Misdirected Request";
    case Status::_422_Unprocessable_Entity:
        return "422 Unprocessable Entity";
    case Status::_423_Locked:
        return "423 Locked";
    case Status::_424_Failed_Dependency:
        return "424 Failed Dependency";
    case Status::_425_Too_Early:
        return "425 Too Early";
    case Status::_426_Upgrade_Required:
        return "426 Upgrade Required";
    case Status::_428_Precondition_Required:
        return "428 Precondition Required";
    case Status::_429_Too_Many_Requests:
        return "429 Too Many Requests";
    case Status::_431_Request_Header_Fields_Too_Large:
        return "431 Request Header Fields Too Large";
    case Status::_451_Unavailable_For_Legal_Reasons:
        return "451 Unavailable For Legal Reasons";
    case Status::_500_Internal_Server_Error:
        return "500 Internal Server Error";
    case Status::_501_Not_Implemented:
        return "501 Not Implemented";
    case Status::_502_Bad_Gateway:
        return "502 Bad Gateway";
    case Status::_503_Service_Unavailable:
        return "503 Service Unavailable";
    case Status::_504_Gateway_Timeout:
        return "504 Gateway Timeout";
    case Status::_505_HTTP_Version_Not_Supported:
        return "505 HTTP Version Not Supported";
    case Status::_506_Variant_Also_Negotiates:
        return "506 Variant Also Negotiates";
    case Status::_507_Insufficient_Storage:
        return "507 Insufficient Storage";
    case Status::_508_Loop_Detected:
        return "508 Loop Detected";
    case Status::_510_Not_Extended:
        return "510 Not Extended";
    case Status::_511_Network_Authentication_Required:
        return "511 Network Authentication Required";
    }
    detail::ensureNotReachable();
}

constexpr ResponseStatus statusFromCode(int statusCode)
{
    using Status = ResponseStatus;
    switch (statusCode) {
    case 100:
        return Status::_100_Continue;
    case 101:
        return Status::_101_Switching_Protocol;
    case 102:
        return Status::_102_Processing;
    case 103:
        return Status::_103_Early_Hints;
    case 200:
        return Status::_200_Ok;
    case 201:
        return Status::_201_Created;
    case 202:
        return Status::_202_Accepted;
    case 203:
        return Status::_203_Non_Authoritative_Information;
    case 204:
        return Status::_204_No_Content;
    case 205:
        return Status::_205_Reset_Content;
    case 206:
        return Status::_206_Partial_Content;
    case 207:
        return Status::_207_Multi_Status;
    case 208:
        return Status::_208_Already_Reported;
    case 226:
        return Status::_226_IM_Used;
    case 300:
        return Status::_300_Multiple_Choice;
    case 301:
        return Status::_301_Moved_Permanently;
    case 302:
        return Status::_302_Found;
    case 303:
        return Status::_303_See_Other;
    case 304:
        return Status::_304_Not_Modified;
    case 307:
        return Status::_307_Temporary_Redirect;
    case 308:
        return Status::_308_Permanent_Redirect;
    case 400:
        return Status::_400_Bad_Request;
    case 401:
        return Status::_401_Unauthorized;
    case 402:
        return Status::_402_Payment_Required;
    case 403:
        return Status::_403_Forbidden;
    case 404:
        return Status::_404_Not_Found;
    case 405:
        return Status::_405_Method_Not_Allowed;
    case 406:
        return Status::_406_Not_Acceptable;
    case 407:
        return Status::_407_Proxy_Authentication_Required;
    case 408:
        return Status::_408_Request_Timeout;
    case 409:
        return Status::_409_Conflict;
    case 410:
        return Status::_410_Gone;
    case 411:
        return Status::_411_Length_Required;
    case 412:
        return Status::_412_Precondition_Failed;
    case 413:
        return Status::_413_Payload_Too_Large;
    case 414:
        return Status::_414_URI_Too_Long;
    case 415:
        return Status::_415_Unsupported_Media_Type;
    case 416:
        return Status::_416_Range_Not_Satisfiable;
    case 417:
        return Status::_417_Expectation_Failed;
    case 418:
        return Status::_418_Im_a_teapot;
    case 421:
        return Status::_421_Misdirected_Request;
    case 422:
        return Status::_422_Unprocessable_Entity;
    case 423:
        return Status::_423_Locked;
    case 424:
        return Status::_424_Failed_Dependency;
    case 425:
        return Status::_425_Too_Early;
    case 426:
        return Status::_426_Upgrade_Required;
    case 428:
        return Status::_428_Precondition_Required;
    case 429:
        return Status::_429_Too_Many_Requests;
    case 431:
        return Status::_431_Request_Header_Fields_Too_Large;
    case 451:
        return Status::_451_Unavailable_For_Legal_Reasons;
    case 500:
        return Status::_500_Internal_Server_Error;
    case 501:
        return Status::_501_Not_Implemented;
    case 502:
        return Status::_502_Bad_Gateway;
    case 503:
        return Status::_503_Service_Unavailable;
    case 504:
        return Status::_504_Gateway_Timeout;
    case 505:
        return Status::_505_HTTP_Version_Not_Supported;
    case 506:
        return Status::_506_Variant_Also_Negotiates;
    case 507:
        return Status::_507_Insufficient_Storage;
    case 508:
        return Status::_508_Loop_Detected;
    case 510:
        return Status::_510_Not_Extended;
    case 511:
        return Status::_511_Network_Authentication_Required;
    default:
        return {};
    }
}

constexpr const char* contentTypeToString(ContentType type)
{
    switch (type) {
    case ContentType::Html:
        return "text/html";
    case ContentType::Xhtml:
        return "application/xhtml+xml";
    case ContentType::PlainText:
        return "text/plain";
    case ContentType::Json:
        return "application/json";
    }
    detail::ensureNotReachable();
}

} //namespace detail

} //namespace http

#endif //HOT_TEACUP_TYPES_H
