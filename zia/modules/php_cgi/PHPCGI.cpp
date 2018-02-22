//
// Created by Blowa on 10/02/2018.
//

#include <core/lib/Alias.hpp>
#include <api/conf.h>
#include <utils/BufferUtils.hpp>
#include "PHPCGI.hpp"

namespace zia::modules
{
    bool PHPCGI::config(const api::Conf &conf)
    {
        auto cgiConf = conf.find(cgiExeKey);
        auto filePathConf = conf.find(rootDirectoryKey);
        if (cgiConf == conf.end() || filePathConf == conf.end())
            return false;

        try {
            _cgiPath = std::get<std::string>(cgiConf->second.v);
            _filePath = std::get<std::string>(filePathConf->second.v);
        } catch ([[maybe_unused]] const std::exception &e) {
            return false;
        }
        return true;
    }

    boost::process::native_environment PHPCGI::_setupEnv(const CGIInvocation &invocation) noexcept
    {
        auto env = boost::this_process::environment();

        env["SCRIPT_FILENAME"] = invocation.scriptPath.string();
        env["REQUEST_METHOD"] = invocation.method;
        if (invocation.method == "POST") {
            env["CONTENT_LENGTH"] = std::to_string(invocation.params.length());
            env["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
        } else {
            env["QUERY_STRING"] = invocation.params;
        }

        env["REDIRECT_STATUS"] = "true";
        env["GATEWAY_INTERFACE"] = "CGI/1.1";

        return env;
    }

    PHPCGI::Result PHPCGI::_runCGI(const CGIInvocation &invocation) noexcept
    {
        Result ret;
        boost::process::ipstream outStream;
        boost::process::ipstream errStream;
        boost::process::opstream inStream;

        try {
            auto env = _setupEnv(invocation);

            if (invocation.method == "POST") {
                inStream << invocation.params << std::flush;
            }

            boost::process::child childProcess(_cgiPath, env,
                                               boost::process::std_in < inStream,
                                               boost::process::std_out > outStream,
                                               boost::process::std_err > errStream);
            if (!childProcess.wait_for(std::chrono::seconds(5)))
                childProcess.terminate();
            ret.exitStatus = childProcess.exit_code();
        } catch (const std::exception &e) {
            ret.exitStatus = 127;
            ret.stdoutData.clear();
            if (ret.stderrData.empty()) {
                ret.stderrData = std::string(e.what());
            }
            return ret;
        }
        std::string s;
        while (std::getline(outStream, s)) {
            if (s == "\r")
                break;
        }
        ret.stdoutData = std::string(std::istreambuf_iterator<char>{outStream}, {});
        ret.stderrData = std::string(std::istreambuf_iterator<char>{errStream}, {});
        return ret;
    }

    static bool shouldHandleRequest(const std::string_view uri, api::http::Method method)
    {
        static const std::string_view extension = ".php";
        static constexpr const api::http::Method acceptedMethods[] = {
            api::http::Method::get,
            api::http::Method::head,
            api::http::Method::post,
        };

        if (uri.length() <= 4u || !std::equal(extension.rbegin(), extension.rend(), uri.rbegin()))
            return false;
        if (std::count(std::begin(acceptedMethods), std::end(acceptedMethods), method) == 0)
            return false;
        return true;
    }

    PHPCGI::InvocationOrErrCode PHPCGI::_prepareInvocation(const api::HttpRequest &req) noexcept
    {
        CGIInvocation invocation;

        const auto &uri = req.uri;
        auto pos = uri.find('?');
        std::string_view pureUri{uri.data(), pos == std::string::npos ? uri.length() : pos};

        if (!shouldHandleRequest(pureUri, req.method))
            return unhandled;

        invocation.scriptPath = fs::path(_filePath) / std::string(pureUri);
        if (!fs::exists(invocation.scriptPath)) {
            return api::http::common_status::not_found;
        }

        if (req.method == api::http::Method::get || req.method == api::http::Method::head) {
            invocation.method = req.method == api::http::Method::get ? "GET" : "HEAD";
            if (pos != std::string::npos) {
                invocation.params = {uri.begin() + pos + 1, uri.end()};
            }
        } else {
            invocation.method = "POST";
            invocation.params = utils::rawToString(req.body);
        }

        return invocation;
    }

    bool PHPCGI::exec(api::HttpDuplex &http)
    {
        InvocationOrErrCode req = _prepareInvocation(http.req);
        if (std::holds_alternative<ErrorCode>(req)) {
            http.resp.status = std::get<ErrorCode>(req);
            return true;
        }

        auto &script = std::get<CGIInvocation>(req);
        const auto result = _runCGI(script);

        if (http.req.method != api::http::Method::head) {
            std::transform(result.stdoutData.begin(), result.stdoutData.end(),
                           std::back_inserter(http.resp.body), [](auto c) { return static_cast<std::byte>(c); });
        }
        http.resp.headers["Content-Type"] = "text/html";
        http.resp.headers["Content-Length"] = std::to_string(result.stdoutData.size());
        http.resp.status = result.exitStatus != 0
                           ? api::http::common_status::internal_server_error
                           : api::http::common_status::ok;
        return true;
    }

    api::Module *create() noexcept
    {
        return new PHPCGI;
    }
}

lib_alias_function(zia::modules::create, create);
