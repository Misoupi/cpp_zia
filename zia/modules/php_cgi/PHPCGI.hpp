//
// Created by Blowa
//

#ifndef ZIA_PHP_CGI_HPP
#define ZIA_PHP_CGI_HPP

#include <boost/process.hpp>
#include <api/module.h>

namespace zia::modules
{
    const static std::string cgiExeKey = "php_cgi_exe";
    const static std::string rootDirectoryKey = "root_directory";
    const static std::string cgiEnvKey = "CGI_ENV";

    class PHPCGI : public zia::api::Module
    {
    public:
        ~PHPCGI() override = default;
        bool config(const api::Conf &conf) override;
        bool exec(api::HttpDuplex &http) override;

    private:
        struct Result
        {
            std::string stdoutData;
            std::string stderrData;
            int exitStatus;
        };

        Result _runCGI(const std::string &pathToFile) noexcept;

        std::string _cgiPath;
        std::string _filePath;
    };
}

#endif //ZIA_PHP_CGI_HPP
