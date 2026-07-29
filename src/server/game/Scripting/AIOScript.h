#ifndef TRINITY_AIOSCRIPT_H
#define TRINITY_AIOSCRIPT_H

// Included from ScriptMgr.h after ScriptObject is defined.
#include "AIO.h"
#include "Define.h"
#include "smallfolk.h"
#include <functional>
#include <unordered_map>

class Player;

// Inherit AIOScript for server-side AIO handlers (LuaVal / smallfolk_cpp).
// Full example: doc/CAIO_SCRIPT_EXAMPLE.md
class TC_GAME_API AIOScript : public ScriptObject
{
    public:
        ~AIOScript() override;

        LuaVal GetKey() const { return _key; }
        bool IsDatabaseBound() const { return false; }

        static AIOScript* FindByKey(LuaVal const& scriptKey);
        static void ClearScriptByKeyMap();

        void HandleAddonBlock(Player* sender, LuaVal const& handlerKey, LuaVal const& args);
        static void DispatchIncomingBlocks(Player* sender, LuaVal const& mainTable);

        using HandlerFunc = std::function<void(Player*, LuaVal const&)>;
        using ArgFunc = std::function<LuaVal(Player*)>;
        using InitMessageFunc = std::function<void(Player*, LuaVal&)>;

    protected:
        AIOScript(LuaVal const& scriptKey);

        void AddHandler(LuaVal const& handlerKey, HandlerFunc function) { _handlerMap[handlerKey] = std::move(function); }

        void AddInitArgs(LuaVal const& scriptKey, LuaVal const& handlerKey,
            ArgFunc a1 = ArgFunc(), ArgFunc a2 = ArgFunc(), ArgFunc a3 = ArgFunc(),
            ArgFunc a4 = ArgFunc(), ArgFunc a5 = ArgFunc(), ArgFunc a6 = ArgFunc());

        void AddOnInit(InitMessageFunc func);

        bool AddAddon(std::string const& addonName, std::string const& addonFile, uint32 permission = AIO_DEFAULT_ADDON_PERMISSION);

        template<class ScriptClass>
        ScriptClass* GetScript(LuaVal const& scriptKey);

    private:
        void OnHandle(Player* sender, LuaVal const& handlerKey, LuaVal const& args);

        LuaVal _key;

        using HandlerMapType = std::unordered_map<LuaVal, HandlerFunc, LuaVal::LuaValHasher>;
        HandlerMapType _handlerMap;

        using AIOScriptByKeyMap = std::unordered_map<LuaVal, AIOScript*, LuaVal::LuaValHasher>;
        static AIOScriptByKeyMap _scriptByKeyMap;
};

template<class ScriptClass>
ScriptClass* AIOScript::GetScript(LuaVal const& scriptKey)
{
    return dynamic_cast<ScriptClass*>(FindByKey(scriptKey));
}

class AIOHandlers;

TC_GAME_API AIOHandlers* CreateAIOHandlers();
TC_GAME_API void DestroyAIOHandlers(AIOHandlers* handlers);
TC_GAME_API void RegisterAIOInitHookOnHandlers(AIOHandlers* handlers, AIOScript::InitMessageFunc func);
TC_GAME_API void RegisterAIOInitArgsOnHandlers(AIOHandlers* handlers, LuaVal const& scriptKey, LuaVal const& handlerKey,
    AIOScript::ArgFunc a1, AIOScript::ArgFunc a2, AIOScript::ArgFunc a3,
    AIOScript::ArgFunc a4, AIOScript::ArgFunc a5, AIOScript::ArgFunc a6);

#endif
