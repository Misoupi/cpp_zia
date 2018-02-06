//
// Created by doom on 06/02/18.
//

#ifndef ZIA_CONFIGLOADER_HPP
#define ZIA_CONFIGLOADER_HPP

#include <experimental/filesystem>
#include <core/config/CompilerConfig.hpp>
#include <api/module.h>

namespace fs = std::experimental::filesystem;

namespace zia
{
    class ConfigLoader
    {
    public:
        bool loadFromFile(const fs::path &path) noexcept;
        zia::api::Conf extractConfig() noexcept;

    private:
        zia::api::Conf _conf;
    };
}

#endif //ZIA_CONFIGLOADER_HPP
