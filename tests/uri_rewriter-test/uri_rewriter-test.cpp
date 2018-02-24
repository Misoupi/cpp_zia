//
// Created by doom on 19/02/18
//

#include <gtest/gtest.h>
#include <core/lib/Lib.hpp>
#include <api/module.h>

using namespace zia;
static const auto modulesPath = fs::current_path().parent_path() / "modules" / "uri_rewriter";
using ModuleCreator = zia::api::Module *(*)();

TEST(URIRewriter, Subdomain)
{
    auto symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> uriRewriter((*symbol)());

    api::Conf conf;
    api::ConfValue value;
    value.v = std::string("lalala.lol");
    conf.emplace("domain", std::move(value));
    api::Conf subdomains;
    value.v = std::string("/doc");
    subdomains.emplace("doc", std::move(value));
    value.v = subdomains;
    conf.emplace("subdomains", std::move(value));

    ASSERT_TRUE(uriRewriter->config(conf));

    api::HttpDuplex duplex{};
    duplex.req.uri = "/dir/lol.html";
    duplex.req.headers.emplace("Host", "doc.lalala.lol");
    ASSERT_TRUE(uriRewriter->exec(duplex));
    ASSERT_EQ(duplex.resp.status, api::http::common_status::moved_permanently);
    ASSERT_EQ(duplex.resp.headers["Location"], "http://lalala.lol/doc/dir/lol.html");
}

TEST(URIRewriter, UnknownDomain)
{
    auto symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> uriRewriter((*symbol)());

    api::Conf conf;
    api::ConfValue value;
    value.v = std::string("lalala.lol");
    conf.emplace("domain", std::move(value));
    api::Conf subdomains;
    value.v = std::string("/doc");
    subdomains.emplace("doc", std::move(value));
    value.v = subdomains;
    conf.emplace("subdomains", std::move(value));

    ASSERT_TRUE(uriRewriter->config(conf));

    api::HttpDuplex duplex;
    duplex.req.uri = "/dir/lol.html";
    duplex.req.headers.emplace("Host", "doc.unknown.lol");
    ASSERT_TRUE(uriRewriter->exec(duplex));
    ASSERT_EQ(duplex.req.headers["Host"], "doc.unknown.lol");
    ASSERT_EQ(duplex.req.uri, "/dir/lol.html");
}
