//
// Created by Blowa
//

#ifndef ZIA_PHP_CGI_HPP
#define ZIA_PHP_CGI_HPP

#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <api/module.h>

namespace fs = boost::filesystem;

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

        struct CGIInvocation
        {
            fs::path scriptPath;
            std::string params;
            std::string method;
        };

        boost::process::native_environment _setupEnv(const CGIInvocation &invocation) noexcept;

        Result _runCGI(const CGIInvocation &invocation) noexcept;

        using ErrorCode = int;
        using InvocationOrErrCode = std::variant<CGIInvocation, ErrorCode>;
        InvocationOrErrCode _prepareInvocation(const api::HttpRequest &req) noexcept;

        static constexpr const PHPCGI::ErrorCode unhandled = 0;

        std::string _cgiPath;
        std::string _filePath;
    };
}

#endif //ZIA_PHP_CGI_HPP
