//
// Created by doom on 05/02/18.
//

#include <core/log/Logger.hpp>
#include "Core.hpp"

int main(int ac, char **av)
{
    if (ac == 2) {
        zia::Core core{fs::path(av[1])};
        logging::Logger logger("zia", logging::Debug);

        if (!core.loadConfiguration()) {
            logger(logging::Error) << "Unable to load configuration" << std::endl;
            return 1;
        }

        if (!core.loadModules() || !core.configureModules()) {
            logger(logging::Error) << "Unable to load and configure the specified modules" << std::endl;
            return 1;
        }
        core.run();
    }
    return 0;
}
