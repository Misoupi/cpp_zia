//
// Created by doom on 08/02/18
//

#include <fstream>
#include <experimental/filesystem>
#include <gtest/gtest.h>
#include <core/lib/Lib.hpp>
#include <utils/BufferUtils.hpp>
#include <api/module.h>

using namespace zia;
static const auto modulesPath = fs::current_path().parent_path() / "modules" / "request_writer";
using ModuleCreator = zia::api::Module *(*)();

TEST(RequestWriter, Basic)
{
    auto symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> requestWriter((*symbol)());

    const std::string respData = "HTTP/1.1 200 OK\r\n"
        "Content-Length: 44\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html><body><h1>It works!</h1></body></html>";
    api::Net::Raw raw = utils::stringToRaw(respData);

    api::HttpDuplex duplex;
    api::HttpResponse &resp = duplex.resp;
    resp.version = api::http::Version::http_1_1;
    resp.status = 200;
    resp.reason = "OK";
    resp.headers["Content-Length"] = "44";
    resp.headers["Content-Type"] = "text/html";
    resp.body = utils::stringToRaw("<html><body><h1>It works!</h1></body></html>");

    ASSERT_TRUE(requestWriter->exec(duplex));
    ASSERT_EQ(duplex.raw_resp.size(), raw.size());
    for (size_t i = 0; i < raw.size(); ++i) {
        ASSERT_EQ(raw[i], duplex.raw_resp[i]);
    }
}
