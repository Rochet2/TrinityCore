
#ifndef TRINITY_PLAYER_AIO_H
#define TRINITY_PLAYER_AIO_H

#include "AIOMsg.h"
#include "smallfolk.h"

class Player;

namespace Trinity::AIO
{
    TC_GAME_API void Message(Player* player, AIOMsg& msg);
    TC_GAME_API void Handle(Player* player, LuaVal const& scriptKey, LuaVal const& handlerKey,
        LuaVal const& a1 = LuaVal::nil, LuaVal const& a2 = LuaVal::nil, LuaVal const& a3 = LuaVal::nil,
        LuaVal const& a4 = LuaVal::nil, LuaVal const& a5 = LuaVal::nil, LuaVal const& a6 = LuaVal::nil);

    TC_GAME_API void Handle(Player* player, char const* scriptKey, char const* handlerKey,
        LuaVal const& a1 = LuaVal::nil, LuaVal const& a2 = LuaVal::nil, LuaVal const& a3 = LuaVal::nil,
        LuaVal const& a4 = LuaVal::nil, LuaVal const& a5 = LuaVal::nil, LuaVal const& a6 = LuaVal::nil);
}

#endif
