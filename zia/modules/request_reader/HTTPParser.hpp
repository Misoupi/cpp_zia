//
// Created by doom on 17/02/18.
//

#ifndef ZIA_HTTPPARSER_HPP
#define ZIA_HTTPPARSER_HPP

#include <optional>
#include <api/http.h>
#include <utils/Automaton.hpp>

namespace zia
{
    class HTTPParser
    {
        enum InternalState
        {
            Error,
            Success,
            Start,
            InMethod,
            InURI,
            ExpectingH,
            ExpectingFirstT,
            ExpectingSecondT,
            ExpectingP,
            ExpectingSlash,
            MajorStart,
            InMajor,
            MinorStart,
            InMinor,
            ExpectingEndOfRequestLine,
            HeaderLineStart,
            InHeaderFieldKey,
            ExpectingSpaceBeforeHeaderValue,
            InHeaderFieldValue,
            ExpectingEndOfFieldLine,
            ExpectingEndOfHeader,
        };

    public:
        HTTPParser() noexcept :
            _autom(Start, Error, Success, std::unordered_map<InternalState, std::function<InternalState(char)>>{
                {Start,
                    std::bind(&HTTPParser::_start, this, std::placeholders::_1)},
                {InMethod,
                    std::bind(&HTTPParser::_inMethod, this, std::placeholders::_1)},
                {InURI,
                    std::bind(&HTTPParser::_inURI, this, std::placeholders::_1)},
                {ExpectingH,
                    std::bind(&HTTPParser::_expectingH, this, std::placeholders::_1)},
                {ExpectingFirstT,
                    std::bind(&HTTPParser::_expectingFirstT, this, std::placeholders::_1)},
                {ExpectingSecondT,
                    std::bind(&HTTPParser::_expectingSecondT, this, std::placeholders::_1)},
                {ExpectingP,
                    std::bind(&HTTPParser::_expectingP, this, std::placeholders::_1)},
                {ExpectingSlash,
                    std::bind(&HTTPParser::_expectingSlash, this, std::placeholders::_1)},
                {MajorStart,
                    std::bind(&HTTPParser::_majorStart, this, std::placeholders::_1)},
                {InMajor,
                    std::bind(&HTTPParser::_inMajor, this, std::placeholders::_1)},
                {MinorStart,
                    std::bind(&HTTPParser::_minorStart, this, std::placeholders::_1)},
                {InMinor,
                    std::bind(&HTTPParser::_inMinor, this, std::placeholders::_1)},
                {ExpectingEndOfRequestLine,
                    std::bind(&HTTPParser::_expectingEndOfRequestLine, this, std::placeholders::_1)},
                {HeaderLineStart,
                    std::bind(&HTTPParser::_headerLineStart, this, std::placeholders::_1)},
                {InHeaderFieldKey,
                    std::bind(&HTTPParser::_inHeaderFieldKey, this, std::placeholders::_1)},
                {ExpectingSpaceBeforeHeaderValue,
                    std::bind(&HTTPParser::_expectingSpaceBeforeHeaderValue, this, std::placeholders::_1)},
                {InHeaderFieldValue,
                    std::bind(&HTTPParser::_inHeaderFieldValue, this, std::placeholders::_1)},
                {ExpectingEndOfFieldLine,
                    std::bind(&HTTPParser::_expectingEndOfFieldLine, this, std::placeholders::_1)},
                {ExpectingEndOfHeader,
                    std::bind(&HTTPParser::_expectingEndOfHeader, this, std::placeholders::_1)},
            })
        {
        }

        template <typename Iterator>
        auto feed(Iterator begin, Iterator end) noexcept
        {
            return _autom.feed(begin, end);
        }

        zia::api::HttpRequest &&extractRequest() noexcept
        {
            return std::move(_req);
        }

        using Result = parsing::Automaton<InternalState>::Result;

    private:
        parsing::Automaton<InternalState> _autom;
        zia::api::HttpRequest _req;

        struct HeaderField
        {
            std::string key;
            std::string value;
        };

        std::string _method;
        std::string _version;
        HeaderField _curField;

        static bool _isValid(char c) noexcept
        {
            return c > 31 && c < 127;
        }

        static bool _isSpecial(char c) noexcept
        {
            static const std::string_view specialChars = " \t()[]{}<>:,;\"/";

            return specialChars.find(c) != std::string_view::npos;
        }

        static std::optional<api::http::Version> stringToVersion(const std::string_view sv) noexcept
        {
            static std::unordered_map<std::string_view, api::http::Version> conversions{
                {"0.9", api::http::Version::http_0_9},
                {"1.0", api::http::Version::http_1_0},
                {"1.1", api::http::Version::http_1_1},
                {"2.0", api::http::Version::http_2_0},
            };
            auto it = conversions.find(sv);
            if (it == conversions.end())
                return {};
            return {it->second};
        }

