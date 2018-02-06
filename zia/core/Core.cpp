//
// Created by doom on 05/02/18.
//

#include "Core.hpp"

bool zia::Core::loadConfiguration() noexcept
{
    ConfigLoader loader;

    if (!loader.loadFromFile(_configFile))
        return false;
    _conf = loader.extractConfig();
    return true;
}

bool zia::Core::loadModules() noexcept
{
    fs::path modulesPath = std::get<std::string>(_conf["modules_path"].v);
    _log(logging::Info) << "Loading modules from directory " << modulesPath << "..." << std::endl;

    for (const auto &curValue : std::get<zia::api::ConfArray>(_conf["modules"].v)) {
        const auto &curFile = std::get<std::string>(curValue.v);
        _log(logging::Debug) << "Loading " << curFile << std::endl;

        try {
            auto symbol = lib::getSymbol<ModuleCreator>(modulesPath / curFile, "create");
            _creators.push_back(symbol);
        } catch (const std::exception &e) {
            _log(logging::Error) << "Unable to load " << curFile << ": " << e.what() << std::endl;
            return false;
        }
    }
    _log(logging::Info) << "Successfully loaded " << _creators.size() << " modules" << std::endl;
    return true;
}

bool zia::Core::configureModules() noexcept
{
    for (const auto &curCreator : _creators) {
        _modules.emplace_back((*curCreator)());
        if (!_modules.back()->config(_conf))
            return false;
    }
    return true;
}
