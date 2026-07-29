#ifndef AIO_MESSAGE_H
#define AIO_MESSAGE_H

#include "smallfolk.h"

class Player;

class AIOMsg
{
    public:
        // Creates an empty AIOMsg
        AIOMsg();

        // Creates an AIO message and adds one block
        AIOMsg(LuaVal const& scriptKey, LuaVal const& handlerKey,
            LuaVal const& a1 = LuaVal::nil, LuaVal const& a2 = LuaVal::nil, LuaVal const& a3 = LuaVal::nil,
            LuaVal const& a4 = LuaVal::nil, LuaVal const& a5 = LuaVal::nil, LuaVal const& a6 = LuaVal::nil)
            : _val(TTABLE)
        {
            Add(scriptKey, handlerKey, a1, a2, a3, a4, a5, a6);
        }

        AIOMsg(char const* scriptKey, char const* handlerKey,
            LuaVal const& a1 = LuaVal::nil, LuaVal const& a2 = LuaVal::nil, LuaVal const& a3 = LuaVal::nil,
            LuaVal const& a4 = LuaVal::nil, LuaVal const& a5 = LuaVal::nil, LuaVal const& a6 = LuaVal::nil)
            : _val(TTABLE)
        {
            Add(LuaVal(scriptKey), LuaVal(handlerKey), a1, a2, a3, a4, a5, a6);
        }

        // Adds another block (calls another handler in one message)
        AIOMsg& Add(LuaVal const& scriptKey, LuaVal const& handlerKey,
            LuaVal const& a1 = LuaVal::nil, LuaVal const& a2 = LuaVal::nil, LuaVal const& a3 = LuaVal::nil,
            LuaVal const& a4 = LuaVal::nil, LuaVal const& a5 = LuaVal::nil, LuaVal const& a6 = LuaVal::nil);

        AIOMsg& Add(char const* scriptKey, char const* handlerKey,
            LuaVal const& a1 = LuaVal::nil, LuaVal const& a2 = LuaVal::nil, LuaVal const& a3 = LuaVal::nil,
            LuaVal const& a4 = LuaVal::nil, LuaVal const& a5 = LuaVal::nil, LuaVal const& a6 = LuaVal::nil)
        {
            return Add(LuaVal(scriptKey), LuaVal(handlerKey), a1, a2, a3, a4, a5, a6);
        }

        // Appends arguments to the last block
        AIOMsg& AppendLast(LuaVal const& a1 = LuaVal::nil, LuaVal const& a2 = LuaVal::nil, LuaVal const& a3 = LuaVal::nil,
            LuaVal const& a4 = LuaVal::nil, LuaVal const& a5 = LuaVal::nil, LuaVal const& a6 = LuaVal::nil);

        // Returns smallfolk dump of the AIO message
        std::string dumps() const { return _val.dumps(); }

    private:
        LuaVal _val;
        friend class Player;
};

#endif
