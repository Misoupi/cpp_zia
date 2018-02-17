//
// Created by doom on 08/02/18.
//

#include <sstream>
#include <core/lib/Alias.hpp>
#include <utils/Automaton.hpp>
#include <utils/BufferUtils.hpp>
#include "HTTPParser.hpp"
#include "RequestReader.hpp"

namespace zia::modules
{
    bool RequestReader::config([[maybe_unused]] const zia::api::Conf &conf)
    {
        return true;
    }

#define fail_if(x)                                                            \
    if (x) {                                                                  \
        return false;                                                         \
    }

    bool RequestReader::exec(zia::api::HttpDuplex &http)
    {
        HTTPParser parser;

        auto pair = parser.feed(http.raw_req.begin(), http.raw_req.end());
        if (pair.first != HTTPParser::Result::ParseSuccess)
            return false;
        http.req = parser.extractRequest();
        std::copy(pair.second, http.raw_req.end(), std::back_inserter(http.req.body));
        return true;
    }

#undef fail_if

    api::Module *create() noexcept
    {
        return new RequestReader;
    }
}

lib_alias_function(zia::modules::create, create);
