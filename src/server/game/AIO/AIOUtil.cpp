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

#include "AIOUtil.h"

#include <exception>

namespace Trinity::AIO
{
bool IsSafeAddonRelativePath(std::string const& path)
{
    if (path.empty() || path.front() == '/' || path.front() == '\\')
        return false;

    if (path.find("..") != std::string::npos)
        return false;

    return path.find_first_of(":*?\"<>|") == std::string::npos;
}

LoadMessageOutcome TryLoadIncomingMessage(std::string const& message, uint32 maxBytes, uint32 maxBlocks)
{
    LoadMessageOutcome outcome;

    if (message.size() > maxBytes)
    {
        outcome.result = LoadMessageResult::Oversize;
        return outcome;
    }

    try
    {
        outcome.table = LuaVal::loads(message);
    }
    catch (std::exception const&)
    {
        outcome.result = LoadMessageResult::ParseError;
        return outcome;
    }
    catch (...)
    {
        outcome.result = LoadMessageResult::ParseError;
        return outcome;
    }

    if (outcome.table.isnil())
    {
        outcome.result = LoadMessageResult::ParseError;
        return outcome;
    }

    if (!outcome.table.istable())
    {
        outcome.result = LoadMessageResult::NotTable;
        return outcome;
    }

    if (outcome.table.len() > maxBlocks)
    {
        outcome.result = LoadMessageResult::TooManyBlocks;
        return outcome;
    }

    outcome.result = LoadMessageResult::Ok;
    return outcome;
}
}
