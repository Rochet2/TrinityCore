#include "tc_catch2.h"

#include "AIOCodec.h"

#include <string>

using namespace Trinity::AIO::Codec;

TEST_CASE("AIO codec byte decoding", "[AIO]")
{
    REQUIRE(DecodeByte('\x01') == 0);
    REQUIRE(DecodeByte('\xFF') == 254);
    REQUIRE(DecodePair('\x01', '\x01') == 0);
    REQUIRE(DecodePair('\x02', '\x02') == 255);
}

TEST_CASE("AIO codec uint16 roundtrip", "[AIO]")
{
    for (uint32 value : { 0u, 1u, 255u, 1000u, MAX_ENCODED_UINT16 })
    {
        std::string const encoded = EncodePair(value);
        REQUIRE(encoded.size() == 2);
        REQUIRE(DecodePair(encoded[0], encoded[1]) == value);
    }

    char high = 0;
    char low = 0;
    REQUIRE_FALSE(TryEncodePair(MAX_ENCODED_UINT16 + 1, high, low));
    REQUIRE(EncodePair(MAX_ENCODED_UINT16 + 1).empty());
}

TEST_CASE("AIO framing short and long split", "[AIO]")
{
    uint32 const maxAfterTab = 200;

    SECTION("short message unchanged")
    {
        std::string const msg = "short payload";
        std::vector<std::string> const packets = SplitPayload(msg, maxAfterTab, 1);
        REQUIRE(packets.size() == 1);

        ParsedFrame const frame = ParseAfterTab(packets[0]);
        REQUIRE(frame.kind == FrameKind::Short);
        REQUIRE(frame.shortPayload == msg);
    }

    SECTION("long message split and reassemble")
    {
        std::string const msg(maxAfterTab + 100, 'x');
        std::vector<std::string> const packets = SplitPayload(msg, maxAfterTab, 42);
        REQUIRE(packets.size() > 1);

        std::string assembled;
        uint32 parts = 0;
        for (std::string const& packet : packets)
        {
            ParsedFrame const frame = ParseAfterTab(packet);
            REQUIRE(frame.kind == FrameKind::LongPart);
            REQUIRE(frame.messageId == 42u);
            if (!parts)
                parts = frame.parts;
            REQUIRE(frame.parts == parts);
            assembled += frame.partPayload;
        }
        REQUIRE(parts == packets.size());
        REQUIRE(assembled == msg);
    }

    SECTION("rejects too-short long body")
    {
        ParsedFrame const frame = ParseAfterTab(std::string("\x02\x02\x01"));
        REQUIRE(frame.kind == FrameKind::Invalid);
    }
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
