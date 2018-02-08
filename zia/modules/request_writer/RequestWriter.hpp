//
// Created by doom on 08/02/18.
//

#ifndef ZIA_REQUESTWRITER_HPP
#define ZIA_REQUESTWRITER_HPP

#include <api/module.h>

namespace zia::modules
{
    class RequestWriter : public api::Module
    {
    public:
        ~RequestWriter() override = default;
        bool config(const api::Conf &conf) override;
        bool exec(api::HttpDuplex &http) override;
    };
}

#endif //ZIA_REQUESTWRITER_HPP
