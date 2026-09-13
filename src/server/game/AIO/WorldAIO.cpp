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
