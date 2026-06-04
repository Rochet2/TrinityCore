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

#include "PlayerAIO.h"
#include "Player.h"

namespace Trinity::AIO
{
void Message(Player* player, AIOMsg& msg)
{
    if (player)
        player->SendSimpleAIOMessage(msg.dumps());
}

void Handle(Player* player, LuaVal const& scriptKey, LuaVal const& handlerKey,
    LuaVal const& a1, LuaVal const& a2, LuaVal const& a3,
    LuaVal const& a4, LuaVal const& a5, LuaVal const& a6)
{
    if (!player)
        return;

    AIOMsg msg(scriptKey, handlerKey, a1, a2, a3, a4, a5, a6);
    player->SendSimpleAIOMessage(msg.dumps());
}

void Handle(Player* player, char const* scriptKey, char const* handlerKey,
    LuaVal const& a1, LuaVal const& a2, LuaVal const& a3,
    LuaVal const& a4, LuaVal const& a5, LuaVal const& a6)
{
    Handle(player, LuaVal(scriptKey), LuaVal(handlerKey), a1, a2, a3, a4, a5, a6);
}
}
