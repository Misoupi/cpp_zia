//
// Created by doom on 08/02/18.
//

#ifndef ZIA_REQUESTREADER_HPP
#define ZIA_REQUESTREADER_HPP

#include <api/module.h>

namespace zia::modules
{
    class RequestReader : public zia::api::Module
    {
    public:
        ~RequestReader() override = default;
        bool config(const api::Conf &conf) override;
        bool exec(api::HttpDuplex &http) override;
    };
}

#endif //ZIA_REQUESTREADER_HPP
