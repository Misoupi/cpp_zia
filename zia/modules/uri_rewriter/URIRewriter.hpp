//
// Created by doom on 18/02/18.
//

#ifndef ZIA_URI_REWRITER_HPP
#define ZIA_URI_REWRITER_HPP

#include <map>
#include <api/module.h>

namespace zia::modules
{
    class URIRewriter : public api::Module
    {
    public:
        ~URIRewriter() override = default;
        bool config(const api::Conf &conf) override;
        bool exec(api::HttpDuplex &http) override;

    private:
        std::string _domain;
        std::unordered_map<std::string, std::string> _subdomains;
    };
}

#endif //ZIA_URI_REWRITER_HPP
