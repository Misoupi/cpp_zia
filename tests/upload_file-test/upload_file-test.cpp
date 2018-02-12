//
// Created by riz on 11/02/18
//

#include <fstream>
#include <iterator>
#include <gtest/gtest.h>
#include <core/lib/Lib.hpp>
#include <api/module.h>

class UploadFileCreate : public ::testing::Test
{
    void SetUp() override
    {
        if (fs::exists(fs::current_path() / "create.html"))
            fs::remove(fs::current_path() / "create.html");
    }

    void TearDown() override
    {
        if (fs::exists(fs::current_path() / "create.html"))
            fs::remove(fs::current_path() / "create.html");
    }
};

TEST_F(UploadFileCreate, Create)
{
    const std::string upStr = "<html>\r\n<header>\r\n    <title> this is title </title>\r\n</header>\r\n<body> Hello World ! </body>\r\n</html>";

    zia::api::HttpDuplex duplex;
    duplex.req.uri = (fs::current_path() / "create.html").string();
    duplex.req.method = zia::api::http::Method::put;
    for (auto cur : upStr) {
        duplex.req.body.push_back(static_cast<std::byte>(cur));
    }
    auto modulesPath = fs::current_path().parent_path() / "modules" / "upload_file";
    using ModuleCreator = zia::api::Module *(*)();
    lib::Symbol<ModuleCreator> symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> ptr((*symbol)());

    ASSERT_FALSE(fs::exists(fs::current_path() / "create.html"));
    ASSERT_TRUE(ptr->exec(duplex));
    ASSERT_TRUE(fs::exists(fs::current_path() / "create.html"));
    std::ifstream ifs(fs::current_path() / "create.html");
    const std::string resStr{std::istreambuf_iterator<char>(ifs),
                       std::istreambuf_iterator<char>()};
    ASSERT_EQ(upStr, resStr);
    ASSERT_EQ(duplex.resp.status, zia::api::http::common_status::created);
}

class UploadFileTrunc : public ::testing::Test
{
    void SetUp() override
    {
        std::ofstream ofs((fs::current_path() / "trunc.html").string(), std::ios_base::trunc);
        ofs << "<html>\r\n"
            << "<header>\r\n"
            << "    <title> This is title </title>\r\n"
            << "</header>\r\n"
            << "<body> I ain't born yet </body>\r\n"
            << "</html>"  << std::flush;
    }

    void TearDown() override
    {
        if (fs::exists(fs::current_path() / "trunc.html"))
            fs::remove(fs::current_path() / "trunc.html");
    }
};

TEST_F(UploadFileTrunc, Trunc)
{
    const std::string upStr = "<html>\r\n<header>\r\n    <title> This is title </title>\r\n</header>\r\n<body> Hello World ! </body>\r\n</html>";

    zia::api::HttpDuplex duplex;
    duplex.req.uri = (fs::current_path() / "trunc.html").string();
    duplex.req.method = zia::api::http::Method::put;
    for (auto cur : upStr) {
        duplex.req.body.push_back(static_cast<std::byte>(cur));
    }

    auto modulesPath = fs::current_path().parent_path() / "modules" / "upload_file";
    using ModuleCreator = zia::api::Module *(*)();
    lib::Symbol<ModuleCreator> symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> ptr((*symbol)());

    ASSERT_TRUE(fs::exists(fs::current_path() / "trunc.html"));
    ASSERT_TRUE(ptr->exec(duplex));
    ASSERT_TRUE(fs::exists(fs::current_path() / "trunc.html"));
    std::ifstream ifs(fs::current_path() / "trunc.html");
    const std::string resStr{std::istreambuf_iterator<char>(ifs),
                       std::istreambuf_iterator<char>()};
    ASSERT_EQ(upStr, resStr);
    ASSERT_EQ(duplex.resp.status, zia::api::http::common_status::ok);
}

class UploadFileDenied : public ::testing::Test
{
    void SetUp() override
    {
        std::ofstream ofs((fs::current_path() / "denied.html").string(), std::ios_base::trunc);
        ofs << "<html>\r\n"
            << "<header>\r\n"
            << "    <title> This is title </title>\r\n"
            << "</header>\r\n"
            << "<body> I ain't born yet </body>\r\n"
            << "</html>"  << std::flush;

        fs::permissions(fs::current_path() / "denied.html", fs::perms::remove_perms
                                                            | fs::perms::owner_write
                                                            | fs::perms::group_all
                                                            | fs::perms::others_all);

    }

    void TearDown() override
    {
        if (fs::exists(fs::current_path() / "denied.html")) {
            fs::permissions(fs::current_path() / "denied.html", fs::perms::add_perms
                                                                | fs::perms::owner_write);
            fs::remove(fs::current_path() / "denied.html");
        }
    }
};

TEST_F(UploadFileDenied, Denied)
{
    const std::string oldStr = "<html>\r\n<header>\r\n    <title> This is title </title>\r\n</header>\r\n<body> I ain't born yet </body>\r\n</html>";
    const std::string upStr = "<html>\r\n<header>\r\n    <title> This is title </title>\r\n</header>\r\n<body> Hello World ! </body>\r\n</html>";

    zia::api::HttpDuplex duplex;
    duplex.req.uri = (fs::current_path() / "denied.html").string();
    duplex.req.method = zia::api::http::Method::put;
    for (auto cur : upStr) {
        duplex.req.body.push_back(static_cast<std::byte>(cur));
    }

    auto modulesPath = fs::current_path().parent_path() / "modules" / "upload_file";
    using ModuleCreator = zia::api::Module *(*)();
    lib::Symbol<ModuleCreator> symbol = lib::getSymbol<ModuleCreator>(modulesPath, "create");
    std::unique_ptr<zia::api::Module> ptr((*symbol)());

    ASSERT_TRUE(fs::exists(fs::current_path() / "denied.html"));
    ASSERT_TRUE(ptr->exec(duplex));
    ASSERT_TRUE(fs::exists(fs::current_path() / "denied.html"));
    std::ifstream ifs(fs::current_path() / "denied.html");
    const std::string resStr{std::istreambuf_iterator<char>(ifs),
                       std::istreambuf_iterator<char>()};
    ASSERT_EQ(oldStr, resStr);
    ASSERT_EQ(duplex.resp.status, zia::api::http::common_status::unauthorized);
}
