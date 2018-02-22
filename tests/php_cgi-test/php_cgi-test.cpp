//
// Created by doom on 08/02/18
//

#include <fstream>
#include <experimental/filesystem>
#include <gtest/gtest.h>
#include <core/lib/Lib.hpp>
#include <api/module.h>
#include <utils/BufferUtils.hpp>

using namespace zia;
static const auto cgiModulePath = fs::current_path().parent_path() / "modules" / "php_cgi";
using ModuleCreator = zia::api::Module *(*)();

#ifndef CGIPATH
# ifdef USING_WINDOWS
# define CGIPATH "C:\\php\\php-cgi.exe"
# else
# define CGIPATH "/usr/bin/php-cgi"
#endif
#endif

TEST(PHPCGI, HelloWorld)
{
    auto cgiSymbol = lib::getSymbol<ModuleCreator>(cgiModulePath, "create");
    std::unique_ptr<zia::api::Module> phpCgi((*cgiSymbol)());

    api::Conf conf;
    api::ConfValue cgiPath;
    cgiPath.v = std::string(CGIPATH);
    conf.emplace("php_cgi_exe", std::move(cgiPath));
    api::ConfValue filePath;
    filePath.v = std::string("tests_files");
    conf.emplace("root_directory", std::move(filePath));

    const std::string expected = "<p>Hello World</p>";

    api::HttpDuplex duplex;
    duplex.req.method = api::http::Method::get;
    duplex.req.uri = "helloworld.php";

    ASSERT_TRUE(phpCgi->config(conf));
    ASSERT_TRUE(phpCgi->exec(duplex));
    api::HttpResponse &resp = duplex.resp;
    ASSERT_EQ(expected, utils::rawToString(resp.body));
}

TEST(PHPCGI, NotPhpExtension)
{
    auto cgiSymbol = lib::getSymbol<ModuleCreator>(cgiModulePath, "create");
    std::unique_ptr<zia::api::Module> phpCgi((*cgiSymbol)());

    api::HttpDuplex duplex;
    duplex.req.method = api::http::Method::get;
    duplex.req.uri = "helloworld.js";

    ASSERT_TRUE(phpCgi->exec(duplex));
    ASSERT_EQ(duplex.resp.body.size(), 0u);
}

TEST(PHPCGI, FileNotFound)
{
    auto cgiSymbol = lib::getSymbol<ModuleCreator>(cgiModulePath, "create");
    std::unique_ptr<zia::api::Module> phpCgi((*cgiSymbol)());

    api::Conf conf;
    api::ConfValue cgiPath;
    cgiPath.v = std::string(CGIPATH);
    conf.emplace("php_cgi_exe", std::move(cgiPath));
    api::ConfValue filePath;
    filePath.v = std::string("tests_files");
    conf.emplace("root_directory", std::move(filePath));

    api::HttpDuplex duplex;
    duplex.req.method = api::http::Method::get;
    duplex.req.uri = "not_found.php";

    ASSERT_TRUE(phpCgi->config(conf));
    ASSERT_TRUE(phpCgi->exec(duplex));
    ASSERT_EQ(duplex.resp.status, api::http::common_status::not_found);
}
