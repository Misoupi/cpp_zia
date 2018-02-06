//
// Created by doom on 06/02/18
//

#include <gtest/gtest.h>
#include <ConfigLoader.hpp>

TEST(ConfigLoader, Basic)
{
    zia::ConfigLoader loader;

    ASSERT_TRUE(loader.loadFromFile(fs::path("conf.json")));
    auto conf = loader.extractConfig();
    ASSERT_TRUE(std::holds_alternative<zia::api::ConfArray>(conf["modules"].v));
    ASSERT_TRUE(std::holds_alternative<std::string>(conf["modules_path"].v));
}

TEST(ConfigLoader, ErrorHandling)
{
    zia::ConfigLoader loader;

    ASSERT_FALSE(loader.loadFromFile(fs::path("lala")));
}