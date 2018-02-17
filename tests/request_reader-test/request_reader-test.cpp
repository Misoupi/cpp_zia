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
static const auto modulesPath = fs::current_path().parent_path() / "modules" / "request_reader";
using ModuleCreator = zia::api::Module *(*)();

TEST(RequestReader, Basic)
{
    auto symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> requestReader((*symbol)());

    const std::string reqData = "GET /docs/index.html HTTP/1.1\r\n"
        "Host: www.nowhere123.com\r\n"
        "Accept: image/gif, image/jpeg, */*\r\n"
        "Accept-Language: en-us\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
        "\r\n";

    api::HttpDuplex duplex;
    duplex.raw_req = utils::stringToRaw(reqData);

    ASSERT_TRUE(requestReader->exec(duplex));
    api::HttpRequest &req = duplex.req;

    ASSERT_EQ(req.method, api::http::Method::get);
    ASSERT_EQ(req.uri, "/docs/index.html");
    ASSERT_EQ(req.version, api::http::Version::http_1_1);

    ASSERT_EQ(req.headers["Host"], "www.nowhere123.com");
    ASSERT_EQ(req.headers["Accept-Language"], "en-us");
}

TEST(RequestReader, WithBody)
{
    auto symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> requestReader((*symbol)());

    const std::string reqData = "PUT /uploads/lala.txt HTTP/1.1\r\n"
        "Host: www.nowhere123.com\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 6\r\n"
        "\r\n"
        "lalala";

    api::HttpDuplex duplex;
    duplex.raw_req = utils::stringToRaw(reqData);

    ASSERT_TRUE(requestReader->exec(duplex));
    api::HttpRequest &req = duplex.req;

    ASSERT_EQ(req.method, api::http::Method::put);
    ASSERT_EQ(req.uri, "/uploads/lala.txt");
    ASSERT_EQ(req.version, api::http::Version::http_1_1);

    ASSERT_EQ(req.headers["Host"], "www.nowhere123.com");
    ASSERT_EQ(req.headers["Content-Type"], "text/plain");
    ASSERT_EQ(req.headers["Content-Length"], "6");

    ASSERT_EQ("lalala", utils::rawToString(req.body));
}

TEST(RequestReader, InvalidContent)
{
    auto symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> requestReader((*symbol)());

    api::HttpDuplex duplex;
    duplex.raw_req = utils::stringToRaw("lalalalala");
    ASSERT_FALSE(requestReader->exec(duplex));
}

TEST(RequestReader, InvalidProtocol)
{
    auto symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> requestReader((*symbol)());

    api::HttpDuplex duplex;
    duplex.raw_req = utils::stringToRaw("GET /lalala.html HTTPAAAAAAAAA/1.1\r\n\r\n");
    ASSERT_FALSE(requestReader->exec(duplex));

    duplex.raw_req = utils::stringToRaw("GET /lalala.html HTTP/lala\r\n\r\n");
    ASSERT_FALSE(requestReader->exec(duplex));

    duplex.raw_req = utils::stringToRaw("GET /lalala.html HTTP/2323.3\r\n\r\n");
    ASSERT_FALSE(requestReader->exec(duplex));
}

TEST(RequestReader, InvalidHeader)
{
    auto symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> requestReader((*symbol)());

    api::HttpDuplex duplex;
    duplex.raw_req = utils::stringToRaw("GET /lalala.html HTTP/1.1\r\nLol\r\n\r\n");
    ASSERT_FALSE(requestReader->exec(duplex));
}
