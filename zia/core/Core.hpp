//
// Created by doom on 05/02/18.
//

#ifndef ZIA_CORE_HPP
#define ZIA_CORE_HPP

#include <vector>
#include <memory>
#include <core/log/Logger.hpp>
#include <core/lib/Lib.hpp>
#include <api/module.h>
#include "ConfigLoader.hpp"

namespace zia
{
    class Core
    {
    public:
        explicit Core(fs::path configFile) noexcept : _configFile(std::move(configFile))
        {
        }

        bool loadConfiguration() noexcept;

        bool loadModules() noexcept;

        bool configureModules() noexcept;

        bool run() noexcept
        {
            _log(logging::Info) << "Starting the Zia..." << std::endl;
            return true;
        }

    private:
        fs::path _configFile;
        zia::api::Conf _conf;
        logging::Logger _log{"zia", logging::Debug};
        using ModuleCreator = zia::api::Module *(*)();
        std::vector<lib::Symbol<ModuleCreator>> _creators;
        std::vector<std::unique_ptr<zia::api::Module>> _modules;
    };
}

#endif //ZIA_CORE_HPP
