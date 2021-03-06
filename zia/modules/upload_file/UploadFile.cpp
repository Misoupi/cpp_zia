//
// Created by riz on 11/02/18.
//

#include <zia/modules/upload_file/UploadFile.hpp>
#include <core/lib/Alias.hpp>
#include <iostream>
#include <iterator>
#include <fstream>

namespace zia::modules
{
    bool UploadFile::config([[maybe_unused]] const api::Conf &conf)
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

    bool UploadFile::exec(api::HttpDuplex &http)
    {
        if (http.req.method == zia::api::http::Method::put) {
            fs::path uploadPath = _root / http.req.uri;
            http.resp.headers.emplace("Content-Length", "0");
            http.resp.status = zia::api::http::common_status::ok;
            if (!fs::exists(uploadPath))
                http.resp.status = zia::api::http::common_status::created;

            std::ofstream ofs(uploadPath.string(), std::ios_base::trunc);
            if (ofs.fail()) {
                http.resp.status = zia::api::http::common_status::unauthorized;
                return true;
            }
            std::transform(http.req.body.begin(), http.req.body.end(), std::ostream_iterator<char>(ofs), [](auto byte) {
                return static_cast<char>(byte);
            });
            ofs.flush();
        }
        return true;
    }

    zia::api::Module *create() noexcept
    {
        return new zia::modules::UploadFile;
    }
}

lib_alias_function(zia::modules::create, create);