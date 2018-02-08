//
// Created by doom on 08/02/18.
//

#ifndef ZIA_BUFFERUTILS_HPP
#define ZIA_BUFFERUTILS_HPP

#include <string>
#include <algorithm>
#include <api/net.h>

namespace utils
{
    static inline std::string rawToString(const zia::api::Net::Raw &raw) noexcept
    {
        std::string ret;

        std::transform(raw.begin(), raw.end(), std::back_inserter(ret), [](auto c) {
            return static_cast<char>(c);
        });
        return ret;
    }

    static inline zia::api::Net::Raw stringToRaw(const std::string &str) noexcept
    {
        zia::api::Net::Raw ret;

        std::transform(str.begin(), str.end(), std::back_inserter(ret), [](auto c) {
            return static_cast<std::byte>(c);
        });
        return ret;
    }
}

#endif //ZIA_BUFFERUTILS_HPP
