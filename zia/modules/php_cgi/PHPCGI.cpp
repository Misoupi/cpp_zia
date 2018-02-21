//
// Created by Blowa on 10/02/2018.
//

#include <iostream>
#include <boost/filesystem.hpp>
#include <core/lib/Alias.hpp>
#include <api/conf.h>
#include <utils/BufferUtils.hpp>
#include "PHPCGI.hpp"

namespace fs = boost::filesystem;

namespace zia::modules
{
    PHPCGI::Result PHPCGI::_runCGI(const std::string &pathToFile) noexcept
    {
        Result ret;
        boost::process::ipstream outStream;
        boost::process::ipstream errStream;

        try {
            boost::process::child childProcess(_cgiPath, "--no-header", pathToFile,
                                               boost::process::std_out > outStream,
                                               boost::process::std_err > errStream);
            childProcess.wait();
            ret.exitStatus = childProcess.exit_code();
        } catch (const std::exception &e) {
            ret.exitStatus = 127;
            ret.stdoutData.clear();
            if (ret.stderrData.empty()) {
                ret.stderrData = std::string(e.what());
            }
            return ret;
        }
        ret.stdoutData = std::string(std::istreambuf_iterator<char>{outStream}, {});
        ret.stderrData = std::string(std::istreambuf_iterator<char>{errStream}, {});
        return ret;
    }

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

    bool PHPCGI::exec(api::HttpDuplex &http)
    {
        const std::string_view extension = ".php";
        if (http.req.uri.size() <= 4u || http.req.uri.substr(http.req.uri.find_last_of('.')) != extension)
            return true;

        auto scriptPath = fs::path(_filePath) / http.req.uri;
        if (!fs::exists(scriptPath)) {
            http.resp.status = api::http::common_status::not_found;
            return true;
        }

        const auto result = _runCGI(scriptPath.string());

        if (http.req.method == api::http::Method::get || http.req.method == api::http::Method::head) {
            // head == no body
            if (http.req.method != api::http::Method::head) {
                std::transform(result.stdoutData.begin(), result.stdoutData.end(),
                               std::back_inserter(http.resp.body), [](auto c) { return static_cast<std::byte>(c); });
            }
            http.resp.headers["Content-Length"] = std::to_string(result.stdoutData.size());
            http.resp.status = result.exitStatus != 0
                               ? api::http::common_status::internal_server_error
                               : api::http::common_status::ok;
        }
        return true;
    }

    api::Module *create() noexcept
    {
        return new PHPCGI;
    }
}

lib_alias_function(zia::modules::create, create);
