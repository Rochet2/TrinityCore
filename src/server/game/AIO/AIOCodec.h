/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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

    inline bool IsClientPrefix(std::string const& prefix, std::string const& msg, size_t& delimPosOut)
    {
        delimPosOut = msg.find('\t');
        if (delimPosOut == std::string::npos)
            return false;

        return msg.compare(0, delimPosOut, "C" + prefix) == 0;
    }
}

#endif
