#include "AIOMsg.h"
#include "Player.h"

AIOMsg::AIOMsg()
    : _val(TTABLE)
{ }

AIOMsg& AIOMsg::Add(LuaVal const& scriptKey, LuaVal const& handlerKey, LuaVal const& a1, LuaVal const& a2, LuaVal const& a3, LuaVal const& a4, LuaVal const& a5, LuaVal const& a6)
{
    LuaVal block(TTABLE);
    uint32 nArgs = 1;

    block[1] = 0;
    block[2] = scriptKey;
    block[3] = handlerKey;

    if (!a1.isnil())
    {
        block.insert(a1);
        ++nArgs;
    }
    if (!a2.isnil())
    {
        block.insert(a2);
        ++nArgs;
    }
    if (!a3.isnil())
    {
        block.insert(a3);
        ++nArgs;
    }
    if (!a4.isnil())
    {
        block.insert(a4);
        ++nArgs;
    }
    if (!a5.isnil())
    {
        block.insert(a5);
        ++nArgs;
    }
    if (!a6.isnil())
    {
        block.insert(a6);
        ++nArgs;
    }

    block[1] = nArgs;
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

    unsigned int nArgs = static_cast<unsigned int>(nArgsVal.num());
    if (!a1.isnil())
    {
        block.insert(a1);
        ++nArgs;
    }
    if (!a2.isnil())
    {
        block.insert(a2);
        ++nArgs;
    }
    if (!a3.isnil())
    {
        block.insert(a3);
        ++nArgs;
    }
    if (!a4.isnil())
    {
        block.insert(a4);
        ++nArgs;
    }
    if (!a5.isnil())
    {
        block.insert(a5);
        ++nArgs;
    }
    if (!a6.isnil())
    {
        block.insert(a6);
        ++nArgs;
    }

    block[1] = nArgs;
    return *this;
}
