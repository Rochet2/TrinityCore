#ifndef TRINITY_AIO_CODEC_H
#define TRINITY_AIO_CODEC_H

#include "Define.h"
#include <cstddef>
#include <cstdint>
#include <string>

namespace Trinity::AIO::Codec
{
    inline uint32 DecodeByte(char value)
    {
        return uint32(static_cast<uint8>(value) - 1u);
    }

    inline uint32 DecodePair(char high, char low)
    {
        return DecodeByte(high) * 254u + DecodeByte(low);
    }

    inline bool IsClientPrefix(std::string const& clientWirePrefix, std::string const& msg, size_t& delimPosOut)
    {
        delimPosOut = msg.find('\t');
        if (delimPosOut == std::string::npos)
            return false;

        return delimPosOut == clientWirePrefix.size() && msg.compare(0, delimPosOut, clientWirePrefix) == 0;
    }
}

#endif
