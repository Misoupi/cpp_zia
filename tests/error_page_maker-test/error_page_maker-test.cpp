//
// Created by riz on 11/02/18
//

#include <fstream>
#include <iterator>
#include <gtest/gtest.h>
#include <core/lib/Lib.hpp>
#include <api/module.h>

TEST(ErrorPageMaker, Basic)
{
    zia::api::HttpDuplex duplex;

    const std::string expected = "<html>\r\n  <head>\r\n    <title>Error 404 : Not Found</title>\r\n  </head>\r\n  <body>\r\n    <h1>Error 404 : Not Found</h1>\r\n    <p>Pourtant j'ai bien cherché :/</p>\r\n  </body>\r\n</html>";

    auto modulesPath = fs::current_path().parent_path() / "modules" / "error_page_maker";
    using ModuleCreator = zia::api::Module *(*)();
    lib::Symbol<ModuleCreator> symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> ptr((*symbol)());

    duplex.resp.status = 404;
    duplex.resp.reason = "Pourtant j'ai bien cherché :/";

    ASSERT_TRUE(ptr->exec(duplex));
    ASSERT_EQ(duplex.resp.headers["Content-Type"], "text/html");
    ASSERT_EQ(duplex.resp.headers["Content-Length"], std::to_string(expected.length()));
    std::string res;
    std::transform(duplex.resp.body.begin(), duplex.resp.body.end(), std::back_inserter(res), [](auto a) {
        return static_cast<char>(a);
    });
    ASSERT_EQ(res, expected);
}

TEST(ErrorPageMaker, Unknown)
{
    zia::api::HttpDuplex duplex;

    const std::string expected = "<html>\r\n  <head>\r\n    <title>Error 666 : Unknown</title>\r\n  </head>\r\n  <body>\r\n    <h1>Error 666 : Unknown</h1>\r\n    <p>Je suis le DEVIIIIIIIIIIIIIIIIL BWWWAAAAAAHHHHHH !!!</p>\r\n  </body>\r\n</html>";

    auto modulesPath = fs::current_path().parent_path() / "modules" / "error_page_maker";
    using ModuleCreator = zia::api::Module *(*)();
    lib::Symbol<ModuleCreator> symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> ptr((*symbol)());

    duplex.resp.status = 666;
    duplex.resp.reason = "Je suis le DEVIIIIIIIIIIIIIIIIL BWWWAAAAAAHHHHHH !!!";

    ASSERT_TRUE(ptr->exec(duplex));
    ASSERT_EQ(duplex.resp.headers["Content-Type"], "text/html");
    ASSERT_EQ(duplex.resp.headers["Content-Length"], std::to_string(expected.length()));
    std::string res;
    std::transform(duplex.resp.body.begin(), duplex.resp.body.end(), std::back_inserter(res), [](auto a) {
        return static_cast<char>(a);
    });
    ASSERT_EQ(res, expected);
}


TEST(ErrorPageMaker, NoError)
{
    zia::api::HttpDuplex duplex;

    const std::string expected = "<html>\r\n  <head>\r\n    <title>you are so dumb</title>\r\n  </head>\r\n  <body>\r\n    <h1>you are really dumb</h1>\r\n    <p>pho real</p>\r\n  </body>\r\n</html>";

    auto modulesPath = fs::current_path().parent_path() / "modules" / "error_page_maker";
    using ModuleCreator = zia::api::Module *(*)();
    lib::Symbol<ModuleCreator> symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> ptr((*symbol)());

    duplex.resp.status = 200;
    duplex.resp.reason = "why so dumb ?";
    duplex.resp.headers["Content-Type"] = "text/html";
    duplex.resp.headers["Content-Length"] = std::to_string(expected.length());
    std::transform(expected.begin(), expected.end(), std::back_inserter(duplex.resp.body), [](auto a) {
        return static_cast<std::byte>(a);
    });
    ASSERT_TRUE(ptr->exec(duplex));
    ASSERT_EQ(duplex.resp.headers["Content-Type"], "text/html");
    ASSERT_EQ(duplex.resp.headers["Content-Length"], std::to_string(expected.length()));
    std::string res;
    std::transform(duplex.resp.body.begin(), duplex.resp.body.end(), std::back_inserter(res), [](auto a) {
        return static_cast<char>(a);
    });
    ASSERT_EQ(res, expected);
}