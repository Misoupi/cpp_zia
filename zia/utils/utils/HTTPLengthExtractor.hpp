//
// Created by doom on 12/02/18.
//

#ifndef ZIA_HTTPLENGTHEXTRACTOR_HPP
#define ZIA_HTTPLENGTHEXTRACTOR_HPP

#include <unordered_map>
#include <functional>
#include <utils/Automaton.hpp>

class HTTPLengthExtractor
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
    HTTPLengthExtractor() noexcept :
        _autom(Start, Error, Success, std::unordered_map<InternalState, std::function<InternalState(char)>>{
            {Start,
                std::bind(&HTTPLengthExtractor::_start, this, std::placeholders::_1)},
            {InMethod,
                std::bind(&HTTPLengthExtractor::_inMethod, this, std::placeholders::_1)},
            {InURI,
                std::bind(&HTTPLengthExtractor::_inURI, this, std::placeholders::_1)},
            {ExpectingH,
                std::bind(&HTTPLengthExtractor::_expectingH, this, std::placeholders::_1)},
            {ExpectingFirstT,
                std::bind(&HTTPLengthExtractor::_expectingFirstT, this, std::placeholders::_1)},
            {ExpectingSecondT,
                std::bind(&HTTPLengthExtractor::_expectingSecondT, this, std::placeholders::_1)},
            {ExpectingP,
                std::bind(&HTTPLengthExtractor::_expectingP, this, std::placeholders::_1)},
            {ExpectingSlash,
                std::bind(&HTTPLengthExtractor::_expectingSlash, this, std::placeholders::_1)},
            {MajorStart,
                std::bind(&HTTPLengthExtractor::_majorStart, this, std::placeholders::_1)},
            {InMajor,
                std::bind(&HTTPLengthExtractor::_inMajor, this, std::placeholders::_1)},
            {MinorStart,
                std::bind(&HTTPLengthExtractor::_minorStart, this, std::placeholders::_1)},
            {InMinor,
                std::bind(&HTTPLengthExtractor::_inMinor, this, std::placeholders::_1)},
            {ExpectingEndOfRequestLine,
                std::bind(&HTTPLengthExtractor::_expectingEndOfRequestLine, this, std::placeholders::_1)},
            {HeaderLineStart,
                std::bind(&HTTPLengthExtractor::_headerLineStart, this, std::placeholders::_1)},
            {InHeaderFieldKey,
                std::bind(&HTTPLengthExtractor::_inHeaderFieldKey, this, std::placeholders::_1)},
            {ExpectingSpaceBeforeHeaderValue,
                std::bind(&HTTPLengthExtractor::_expectingSpaceBeforeHeaderValue, this, std::placeholders::_1)},
            {InHeaderFieldValue,
                std::bind(&HTTPLengthExtractor::_inHeaderFieldValue, this, std::placeholders::_1)},
            {ExpectingEndOfFieldLine,
                std::bind(&HTTPLengthExtractor::_expectingEndOfFieldLine, this, std::placeholders::_1)},
            {ExpectingEndOfHeader,
                std::bind(&HTTPLengthExtractor::_expectingEndOfHeader, this, std::placeholders::_1)},
        })
    {
    }

    void restart()
    {
        _autom.restart();
        _bytes.clear();
    }

    template <typename Iterator>
    auto feed(Iterator begin, Iterator end) noexcept
    {
        return _autom.feed(begin, end);
    }

    const std::vector<std::byte> &buffer() const noexcept
    {
        return _bytes;
    }

    const std::string &contentLength() const noexcept
    {
        return _contentLength;
    }

    using Result = parsing::Automaton<InternalState>::Result;

private:
    parsing::Automaton<InternalState> _autom;
    std::vector<std::byte> _bytes;
    struct HeaderField
    {
        std::string key;
        std::string value;
    };
    HeaderField _curField;
    std::string _contentLength{"0"};

    static bool isValid(char c) noexcept
    {
        return c > 31 && c < 127;
    }

    static bool isSpecial(char c) noexcept
    {
        static const std::string_view specialChars = " \t()[]{}<>:,;\"/";

        return specialChars.find(c) != std::string_view::npos;
    }

    void addChar(char c) noexcept
    {
        _bytes.push_back(static_cast<std::byte>(c));
    }

