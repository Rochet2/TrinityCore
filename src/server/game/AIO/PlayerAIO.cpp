
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
