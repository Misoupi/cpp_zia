//
// Created by doom on 12/02/18
//

#include <cstddef>
#include <gtest/gtest.h>
#include <api/net.h>
#include <utils/BufferUtils.hpp>
#include <HTTPLengthExtractor.hpp>

TEST(HTTPLengthExtractor, Basic)
{
    HTTPLengthExtractor parser;
    const zia::api::Net::Raw raw = utils::stringToRaw("GET /docs/index.html HTTP/1.1\r\n"
                                                          "Host: www.nowhere123.com\r\n"
                                                          "Accept: image/gif, image/jpeg, */*\r\n"
                                                          "Accept-Language: en-us\r\n"
                                                          "Accept-Encoding: gzip, deflate\r\n"
                                                          "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
                                                          "\r\n");

    auto pair = parser.feed(raw.begin(), raw.begin() + raw.size() - 1);
    ASSERT_EQ(pair.first, HTTPLengthExtractor::Result::InProgress);

    auto pair2 = parser.feed(raw.begin() + raw.size() - 1, raw.end());
    ASSERT_EQ(pair2.first, HTTPLengthExtractor::Result::ParseSuccess);

    ASSERT_EQ(raw, parser.buffer());
}

TEST(HTTPLengthExtractor, Errors)
{
    HTTPLengthExtractor parser;

    const zia::api::Net::Raw raw = utils::stringToRaw("GET /lalala HTTP/1.1\r");
    auto pair = parser.feed(raw.begin(), raw.end());
    ASSERT_EQ(pair.first, HTTPLengthExtractor::Result::InProgress);

    parser.restart();
    const zia::api::Net::Raw raw2 = utils::stringToRaw("GET /lalala HTTP/1.1\rlala");
    auto pair2 = parser.feed(raw2.begin(), raw2.end());
    ASSERT_EQ(pair2.first, HTTPLengthExtractor::Result::ParseError);

    parser.restart();
    const zia::api::Net::Raw raw3 = utils::stringToRaw("GET /lalala HTTTTTP/1.1\r\n");
    auto pair3 = parser.feed(raw3.begin(), raw3.end());
    ASSERT_EQ(pair3.first, HTTPLengthExtractor::Result::ParseError);

    parser.restart();
    const zia::api::Net::Raw raw4 = utils::stringToRaw("G(ET /lalala HTTP/1.1\r\n");
    auto pair4 = parser.feed(raw4.begin(), raw4.end());
    ASSERT_EQ(pair4.first, HTTPLengthExtractor::Result::ParseError);
}