        static std::optional<api::http::Method> stringToMethod(const std::string_view sv) noexcept
        {
            static std::unordered_map<std::string_view, api::http::Method> conversions{
                {"OPTIONS", api::http::Method::options},
                {"GET",     api::http::Method::get},
                {"HEAD",    api::http::Method::head},
                {"POST",    api::http::Method::post},
                {"PUT",     api::http::Method::put},
                {"DELETE",  api::http::Method::delete_},
                {"TRACE",   api::http::Method::trace},
                {"CONNECT", api::http::Method::connect},
            };
            auto it = conversions.find(sv);
            if (it == conversions.end())
                return {};
            return {it->second};
        }

#define fail_if(x)                                                          \
    if (x) {                                                                \
        return Error;                                                       \
    }

#define fail_if_invalid(x)                                                  \
    if (!_isValid(x)) {                                                     \
        return Error;                                                       \
    }

#define fail_if_special(x)                                                  \
    if (_isSpecial(x)) {                                                    \
        return Error;                                                       \
    }

        InternalState _start(char c) noexcept
        {
            fail_if_invalid(c);
            fail_if_special(c);
            _method.push_back(c);
            return InMethod;
        }

        InternalState _inMethod(char c) noexcept
        {
            if (c == ' ') {
                auto opt = stringToMethod(_method);
                fail_if(!opt);
                _req.method = *opt;
                return InURI;
            }
            fail_if_invalid(c);
            fail_if_special(c);
            _method.push_back(c);
            return InMethod;
        }

        InternalState _inURI(char c) noexcept
        {
            if (c == ' ')
                return ExpectingH;
            fail_if_invalid(c);
            _req.uri.push_back(c);
            return InURI;
        }

        InternalState _expectingH(char c) noexcept
        {
            if (c == 'H')
                return ExpectingFirstT;
            return Error;
        }

        InternalState _expectingFirstT(char c) noexcept
        {
            if (c == 'T')
                return ExpectingSecondT;
            return Error;
        }

        InternalState _expectingSecondT(char c) noexcept
        {
            if (c == 'T')
                return ExpectingP;
            return Error;
        }

        InternalState _expectingP(char c) noexcept
        {
            if (c == 'P')
                return ExpectingSlash;
            return Error;
        }

        InternalState _expectingSlash(char c) noexcept
        {
            if (c == '/')
                return MajorStart;
            return Error;
        }

        InternalState _majorStart(char c) noexcept
        {
            if (isdigit(c)) {
                _version.push_back(c);
                return InMajor;
            }
            return Error;
        }

        InternalState _inMajor(char c) noexcept
        {
            if (c == '.') {
                _version.push_back(c);
                return MinorStart;
            }
            if (isdigit(c)) {
                _version.push_back(c);
                return InMajor;
            }
            return Error;
        }

        InternalState _minorStart(char c) noexcept
        {
            if (isdigit(c)) {
                _version.push_back(c);
                return InMinor;
            }
            return Error;
        }

        InternalState _inMinor(char c) noexcept
        {
            if (c == '\r') {
                auto opt = stringToVersion(_version);
                fail_if(!opt);
                _req.version = *opt;
                return ExpectingEndOfRequestLine;
            }
            if (isdigit(c)) {
                _version.push_back(c);
                return InMinor;
            }
            return Error;
        }

        InternalState _expectingEndOfRequestLine(char c) noexcept
        {
            if (c == '\n')
                return HeaderLineStart;
            return Error;
        }

        InternalState _headerLineStart(char c) noexcept
        {
            if (c == '\r')
                return ExpectingEndOfHeader;
            //TODO: Support HTTP/1.1's LWS
            fail_if_invalid(c);
            fail_if_special(c);
            _curField.key.push_back(c);
            return InHeaderFieldKey;
        }

        InternalState _inHeaderFieldKey(char c) noexcept
        {
            if (c == ':')
                return ExpectingSpaceBeforeHeaderValue;
            fail_if_special(c);
            fail_if_invalid(c);
            _curField.key.push_back(c);
            return InHeaderFieldKey;
        }

        InternalState _expectingSpaceBeforeHeaderValue(char c) noexcept
        {
            if (c == ' ')
                return InHeaderFieldValue;
            return Error;
        }

        InternalState _inHeaderFieldValue(char c) noexcept
        {
            if (c == '\r')
                return ExpectingEndOfFieldLine;
            fail_if_invalid(c);
            _curField.value.push_back(c);
            return InHeaderFieldValue;
        }

        InternalState _expectingEndOfFieldLine(char c) noexcept
        {
            if (c == '\n') {
                _req.headers.emplace(std::move(_curField.key), std::move(_curField.value));
                _curField.key.clear();
                _curField.value.clear();
                return HeaderLineStart;
            }
            return Error;
        }

        InternalState _expectingEndOfHeader(char c) noexcept
        {
            if (c == '\n')
                return Success;
            return Error;
        }

#undef fail_if_invalid
#undef fail_if_special
#undef fail_if
    };
}

#endif //ZIA_HTTPPARSER_HPP
