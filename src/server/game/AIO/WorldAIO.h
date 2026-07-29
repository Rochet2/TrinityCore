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

#ifndef TRINITY_WORLD_AIO_H
#define TRINITY_WORLD_AIO_H

#include "AIO.h"
#include <string>

class AIOMsg;
class World;

namespace Trinity::AIO
{
    TC_GAME_API void MessageAll(World* world, AIOMsg& msg, uint32 permission = AIO_DEFAULT_ADDON_PERMISSION);
    TC_GAME_API void SendAllSimple(World* world, std::string const& message, uint32 permission = AIO_DEFAULT_ADDON_PERMISSION);
}

#endif
