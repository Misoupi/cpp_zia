//
// Created by doom on 06/02/18.
//

#include <fstream>
#include <nlohmann/json.hpp>
#include "ConfigLoader.hpp"

using json = nlohmann::json;

static zia::api::ConfValue extractNode(const json &node) noexcept;

static zia::api::ConfObject extractObject(const json &object) noexcept
{
    zia::api::ConfObject ret;

    for (auto it = object.begin(); it != object.end(); ++it) {
        ret.emplace(it.key(), extractNode(it.value()));
    }
    return ret;
}

static zia::api::ConfArray extractArray(const json &array) noexcept
{
    zia::api::ConfArray ret;

    for (const auto &curNode : array) {
        ret.push_back(extractNode(curNode));
    }
    return ret;
}

static zia::api::ConfValue extractNode(const json &node) noexcept
{
    zia::api::ConfValue ret{};

    switch (node.type()) {
        case nlohmann::detail::value_t::object:
            ret.v = extractObject(node);
            break;
        case nlohmann::detail::value_t::array:
            ret.v = extractArray(node);
            break;
        case nlohmann::detail::value_t::string:
            ret.v = node.get<std::string>();
            break;
        case nlohmann::detail::value_t::boolean:
            ret.v = node.get<bool>();
            break;
        case nlohmann::detail::value_t::number_integer:
            ret.v = static_cast<long long>(node.get<int>());
            break;
        case nlohmann::detail::value_t::number_unsigned:
            ret.v = static_cast<long long>(node.get<unsigned int>());
            break;
        case nlohmann::detail::value_t::number_float:
            ret.v = node.get<float>();
            break;
        default:
            break;
    }
    return ret;
}

bool zia::ConfigLoader::loadFromFile(const fs::path &path) noexcept
{
    std::ifstream ifs(path.string());

    if (!ifs.is_open())
        return false;
    nlohmann::json json;
    ifs >> json;
    _conf = extractObject(json);
    return true;
}

zia::api::Conf zia::ConfigLoader::extractConfig() noexcept
{
    return _conf;
}