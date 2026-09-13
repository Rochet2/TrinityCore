#ifndef TRINITY_AIO_CODEC_H
#define TRINITY_AIO_CODEC_H

#include "Define.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace Trinity::AIO::Codec
{
    // Matches AIO.lua / aio_framing.lua: usable uint16 range without NUL bytes.
    constexpr uint32 MAX_ENCODED_UINT16 = 64769u; // 2^16 - 767

    inline constexpr char SHORT_TAG_HIGH = '\x01';
    inline constexpr char SHORT_TAG_LOW = '\x01';

    inline uint32 DecodeByte(char value)
    {
        return uint32(static_cast<uint8>(value) - 1u);
    }

    inline uint32 DecodePair(char high, char low)
    {
        return DecodeByte(high) * 254u + DecodeByte(low);
    }

    inline bool TryEncodePair(uint32 value, char& high, char& low)
    {
        if (value > MAX_ENCODED_UINT16)
            return false;

        uint32 const hi = value / 254u;
        high = char(hi + 1u);
        low = char(value - hi * 254u + 1u);
        return true;
    }

    inline std::string EncodePair(uint32 value)
    {
        char high = 0;
        char low = 0;
        if (!TryEncodePair(value, high, low))
            return {};
        std::string out(1, high);
        out += low;
        return out;
    }

    inline bool IsClientPrefix(std::string const& clientWirePrefix, std::string const& msg, size_t& delimPosOut)
    {
        delimPosOut = msg.find('\t');
        if (delimPosOut == std::string::npos)
            return false;

        return delimPosOut == clientWirePrefix.size() && msg.compare(0, delimPosOut, clientWirePrefix) == 0;
    }

    // maxAfterTab: max bytes after the wire prefix tab (short tag + payload, or long meta + chunk).
    TC_GAME_API std::vector<std::string> SplitPayload(std::string const& payload, uint32 maxAfterTab, uint32 messageId);

    enum class FrameKind : uint8
    {
        Invalid,
        Short,
        LongPart
    };

    struct ParsedFrame
    {
        FrameKind kind = FrameKind::Invalid;
        std::string shortPayload;
        uint32 messageId = 0;
        uint32 parts = 0;
        uint32 partId = 0;
        std::string partPayload;
    };

    // body = bytes after the client/server prefix tab.
    TC_GAME_API ParsedFrame ParseAfterTab(std::string_view body);
}

#endif