#define fail_if_invalid(x)                                                  \
    if (!isValid(x)) {                                                      \
        return Error;                                                       \
    }

#define fail_if_special(x)                                                  \
    if (isSpecial(x)) {                                                     \
        return Error;                                                       \
    }

    InternalState _start(char c) noexcept
    {
        addChar(c);
        fail_if_invalid(c);
        fail_if_special(c);
        return InMethod;
    }

    InternalState _inMethod(char c) noexcept
    {
        addChar(c);
        if (c == ' ')
            return InURI;
        fail_if_invalid(c);
        fail_if_special(c);
        return InMethod;
    }

    InternalState _inURI(char c) noexcept
    {
        addChar(c);
        if (c == ' ')
            return ExpectingH;
        fail_if_invalid(c);
        return InURI;
    }

    InternalState _expectingH(char c) noexcept
    {
        addChar(c);
        if (c == 'H')
            return ExpectingFirstT;
        return Error;
    }

    InternalState _expectingFirstT(char c) noexcept
    {
        addChar(c);
        if (c == 'T')
            return ExpectingSecondT;
        return Error;
    }

    InternalState _expectingSecondT(char c) noexcept
    {
        addChar(c);
        if (c == 'T')
            return ExpectingP;
        return Error;
    }

    InternalState _expectingP(char c) noexcept
    {
        addChar(c);
        if (c == 'P')
            return ExpectingSlash;
        return Error;
    }

    InternalState _expectingSlash(char c) noexcept
    {
        addChar(c);
        if (c == '/')
            return MajorStart;
        return Error;
    }

    InternalState _majorStart(char c) noexcept
    {
        addChar(c);
        if (isdigit(c))
            return InMajor;
        return Error;
    }

    InternalState _inMajor(char c) noexcept
    {
        addChar(c);
        if (c == '.')
            return MinorStart;
        if (isdigit(c))
            return InMajor;
        return Error;
    }

    InternalState _minorStart(char c) noexcept
    {
        addChar(c);
        if (isdigit(c))
            return InMinor;
        return Error;
    }

    InternalState _inMinor(char c) noexcept
    {
        addChar(c);
        if (c == '\r')
            return ExpectingEndOfRequestLine;
        if (!isdigit(c))
            return Error;
        return InMinor;
    }

    InternalState _expectingEndOfRequestLine(char c) noexcept
    {
        addChar(c);
        if (c == '\n')
            return HeaderLineStart;
        return Error;
    }

    InternalState _headerLineStart(char c) noexcept
    {
        addChar(c);
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
        addChar(c);
        if (c == ':')
            return ExpectingSpaceBeforeHeaderValue;
        fail_if_special(c);
        fail_if_invalid(c);
        _curField.key.push_back(c);
        return InHeaderFieldKey;
    }

    InternalState _expectingSpaceBeforeHeaderValue(char c) noexcept
    {
        addChar(c);
        if (c == ' ')
            return InHeaderFieldValue;
        return Error;
    }

    InternalState _inHeaderFieldValue(char c) noexcept
    {
        addChar(c);
        if (c == '\r')
            return ExpectingEndOfFieldLine;
        fail_if_invalid(c);
        _curField.value.push_back(c);
        return InHeaderFieldValue;
    }

    InternalState _expectingEndOfFieldLine(char c) noexcept
    {
        addChar(c);
        if (c == '\n') {
            if (_curField.key == "Content-Length")
                _contentLength = _curField.value;
            _curField.key.clear();
            _curField.value.clear();
            return HeaderLineStart;
        }
        return Error;
    }

    InternalState _expectingEndOfHeader(char c) noexcept
    {
        addChar(c);
        if (c == '\n')
            return Success;
        return Error;
    }
};

#endif //ZIA_HTTPLENGTHEXTRACTOR_HPP
