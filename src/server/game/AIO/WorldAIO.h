
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
