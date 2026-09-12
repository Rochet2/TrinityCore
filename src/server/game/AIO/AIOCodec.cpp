#include "AIOCodec.h"

namespace Trinity::AIO::Codec
{
std::vector<std::string> SplitPayload(std::string const& payload, uint32 maxAfterTab, uint32 messageId)
{
    std::vector<std::string> packets;
    if (!maxAfterTab)
        return packets;

    if (2u + payload.size() <= size_t(maxAfterTab))
    {
        std::string packet;
        packet.reserve(2 + payload.size());
        packet.push_back(SHORT_TAG_HIGH);
        packet.push_back(SHORT_TAG_LOW);
        packet += payload;
        packets.push_back(std::move(packet));
        return packets;
    }

    if (maxAfterTab <= 6u)
        return packets;

    uint32 const chunkLen = maxAfterTab - 6u;
    uint32 const partsNeeded = uint32((payload.size() + chunkLen - 1) / chunkLen);
    if (partsNeeded < 2 || partsNeeded > MAX_ENCODED_UINT16 || messageId == 0 || messageId > MAX_ENCODED_UINT16)
        return packets;

    std::string const messageIdStr = EncodePair(messageId);
    std::string const partsStr = EncodePair(partsNeeded);
    if (messageIdStr.empty() || partsStr.empty())
        return packets;

    size_t cursor = 0;
    packets.reserve(partsNeeded);
    for (uint32 partId = 1; partId <= partsNeeded; ++partId)
    {
        std::string const partIdStr = EncodePair(partId);
        if (partIdStr.empty())
        {
            packets.clear();
            return packets;
        }

        std::string packet;
        packet.reserve(6 + chunkLen);
        packet += messageIdStr;
        packet += partsStr;
        packet += partIdStr;
        packet += payload.substr(cursor, chunkLen);
        packets.push_back(std::move(packet));
        cursor += chunkLen;
    }

    return packets;
}

ParsedFrame ParseAfterTab(std::string_view body)
{
    ParsedFrame frame;
    if (body.size() < 2)
        return frame;

    uint32 const messageId = DecodePair(body[0], body[1]);
    if (messageId == 0)
    {
        frame.kind = FrameKind::Short;
        frame.shortPayload = std::string(body.substr(2));
        return frame;
    }

    if (body.size() < 6)
        return frame;

    frame.messageId = messageId;
    frame.parts = DecodePair(body[2], body[3]);
    frame.partId = DecodePair(body[4], body[5]);
    frame.partPayload = std::string(body.substr(6));
    frame.kind = FrameKind::LongPart;
    return frame;
}
}
