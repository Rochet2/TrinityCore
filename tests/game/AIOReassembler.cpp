#include "tc_catch2.h"

#include "AIOCodec.h"
#include "AIOReassembler.h"

#include <string>
#include <vector>

using namespace Trinity::AIO;
using namespace Trinity::AIO::Codec;

namespace
{
uint32 const kMsgLen = 200;

std::vector<std::string> SplitLong(std::string const& payload, uint32 messageId)
{
    return SplitPayload(payload, kMsgLen, messageId);
}
}

TEST_CASE("AIO reassembler short message ingest", "[AIO]")
{
    Reassembler r;
    std::vector<std::string> const packets = SplitPayload("hello", kMsgLen, 1);
    REQUIRE(packets.size() == 1);

    ParsedFrame const frame = ParseAfterTab(packets[0]);
    REQUIRE(frame.kind == FrameKind::Short);
    // Short messages bypass the reassembler in WorldSession; framing still round-trips.
    REQUIRE(frame.shortPayload == "hello");
}

TEST_CASE("AIO reassembler long message ingest", "[AIO]")
{
    Reassembler r;
    std::string const payload(kMsgLen + 50, 'z');
    std::vector<std::string> const packets = SplitLong(payload, 7);
    REQUIRE(packets.size() > 1);

    std::string assembled;
    for (std::string const& packet : packets)
    {
        ParsedFrame const frame = ParseAfterTab(packet);
        REQUIRE(frame.kind == FrameKind::LongPart);
        Reassembler::IngestResult const result = r.Ingest(frame.messageId, frame.parts, frame.partId, frame.partPayload, 64, 1048576);
        REQUIRE(result.error == Reassembler::IngestError::None);
        if (result.complete)
            assembled = result.payload;
    }

    REQUIRE(assembled == payload);
    REQUIRE(r.Size() == 0);
    REQUIRE(r.GetBufferedBytes(7) == 0);
}

TEST_CASE("AIO reassembler hole safe until complete", "[AIO]")
{
    Reassembler r;
    std::string const payload(kMsgLen + 50, 'a');
    std::vector<std::string> const packets = SplitLong(payload, 3);
    REQUIRE(packets.size() >= 2);

    ParsedFrame const first = ParseAfterTab(packets[0]);
    ParsedFrame const last = ParseAfterTab(packets.back());

    Reassembler::IngestResult const midMissing = r.Ingest(last.messageId, last.parts, last.partId, last.partPayload, 64, 1048576);
    REQUIRE_FALSE(midMissing.complete);
    REQUIRE(r.GetBufferedBytes(3) > 0);

    // Feed remaining parts except we already have last; feed from start skipping last until done.
    std::string assembled;
    for (size_t i = 0; i + 1 < packets.size(); ++i)
    {
        ParsedFrame const frame = ParseAfterTab(packets[i]);
        Reassembler::IngestResult const result = r.Ingest(frame.messageId, frame.parts, frame.partId, frame.partPayload, 64, 1048576);
        REQUIRE(result.error == Reassembler::IngestError::None);
        if (result.complete)
            assembled = result.payload;
    }

    REQUIRE(assembled == payload);
    REQUIRE(r.GetBufferedBytes(3) == 0);
}

TEST_CASE("AIO reassembler remove clears stored bytes", "[AIO]")
{
    Reassembler r;
    std::string const payload(kMsgLen + 50, 'z');
    std::vector<std::string> const packets = SplitLong(payload, 7);
    REQUIRE(packets.size() > 1);

    ParsedFrame const frame = ParseAfterTab(packets[0]);
    REQUIRE_FALSE(r.Ingest(frame.messageId, frame.parts, frame.partId, frame.partPayload, 64, 1048576).complete);
    REQUIRE(r.GetBufferedBytes(7) > 0);

    r.Remove(7);
    REQUIRE(r.GetBufferedBytes(7) == 0);
    REQUIRE(r.Size() == 0);
}

TEST_CASE("AIO reassembler completing clears stored bytes", "[AIO]")
{
    Reassembler r;
    std::string const payload(kMsgLen + 50, 'a');
    std::vector<std::string> const packets = SplitLong(payload, 3);

    std::string assembled;
    for (size_t i = 0; i < packets.size(); ++i)
    {
        ParsedFrame const frame = ParseAfterTab(packets[i]);
        Reassembler::IngestResult const result = r.Ingest(frame.messageId, frame.parts, frame.partId, frame.partPayload, 64, 1048576);
        if (i + 1 < packets.size())
            REQUIRE(r.GetBufferedBytes(3) > 0);
        if (result.complete)
            assembled = result.payload;
    }

    REQUIRE(assembled == payload);
    REQUIRE(r.GetBufferedBytes(3) == 0);
}

TEST_CASE("AIO reassembler ttl sweep expires incomplete", "[AIO]")
{
    Reassembler r;
    std::string const payload(kMsgLen + 10, 'x');
    std::vector<std::string> const packets = SplitLong(payload, 9);
    ParsedFrame const frame = ParseAfterTab(packets[0]);
    REQUIRE_FALSE(r.Ingest(frame.messageId, frame.parts, frame.partId, frame.partPayload, 64, 1048576).complete);

    REQUIRE(r.Sweep(5000, 15000) == 0);
    REQUIRE(r.Size() == 1);
    REQUIRE(r.Sweep(5000, 15000) == 0);
    REQUIRE(r.Sweep(5000, 15000) == 1);
    REQUIRE(r.Size() == 0);
}

TEST_CASE("AIO reassembler duplicate part rejected", "[AIO]")
{
    Reassembler r;
    std::string const payload(kMsgLen + 10, 'x');
    std::vector<std::string> const packets = SplitLong(payload, 1);
    ParsedFrame const frame = ParseAfterTab(packets[0]);

    REQUIRE_FALSE(r.Ingest(frame.messageId, frame.parts, frame.partId, frame.partPayload, 64, 1048576).complete);
    Reassembler::IngestResult const dup = r.Ingest(frame.messageId, frame.parts, frame.partId, frame.partPayload, 64, 1048576);
    REQUIRE(dup.error == Reassembler::IngestError::DuplicatePart);
}
