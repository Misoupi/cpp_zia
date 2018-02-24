//
// Created by doom on 18/02/18.
//

#include <core/lib/Alias.hpp>
#include "URIRewriter.hpp"

namespace zia::modules
{
    bool URIRewriter::config(const api::Conf &conf)
    {
        auto it = conf.find("subdomains");
        auto it2 = conf.find("domain");
        if (it == conf.end() || it2 == conf.end())
            return true;
        if (!std::holds_alternative<api::ConfObject>(it->second.v)
            || !std::holds_alternative<std::string>(it2->second.v))
            return false;

        const auto &subdomains = std::get<api::ConfObject>(it->second.v);
        const auto &domain = std::get<std::string>(it2->second.v);
        _subdomains.reserve(subdomains.size());
        for (const auto &pair : subdomains) {
            if (!std::holds_alternative<std::string>(pair.second.v))
                return false;
            _subdomains.emplace(pair.first + "." + domain, std::get<std::string>(pair.second.v));
        }
        _domain = domain;
        return true;
    }

    bool URIRewriter::exec(api::HttpDuplex &http)
    {
        auto &header = http.req.headers;
        auto domainIt = header.find("Host");

        if (domainIt != header.end()) {
            auto subdomainIt = _subdomains.find(domainIt->second);
            if (subdomainIt != _subdomains.end()) {
                if (http.req.headers["Host"].compare(0, 8, "https://") == 0)
                    http.resp.headers["Location"] = "https://" + _domain + subdomainIt->second + http.req.uri;
                else
                    http.resp.headers["Location"] = "http://" + _domain + subdomainIt->second + http.req.uri;
                http.resp.headers["Content-Length"] = "0";
                http.resp.status = api::http::common_status::moved_permanently;
                http.resp.reason = "Redirecting subdomain";
            }
        }
        return true;
    }

    api::Module *create() noexcept
    {
        return new URIRewriter;
    }
}

lib_alias_function(zia::modules::create, create);
