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
