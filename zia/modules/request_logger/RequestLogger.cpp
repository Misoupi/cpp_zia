//
// Created by doom on 17/02/18.
//

#include <iterator>
#include <core/lib/Alias.hpp>
#include "RequestLogger.hpp"

namespace zia::modules
{
    bool RequestLogger::config(const zia::api::Conf &conf)
    {
        auto it = conf.find("requests_log_file");
        std::string path;

        if (it != conf.end() && std::holds_alternative<std::string>(it->second.v)) {
            path = std::get<std::string>(it->second.v);
        } else {
            path = "requests.log";
        }
        _flog.open(path);
        return _flog.isOpen();
    }

    bool RequestLogger::exec(zia::api::HttpDuplex &http)
    {
        auto n = std::chrono::system_clock::now().time_since_epoch();
        auto stamp = std::chrono::duration_cast<std::chrono::milliseconds>(n).count();

        auto handle = _flog(logging::Debug);
        handle << "-----BEGIN REQUEST: " << stamp << "-----" << std::endl;
        std::for_each(http.raw_req.begin(), http.raw_req.end(), [&handle](auto c) {
            handle << static_cast<char>(c);
        });
        handle << std::endl;
        handle << "-----END REQUEST-----" << std::endl;
        return true;
    }

    api::Module *create() noexcept
    {
        return new RequestLogger;
    }
}

lib_alias_function(zia::modules::create, create);