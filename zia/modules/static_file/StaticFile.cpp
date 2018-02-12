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
        return true;
    }

    bool StaticFile::exec(api::HttpDuplex &http)
    {
        if (http.req.method == zia::api::http::Method::get) {
            if (!fs::exists(http.req.uri)) {
                http.resp.status = zia::api::http::common_status::not_found;
                http.resp.headers.emplace("Content-Length", "0");
                return true;
            }
            if (_map.find(fs::path(http.req.uri).extension().string()) == _map.end()) {
                http.resp.headers["Content-Type"] = _map[""];
            } else
                http.resp.headers["Content-Type"] = _map[fs::path(http.req.uri).extension().string()];
            http.resp.status = zia::api::http::common_status::ok;
            std::ifstream ifs(http.req.uri);
            std::vector<char> v{std::istreambuf_iterator<char>(ifs),
                                std::istreambuf_iterator<char>()};
            for (auto cur : v) {
                http.resp.body.push_back(std::byte(cur));
            }
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