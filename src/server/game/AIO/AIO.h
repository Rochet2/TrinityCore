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

#ifndef TRINITY_AIO_H
#define TRINITY_AIO_H

#include "Define.h"

#define AIO_VERSION 1.75
#define AIO_VERSION_STRING "1.75"

// RBAC permission id used when distributing addons/messages to players (see sql/CAIO/Auth.sql)
constexpr uint32 AIO_DEFAULT_ADDON_PERMISSION = 195;
// WoW addon whisper payload limit (matches client AIO.lua when AIO_SERVER is false)
constexpr uint32 AIO_MAX_WHISPER_LENGTH = 255;

#endif
