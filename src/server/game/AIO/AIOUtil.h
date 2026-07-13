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

#ifndef TRINITY_AIO_UTIL_H
#define TRINITY_AIO_UTIL_H

#include "Define.h"
#include "smallfolk.h"
#include <string>

namespace Trinity::AIO
{
    constexpr uint32 MAX_BLOCK_ARGS = 15;

    TC_GAME_API bool IsSafeAddonRelativePath(std::string const& path);

    enum class LoadMessageResult : uint8
    {
        Ok,
        Oversize,
        ParseError,
        NotTable,
        TooManyBlocks
    };

    struct LoadMessageOutcome
    {
        LoadMessageResult result = LoadMessageResult::ParseError;
        LuaVal table;
    };

    TC_GAME_API LoadMessageOutcome TryLoadIncomingMessage(std::string const& message, uint32 maxBytes, uint32 maxBlocks);
}

#endif
