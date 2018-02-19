//
// Created by milerius on 07/02/18.
//

#include <zia/modules/static_file/StaticFile.hpp>
#include <core/lib/Alias.hpp>
#include <fstream>
#include <iterator>
#include <iostream>

namespace zia::modules
{
    bool StaticFile::config([[maybe_unused]] const api::Conf &conf)
    {
        auto it = conf.find("root_directory");
        if (it != conf.end()) {
            if (!std::holds_alternative<std::string>(it->second.v))
                return false;
            _root = std::get<std::string>(it->second.v);
        } else
            _root = "";
        return true;
    }

    bool StaticFile::exec(api::HttpDuplex &http)
    {
        if (http.req.method == zia::api::http::Method::get) {
            fs::path realPath = _root / fs::path(http.req.uri);

            if (!fs::exists(realPath)) {
                http.resp.status = zia::api::http::common_status::not_found;
                http.resp.headers.emplace("Content-Length", "0");
                return true;
            }
            if (_map.find(realPath.extension().string()) == _map.end()) {
                http.resp.headers["Content-Type"] = _map[""];
            } else
                http.resp.headers["Content-Type"] = _map[realPath.extension().string()];
            http.resp.status = zia::api::http::common_status::ok;

            std::ifstream ifs(realPath);
            std::transform(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(),
                           std::back_inserter(http.resp.body), [](auto c) {
                    return static_cast<std::byte>(c);
                });
            http.resp.headers["Content-Length"] = std::to_string(http.resp.body.size());
        }
        return true;
    }

    zia::api::Module *create() noexcept
    {
        return new zia::modules::StaticFile;
    }
}

lib_alias_function(zia::modules::create, create);