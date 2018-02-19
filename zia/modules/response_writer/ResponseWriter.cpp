//
// Created by doom on 08/02/18.
//

#include <sstream>
#include <core/lib/Alias.hpp>
#include <utils/BufferUtils.hpp>
#include "ResponseWriter.hpp"

namespace zia::modules
{
    bool ResponseWriter::config([[maybe_unused]] const zia::api::Conf &conf)
    {
        return true;
    }

    static std::string versionToString(api::http::Version version) noexcept
    {
        static std::unordered_map<api::http::Version, std::string> conversions{
            {api::http::Version::unknown,  "Unknown"},
            {api::http::Version::http_0_9, "HTTP/0.9"},
            {api::http::Version::http_1_0, "HTTP/1.0"},
            {api::http::Version::http_1_1, "HTTP/1.1"},
            {api::http::Version::http_2_0, "HTTP/2.0"},
        };

        return conversions[version];
    }

    bool ResponseWriter::exec(zia::api::HttpDuplex &http)
    {
        std::ostringstream oss;
        api::HttpResponse &resp = http.resp;
        http.resp.version = api::http::Version::http_1_1;
        api::Net::Raw &data = http.raw_resp;

        oss << versionToString(resp.version) << " ";
        oss << std::to_string(resp.status) << " ";
        oss << resp.reason << "\r" << std::endl;

        for (const auto &cur : resp.headers) {
            oss << cur.first << ": " << cur.second << "\r" << std::endl;
        }
        oss << "\r" << std::endl;

        data = utils::stringToRaw(oss.str());
        std::copy(resp.body.begin(), resp.body.end(), std::back_inserter(data));
        return true;
    }

    api::Module *create() noexcept
    {
        return new ResponseWriter;
    }
}

lib_alias_function(zia::modules::create, create);
