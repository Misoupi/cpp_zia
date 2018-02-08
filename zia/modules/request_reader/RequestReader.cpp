//
// Created by doom on 08/02/18.
//

#include <sstream>
#include <core/lib/Alias.hpp>
#include <utils/BufferUtils.hpp>
#include <iostream>
#include "RequestReader.hpp"

namespace zia::modules
{
    bool RequestReader::config(const zia::api::Conf &conf)
    {
        return true;
    }

    static api::http::Version stringToVersion(const std::string_view sv) noexcept
    {
        static std::unordered_map<std::string_view, api::http::Version> conversions{
            {"0.9", api::http::Version::http_0_9},
            {"1.0", api::http::Version::http_1_0},
            {"1.1", api::http::Version::http_1_1},
            {"2.0", api::http::Version::http_2_0},
        };
        return conversions[sv];
    }

    static api::http::Method stringToMethod(const std::string_view sv) noexcept
    {
        static std::unordered_map<std::string_view, api::http::Method> conversions{
            {"OPTIONS", api::http::Method::options},
            {"GET", api::http::Method::get},
            {"HEAD", api::http::Method::head},
            {"POST", api::http::Method::post},
            {"PUT", api::http::Method::put},
            {"DELETE", api::http::Method::delete_},
            {"TRACE", api::http::Method::trace},
            {"CONNECT", api::http::Method::connect},
        };
        return conversions[sv];
    }

#define fail_if(x)                                                            \
    if (x) {                                                                  \
        return false;                                                         \
    }

    static bool _extractReqLine(api::HttpRequest &out, const std::string &line)
    {
        std::istringstream ss(line);
        std::string method;

        ss >> method;
        fail_if(ss.fail());
        out.method = stringToMethod(method);
        fail_if(out.method == api::http::Method::unknown);

        ss >> out.uri;
        fail_if(ss.fail());

        std::string version;
        ss >> version;
        fail_if(ss.fail() || std::string_view(version.data(), 5) != "HTTP/");
        out.version = stringToVersion(std::string_view(version.data() + 5, version.length() - 5));
        fail_if(out.version == api::http::Version::unknown);
        return true;
    }

    bool RequestReader::exec(zia::api::HttpDuplex &http)
    {
        std::istringstream iss(utils::rawToString(http.raw_req));
        std::vector<std::string> headerLines;
        std::string s;

        while (std::getline(iss, s)) {
            if (!s.empty() && s.back() == '\r')
                s = s.substr(0, s.length() - 1);
            if (s.empty())
                break;
            headerLines.push_back(std::move(s));
        }

        fail_if(iss.fail());
        fail_if(headerLines.empty() || !_extractReqLine(http.req, headerLines.front()));

        for (auto it = headerLines.begin() + 1; it != headerLines.end(); ++it) {
            auto sep = it->find(':');
            fail_if(!sep || sep == std::string::npos);
            std::string key = it->substr(0, sep);
            std::string value = it->substr(it->find_first_not_of(' ', sep + 1));
            http.req.headers.emplace(key, value);
        }
        return true;
    }

#undef fail_if

    api::Module *create() noexcept
    {
        return new RequestReader;
    }
}

lib_alias_function(zia::modules::create, create);
