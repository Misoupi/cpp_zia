//
// Created by riz on 11/02/18.
//

#ifndef ZIA_ERRORPAGEMAKER_HPP
#define ZIA_ERRORPAGEMAKER_HPP

#include <set>
#include <experimental/filesystem>
#include <api/module.h>

namespace fs = std::experimental::filesystem;

namespace zia::modules
{
    class ErrorPageMaker : public zia::api::Module
    {
    public:
        bool config(const api::Conf &conf) override;
        bool exec(api::HttpDuplex &http) override;

    private:
        std::string _statusToString(api::http::Status) noexcept;

        const std::map<int, std::string> statusConverter{
                {100, "Continue"},
                {101, "Switching Protocols"},
                {200, "Ok"},
                {201, "Created"},
                {202, "Accepted"},
                {203, "Nonauthoritative Information"},
                {204, "No Content"},
                {205, "Reset Content"},
                {206, "Partial Content"},
                {300, "Multiple Choice"},
                {301, "Moved Permanently"},
                {302, "Found"},
                {303, "See Other"},
                {304, "Not Modified"},
                {305, "Use Proxy"},
                {307, "Temporary Redirect"},
                {400, "Bad Request"},
                {401, "Unauthorized"},
                {402, "Payment Required"},
                {403, "Forbidden"},
                {404, "Not Found"},
                {405, "Method Not Allowed"},
                {406, "Not Acceptable"},
                {407, "Proxy Authentification Required"},
                {408, "Request Timeout"},
                {409, "Conflict"},
                {410, "Gone"},
                {411, "Length Required"},
                {412, "Precondition Failed"},
                {413, "Request Entity Too Large"},
                {414, "Request Uri Too Large"},
                {415, "Unsupported Media Type"},
                {416, "Request Range Not Satisfiables"},
                {417, "Expectation Failed"},
                {418, "I'm A Teapot"},
                {500, "Internal Server Error"},
                {501, "Not Implemented"},
                {502, "Bad Gateway"},
                {503, "Service Unavailable"},
                {504, "Gateway Timeout"},
                {505, "Http Version Not Supported"},
        };
        const std::set<int> nonErrorStatus{100, 101, 200, 201, 202, 203, 205, 206, 300, 301,
                                           302, 303, 304, 305, 307, 401, 402, 407, 410, 411};
    };
}

#endif //ZIA_ERRORPAGEMAKER_HPP
