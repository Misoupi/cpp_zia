//
// Created by doom on 06/02/18
//

#include <fstream>
#include <gtest/gtest.h>
#include <core/lib/Lib.hpp>
#include <api/module.h>

TEST(StaticFile, Basic)
{
    fs::copy(fs::current_path().parent_path() / "tests/staticfile-test/hello.html", fs::current_path(),
             fs::copy_options::overwrite_existing);
    zia::api::HttpDuplex duplex;
    duplex.req.uri = (fs::current_path() / "hello.html").string();
    std::ifstream ifs((fs::current_path() / "hello.html").string());
    std::string str((std::istreambuf_iterator<char>(ifs)),
                    std::istreambuf_iterator<char>());
    duplex.req.method = zia::api::http::Method::get;


    auto modulesPath = fs::current_path().parent_path() / "modules" / "static_file";
    using ModuleCreator = zia::api::Module *(*)();
    lib::Symbol<ModuleCreator> symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> ptr((*symbol)());

    ASSERT_TRUE(ptr->exec(duplex));
    std::string res;
    for (auto cur : duplex.resp.body) {
        res.push_back(static_cast<unsigned char>(cur));
    }
    ASSERT_EQ(res, str);
    ASSERT_EQ(duplex.resp.status, zia::api::http::common_status::ok);
    ASSERT_NO_THROW(ASSERT_EQ(std::stoi(duplex.resp.headers.at("Content-Length")), str.size()));
}

TEST(StaticFile, NonExistent)
{
    zia::api::HttpDuplex duplex;
    duplex.req.uri = (fs::current_path() / "nonexistent.html").string();
    duplex.req.method = zia::api::http::Method::get;


    auto modulesPath = fs::current_path().parent_path() / "modules" / "static_file";
    using ModuleCreator = zia::api::Module *(*)();
    lib::Symbol<ModuleCreator> symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> ptr((*symbol)());


    ASSERT_TRUE(ptr->exec(duplex));
    ASSERT_EQ(duplex.resp.status, zia::api::http::common_status::not_found);
}

TEST(StaticFile, BinaryFile)
{
    fs::copy(fs::current_path().parent_path() / "tests/staticfile-test/binary-file", fs::current_path(),
             fs::copy_options::overwrite_existing);
    zia::api::HttpDuplex duplex;
    duplex.req.uri = (fs::current_path() / "binary-file").string();
    std::ifstream ifs((fs::current_path() / "binary-file").string());
    std::string str((std::istreambuf_iterator<char>(ifs)),
                    std::istreambuf_iterator<char>());
    duplex.req.method = zia::api::http::Method::get;


    auto modulesPath = fs::current_path().parent_path() / "modules" / "static_file";
    using ModuleCreator = zia::api::Module *(*)();
    lib::Symbol<ModuleCreator> symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> ptr((*symbol)());


    ASSERT_TRUE(ptr->exec(duplex));
    std::string res;
    for (auto cur : duplex.resp.body) {
        res.push_back(static_cast<unsigned char>(cur));
    }
    ASSERT_EQ(res, str);
    ASSERT_EQ(duplex.resp.status, zia::api::http::common_status::ok);
    ASSERT_NO_THROW(ASSERT_EQ(std::stoi(duplex.resp.headers.at("Content-Length")), str.size()));
}