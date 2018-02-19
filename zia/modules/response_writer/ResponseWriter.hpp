//
// Created by doom on 08/02/18.
//

#ifndef ZIA_REQUESTWRITER_HPP
#define ZIA_REQUESTWRITER_HPP

#include <api/module.h>

namespace zia::modules
{
    class ResponseWriter : public api::Module
    {
    public:
        ~ResponseWriter() override = default;
        bool config(const api::Conf &conf) override;
        bool exec(api::HttpDuplex &http) override;
    };
}

#endif //ZIA_REQUESTWRITER_HPP
