//
// Created by riz on 11/02/18.
//

#include <iostream>
#include <iterator>
#include <sstream>
#include <core/lib/Alias.hpp>
#include <utils/BufferUtils.hpp>
#include "ErrorPageMaker.hpp"

namespace zia::modules
{
    bool ErrorPageMaker::config([[maybe_unused]] const api::Conf &conf)
    {
        return true;
    }

    std::string ErrorPageMaker::_statusToString(api::http::Status status) noexcept
    {
        auto it = statusConverter.find(status);
        return it != statusConverter.end() ? it->second : "Unknown";
    }

    bool ErrorPageMaker::exec(api::HttpDuplex &http)
    {
        if (http.resp.status != 0 && !nonErrorStatus.count(http.resp.status)) {
                std::string statusMsg = _statusToString(http.resp.status);
                std::ostringstream body;
                body << "<html>\r\n"
                     << "  <head>\r\n"
                     << "    <title>Error " << http.resp.status << " : " << statusMsg << "</title>\r\n"
                     << "  </head>\r\n"
                     << "  <body>\r\n"
                     << "    <h1>Error " << http.resp.status << " : " << statusMsg << "</h1>\r\n"
                     << "    <p>" << http.resp.reason << "</p>\r\n"
                     << "  </body>\r\n"
                     << "</html>";
                http.resp.body = utils::stringToRaw(body.str());

                http.resp.headers["Content-Type"] = "text/html";
                http.resp.headers["Content-Length"] = std::to_string(http.resp.body.size());
        }
        return true;
    }

    api::Module *create() noexcept
    {
        return new ErrorPageMaker;
    }
}

lib_alias_function(zia::modules::create, create);