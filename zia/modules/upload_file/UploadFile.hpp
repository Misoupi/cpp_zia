//
// Created by riz on 11/02/18.
//

#ifndef ZIA_UPLOADFILE_HPP
#define ZIA_UPLOADFILE_HPP

#include <experimental/filesystem>
#include <api/module.h>

namespace fs = std::experimental::filesystem;

namespace zia::modules
{
    class UploadFile : public zia::api::Module
    {
    public:
        bool config(const api::Conf &conf) override;
        bool exec(api::HttpDuplex &http) override;
    };
}

#endif //ZIA_UPLOADFILE_HPP
