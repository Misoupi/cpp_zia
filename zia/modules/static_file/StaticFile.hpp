//
// Created by milerius on 07/02/18.
//

#ifndef ZIA_STATICFILE_HPP
#define ZIA_STATICFILE_HPP

#include <experimental/filesystem>
#include <api/module.h>

namespace fs = std::experimental::filesystem;

namespace zia::module
{
    class StaticFile : public zia::api::Module
    {
    public:
        StaticFile() noexcept = default;
        virtual ~StaticFile() = default;
        bool config(const api::Conf &conf) override;
        bool exec(api::HttpDuplex &http) override;
    };
}

#endif //ZIA_STATICFILE_HPP
