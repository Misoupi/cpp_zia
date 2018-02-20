//
// Created by doom on 20/02/18.
//

#include <iostream>
#include <sstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <core/lib/Alias.hpp>
#include <utils/BufferUtils.hpp>
#include "ResponseGZipCompressor.hpp"

namespace zia::modules
{
    bool ResponseGZipCompressor::config(const api::Conf &conf)
    {
        auto it = conf.find("gzip_min_size");

        if (it != conf.end()) {
            if (!std::holds_alternative<long long>(it->second.v))
                return false;
            auto value = std::get<long long>(it->second.v);
            if (value <= 0)
                return false;
            _minSize = static_cast<size_t>(value);
        } else
            _minSize = 4096000;
        return true;
    }

    namespace details
    {
        class CommaSplittingLocale : public std::ctype<char>
        {
        public:
            CommaSplittingLocale() noexcept : std::ctype<char>(_getMasks())
            {
            }

        private:
            static const std::ctype_base::mask *_getMasks() noexcept
            {
                static std::vector<std::ctype_base::mask> masks(table_size, std::ctype_base::mask());
                masks[','] = std::ctype_base::space;
                masks[' '] = std::ctype_base::space;
                return masks.data();
            }
        };
    }

    bool ResponseGZipCompressor::_isReqAcceptingGzip(api::HttpDuplex &http) const noexcept
    {
        auto acceptEncodingIt = http.req.headers.find("Accept-Encoding");

        if (acceptEncodingIt != http.req.headers.end()) {
            std::istringstream iss(acceptEncodingIt->second);

            iss.imbue(std::locale(std::locale(), new details::CommaSplittingLocale));
            std::string str;
            while (iss >> str) {
                if (str == "gzip")
                    return true;
            }
        }
        return false;
    }

    bool ResponseGZipCompressor::exec(api::HttpDuplex &http)
    {
        if (http.resp.body.size() >= _minSize && _isReqAcceptingGzip(http)) {
            namespace iostreams = boost::iostreams;

            std::stringstream compressed;
            auto origSize = http.resp.body.size();
            std::stringstream origin(utils::rawToString(http.resp.body));

            iostreams::filtering_streambuf<iostreams::input> out;
            out.push(iostreams::gzip_compressor(iostreams::gzip_params(iostreams::gzip::best_compression)));
            out.push(origin);
            iostreams::copy(out, compressed);

            http.resp.body = utils::stringToRaw(compressed.str());
            http.resp.headers["Content-Length"] = std::to_string(http.resp.body.size());
            http.resp.headers["Content-Encoding"] = "gzip";
        }
        return true;
    }

    api::Module *create() noexcept
    {
        return new ResponseGZipCompressor;
    }
}

lib_alias_function(zia::modules::create, create);
