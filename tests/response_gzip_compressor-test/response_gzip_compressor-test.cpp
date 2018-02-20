//
// Created by doom on 20/02/18.
//

#include <gtest/gtest.h>
#include <core/lib/Lib.hpp>
#include <api/module.h>

//Since the original class cannot be seen from outside the module, here is a reproduction for tests only :/
namespace
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

    static bool acceptsGZip(const std::string &value)
    {
        std::istringstream iss(value);

        iss.imbue(std::locale(std::locale(), new CommaSplittingLocale));
        std::string str;
        while (iss >> str) {
            if (str == "gzip")
                return true;
        }
        return false;
    }
}

TEST(ResponseGZipCompressor, AcceptEncodingParsing)
{
    ASSERT_TRUE(acceptsGZip("gzip, compress, br"));
    ASSERT_TRUE(acceptsGZip("compress, gzip, br"));
    ASSERT_TRUE(acceptsGZip("compress, br, gzip"));
    ASSERT_TRUE(acceptsGZip("gzip"));
    ASSERT_FALSE(acceptsGZip("compress, br"));
    ASSERT_FALSE(acceptsGZip("br"));
}
