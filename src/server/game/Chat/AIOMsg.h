#ifndef AIO_MESSAGE_H
#define AIO_MESSAGE_H

#include "smallfolk.h"

class Player;

class AIOMsg
{
    public:
        //Creates an empty AIOMsg
        AIOMsg();

        //Creates a AIO message and adds one block
        AIOMsg(const LuaVal &scriptKey, const LuaVal &handlerKey,
            const LuaVal &a1 = LuaVal::nil, const LuaVal &a2 = LuaVal::nil, const LuaVal &a3 = LuaVal::nil,
            const LuaVal &a4 = LuaVal::nil, const LuaVal &a5 = LuaVal::nil, const LuaVal &a6 = LuaVal::nil)
            : _val(TTABLE)
        {
            Add(scriptKey, handlerKey, a1, a2, a3, a4, a5, a6);
        }

        AIOMsg(const char* scriptKey, const char* handlerKey,
            const LuaVal &a1 = LuaVal::nil, const LuaVal &a2 = LuaVal::nil, const LuaVal &a3 = LuaVal::nil,
            const LuaVal &a4 = LuaVal::nil, const LuaVal &a5 = LuaVal::nil, const LuaVal &a6 = LuaVal::nil)
            : _val(TTABLE)
        {
            Add(LuaVal(scriptKey), LuaVal(handlerKey), a1, a2, a3, a4, a5, a6);
        }

        //Adds another block
        //Another block will call another handler in one message
        AIOMsg &Add(const LuaVal &scriptKey, const LuaVal &handlerKey,
            const LuaVal &a1 = LuaVal::nil, const LuaVal &a2 = LuaVal::nil, const LuaVal &a3 = LuaVal::nil,
            const LuaVal &a4 = LuaVal::nil, const LuaVal &a5 = LuaVal::nil, const LuaVal &a6 = LuaVal::nil);

        AIOMsg &Add(const char* scriptKey, const char* handlerKey,
            const LuaVal &a1 = LuaVal::nil, const LuaVal &a2 = LuaVal::nil, const LuaVal &a3 = LuaVal::nil,
            const LuaVal &a4 = LuaVal::nil, const LuaVal &a5 = LuaVal::nil, const LuaVal &a6 = LuaVal::nil)
        {
            return Add(LuaVal(scriptKey), LuaVal(handlerKey), a1, a2, a3, a4, a5, a6);
        }

        //Appends the last block
        //You can add additional arguments to the last block
        AIOMsg &AppendLast(const LuaVal &a1 = LuaVal::nil, const LuaVal &a2 = LuaVal::nil, const LuaVal &a3 = LuaVal::nil,
            const LuaVal &a4 = LuaVal::nil, const LuaVal &a5 = LuaVal::nil, const LuaVal &a6 = LuaVal::nil);

        //Returns smallfolk dump of the AIO message
        std::string dumps() const { return _val.dumps(); }

    private:
        LuaVal _val;
        friend class Player;
};

#endif
