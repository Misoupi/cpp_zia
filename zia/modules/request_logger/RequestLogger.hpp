//
// Created by doom on 17/02/18.
//

#ifndef ZIA_REQUESTLOGGER_HPP
#define ZIA_REQUESTLOGGER_HPP

#include <fstream>
#include <core/log/Logger.hpp>
#include <api/module.h>

namespace logging
{
    class FileLogger : public LoggerBase<FileLogger>
    {
    public:
        explicit FileLogger(const std::string &name, Level lvl) noexcept : LoggerBase(lvl), _name(name)
        {
        }

        void open(const std::string &name) noexcept
        {
            _ofs.open(name);
        }

        bool isOpen() const noexcept
        {
            return _ofs.is_open();
        }

        void log(const std::string &str) noexcept
        {
            std::scoped_lock<std::mutex> lock{FileLogger::_mutex()};
            _ofs << str;
        }

        const std::string &name() const noexcept
        {
            return _name;
        }

        Handle operator()(Level lvl) noexcept
        {
            return Handle(*this, lvl);
        }

    protected:
        //MSVC lacks support for inline variables
        static std::mutex &_mutex() noexcept
        {
            static std::mutex mutex;
            return mutex;
        }

        std::ofstream _ofs;
        const std::string _name;
    };
}

namespace zia::modules
{
    class RequestLogger : public api::Module
    {
    public:
        ~RequestLogger() override = default;
        bool config(const api::Conf &conf) override;
        bool exec(api::HttpDuplex &http) override;

    private:
        logging::FileLogger _flog{"requests_log", logging::Debug};
    };
}

#endif //ZIA_REQUESTLOGGER_HPP
