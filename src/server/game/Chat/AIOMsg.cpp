#include "AIOMsg.h"
#include "AIOUtil.h"
#include "Player.h"
#include <cmath>

namespace
{
uint32 CountPackedArgs(LuaVal const& a1, LuaVal const& a2, LuaVal const& a3, LuaVal const& a4, LuaVal const& a5, LuaVal const& a6)
{
    LuaVal const args[6] = { a1, a2, a3, a4, a5, a6 };
    int last = -1;
    for (int i = 0; i < 6; ++i)
    {
        if (!args[i].isnil())
            last = i;
    }
    return last < 0 ? 0u : uint32(last + 1);
}

void AppendPackedArgs(LuaVal& block, uint32& nArgs, LuaVal const& a1, LuaVal const& a2, LuaVal const& a3, LuaVal const& a4, LuaVal const& a5, LuaVal const& a6)
{
    LuaVal const args[6] = { a1, a2, a3, a4, a5, a6 };
    uint32 const count = CountPackedArgs(a1, a2, a3, a4, a5, a6);
    for (uint32 i = 0; i < count; ++i)
    {
        if (nArgs >= Trinity::AIO::MAX_BLOCK_ARGS)
            break;
        // Layout: [1]=n, [2]=script, [3]=handler, [4+]=args. nArgs counts from handlerKey.
        // Assign by index (not insert): smallfolk insert(nil) is a no-op and drops middle nils.
        unsigned int const index = 3u + nArgs;
        if (!args[i].isnil())
            block[index] = args[i];
        ++nArgs;
    }
}
}

AIOMsg::AIOMsg()
    : _val(TTABLE)
{ }

AIOMsg& AIOMsg::Add(LuaVal const& scriptKey, LuaVal const& handlerKey, LuaVal const& a1, LuaVal const& a2, LuaVal const& a3, LuaVal const& a4, LuaVal const& a5, LuaVal const& a6)
{
    LuaVal block(TTABLE);
    // n includes handlerKey (stock AIO layout); start at 1 then pack positional args including middle nils.
    uint32 nArgs = 1;

    block[1] = 0;
    block[2] = scriptKey;
    block[3] = handlerKey;

    AppendPackedArgs(block, nArgs, a1, a2, a3, a4, a5, a6);

    block[1] = static_cast<unsigned int>(nArgs);
    _val.insert(block);
    return *this;
}

AIOMsg& AIOMsg::AppendLast(LuaVal const& a1, LuaVal const& a2, LuaVal const& a3, LuaVal const& a4, LuaVal const& a5, LuaVal const& a6)
{
    unsigned int lastBlock = _val.len();
    if (!lastBlock)
        return *this;

    LuaVal& block = _val.at(static_cast<int>(lastBlock));
    LuaVal nArgsVal = block.get(1);
    if (!nArgsVal.isnumber())
        return *this;

    double const n = nArgsVal.num();
    if (!std::isfinite(n) || n < 1.0 || n != std::floor(n))
        return *this;

    uint32 nArgs = static_cast<uint32>(n);
    AppendPackedArgs(block, nArgs, a1, a2, a3, a4, a5, a6);

    block[1] = static_cast<unsigned int>(nArgs);
    return *this;
}
