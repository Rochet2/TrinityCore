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

#include "WorldAIO.h"
#include "AIOMsg.h"
#include "Player.h"
#include "World.h"
#include "WorldSession.h"

namespace Trinity::AIO
{
void MessageAll(World* world, AIOMsg& msg, uint32 permission)
{
    if (!world)
        return;

    std::string const messageStr = msg.dumps();
    for (SessionMap::const_iterator itr = world->GetAllSessions().begin(); itr != world->GetAllSessions().end(); ++itr)
    {
        if (itr->second->GetPlayer() && itr->second->HasPermission(permission))
            itr->second->GetPlayer()->SendSimpleAIOMessage(messageStr);
    }
}

void SendAllSimple(World* world, std::string const& message, uint32 permission)
{
    if (!world)
        return;

    for (SessionMap::const_iterator itr = world->GetAllSessions().begin(); itr != world->GetAllSessions().end(); ++itr)
    {
        if (itr->second->GetPlayer() && itr->second->HasPermission(permission))
            itr->second->GetPlayer()->SendSimpleAIOMessage(message);
    }
}
}
