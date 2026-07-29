#include "tc_catch2.h"

#include "AIOCodec.h"

using namespace Trinity::AIO::Codec;

TEST_CASE("AIO codec byte decoding", "[AIO]")
{
    REQUIRE(DecodeByte('\x01') == 0);
    REQUIRE(DecodeByte('\xFF') == 254);
    REQUIRE(DecodePair('\x01', '\x01') == 0);
    REQUIRE(DecodePair('\x02', '\x02') == 255);
}

TEST_CASE("AIO client prefix detection", "[AIO]")
{
    SECTION("matches configured wire prefix")
    {
        size_t delimPos = 0;
        REQUIRE(IsClientPrefix("CAIO", "CAIO\tpayload", delimPos));
        REQUIRE(delimPos == 4);
    }

    SECTION("rejects server prefix")
    {
        size_t delimPos = 0;
        REQUIRE_FALSE(IsClientPrefix("CAIO", "SAIO\tpayload", delimPos));
    }

    SECTION("rejects missing tab")
    {
        size_t delimPos = 0;
        REQUIRE_FALSE(IsClientPrefix("AIO", "CAIOpayload", delimPos));
    }
}
