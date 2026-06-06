/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "AIOUtil.h"
#include "Log.h"
#include "Player.h"
#include "PlayerAIO.h"
#include "ScriptMgr.h"
#include "World.h"
#include <cmath>
#include <list>

class AIOHandlers : public AIOScript
{
    public:
        struct InitHookInfo
        {
            LuaVal scriptKey;
            LuaVal handlerKey;
            std::list<AIOScript::ArgFunc> argsList;

            InitHookInfo(LuaVal const& scriptKey, LuaVal const& handlerKey)
                : scriptKey(scriptKey), handlerKey(handlerKey)
            { }
        };

        using HookListType = std::list<InitHookInfo>;

        void RegisterInitMessageHook(AIOScript::InitMessageFunc func)
        {
            _initMessageHooks.push_back(std::move(func));
        }

        void RegisterInitArgs(LuaVal const& scriptKey, LuaVal const& handlerKey,
            AIOScript::ArgFunc a1, AIOScript::ArgFunc a2, AIOScript::ArgFunc a3,
            AIOScript::ArgFunc a4, AIOScript::ArgFunc a5, AIOScript::ArgFunc a6)
        {
            std::list<AIOScript::ArgFunc>* list = nullptr;
            for (HookListType::iterator itr = _initHookList.begin(); itr != _initHookList.end(); ++itr)
            {
                if (itr->scriptKey == scriptKey && itr->handlerKey == handlerKey)
                {
                    list = &itr->argsList;
                    break;
                }
            }

            if (!list)
            {
                _initHookList.push_back(InitHookInfo(scriptKey, handlerKey));
                list = &_initHookList.back().argsList;
            }

            if (a1)
                list->push_back(a1);
            if (a2)
                list->push_back(a2);
            if (a3)
                list->push_back(a3);
            if (a4)
                list->push_back(a4);
            if (a5)
                list->push_back(a5);
            if (a6)
                list->push_back(a6);
        }

    private:
        AIOHandlers();
        void HandleInit(Player* sender, LuaVal const& args);
        void HandleError(Player* sender, LuaVal const& args);

        HookListType _initHookList;
        std::list<AIOScript::InitMessageFunc> _initMessageHooks;
};

AIOScript::AIOScriptByKeyMap AIOScript::_scriptByKeyMap = AIOScript::AIOScriptByKeyMap();

AIOScript::~AIOScript()
{
    _scriptByKeyMap.erase(_key);
}

void AIOScript::AddOnInit(InitMessageFunc func)
{
    sScriptMgr->RegisterAIOInitHook(std::move(func));
}

void AIOScript::AddInitArgs(LuaVal const& scriptKey, LuaVal const& handlerKey, ArgFunc a1, ArgFunc a2, ArgFunc a3, ArgFunc a4, ArgFunc a5, ArgFunc a6)
{
    sScriptMgr->RegisterAIOInitArgs(scriptKey, handlerKey, std::move(a1), std::move(a2), std::move(a3), std::move(a4), std::move(a5), std::move(a6));
}

AIOScript::AIOScript(LuaVal const& scriptKey)
    : ScriptObject(scriptKey.tostring().c_str()), _key(scriptKey)
{
    if (AIOScript::_scriptByKeyMap.find(scriptKey) != AIOScript::_scriptByKeyMap.end())
    {
        sLog->outAIOMessage(0, LOG_LEVEL_FATAL, "AIO scriptKey '{}' of type tag '{}' already exist. Use another key.", scriptKey.tostring(), static_cast<int>(scriptKey.typetag()));
        ASSERT(false);
    }
    sScriptMgr->RegisterAIOScript(this);
    AIOScript::_scriptByKeyMap[scriptKey] = this;
}

void AIOScript::ClearScriptByKeyMap()
{
    _scriptByKeyMap.clear();
}

AIOScript* AIOScript::FindByKey(LuaVal const& scriptKey)
{
    AIOScriptByKeyMap::const_iterator itr = _scriptByKeyMap.find(scriptKey);
    if (itr == _scriptByKeyMap.end())
        return nullptr;

    return itr->second;
}

void AIOScript::HandleAddonBlock(Player* sender, LuaVal const& handlerKey, LuaVal const& args)
{
    OnHandle(sender, handlerKey, args);
}

void AIOScript::DispatchIncomingBlocks(Player* sender, LuaVal const& mainTable)
{
    for (unsigned int i = 1; i <= mainTable.len(); ++i)
    {
        LuaVal block = mainTable.get(static_cast<int>(i));
        if (!block.istable())
            continue;

        LuaVal nArgsVal = block.get(1);
        LuaVal scriptKeyVal = block.get(2);
        LuaVal handlerKeyVal = block.get(3);
        if (!nArgsVal.isnumber() || scriptKeyVal.isnil() || handlerKeyVal.isnil())
            continue;

        if (nArgsVal.num() > double(Trinity::AIO::MAX_BLOCK_ARGS))
        {
            sLog->outAIOMessage(sender->GetGUID().GetCounter(), LOG_LEVEL_ERROR,
                "AIO: Block from '{}' has over {} arguments (n={:.0f}). Sender: {}",
                scriptKeyVal.tostring(), Trinity::AIO::MAX_BLOCK_ARGS, nArgsVal.num(), sender->GetName());
            continue;
        }

        if (AIOScript* aioScript = AIOScript::FindByKey(scriptKeyVal))
            aioScript->HandleAddonBlock(sender, handlerKeyVal, block);
    }
}

bool AIOScript::AddAddon(std::string const& addonName, std::string const& addonFile, uint32 permission)
{
    return sWorld->AddAddon(World::AIOAddon(addonName, addonFile, permission));
}

void AIOScript::OnHandle(Player* sender, LuaVal const& handlerKey, LuaVal const& args)
{
    HandlerMapType::const_iterator itr = _handlerMap.find(handlerKey);
    if (itr != _handlerMap.end())
    {
        itr->second(sender, args);
        return;
    }

    sLog->outAIOMessage(sender->GetGUID().GetCounter(), LOG_LEVEL_ERROR, "AIO: No handler '{}' on script '{}'. Sender: {}",
        handlerKey.tostring(), _key.tostring(), sender->GetName());
}

AIOHandlers::AIOHandlers()
    : AIOScript("AIO")
{
    AddHandler("Init", std::bind(&AIOHandlers::HandleInit, this, std::placeholders::_1, std::placeholders::_2));
    AddHandler("Error", std::bind(&AIOHandlers::HandleError, this, std::placeholders::_1, std::placeholders::_2));
}

void AIOHandlers::HandleInit(Player* sender, LuaVal const& args)
{
    if (sender->isAIOInitOnCooldown())
        return;

    LuaVal versionVal = args.get(4);
    LuaVal clientDataVal = args.get(5);
    if (!versionVal.isnumber() || !clientDataVal.istable())
    {
        sLog->outAIOMessage(sender->GetGUID().GetCounter(), LOG_LEVEL_ERROR, "AIOHandlers::HandleInit: Invalid version value or clientData value. Sender: {}, Args: {}", sender->GetName(), args.dumps());
        return;
    }

    if (std::abs(versionVal.num() - AIO_VERSION) > 0.01)
    {
        Trinity::AIO::Handle(sender, "AIO", "Init", AIO_VERSION);
        return;
    }

    sender->setAIOIntOnCooldown(true);

    LuaVal addonTable(TTABLE);
    LuaVal cacheTable(TTABLE);
    uint32 const nAddons = sWorld->PrepareClientAddons(clientDataVal, addonTable, cacheTable, sender);

    LuaVal argsToSend(TTABLE);

    uint32 blockIndex = 1;
    for (HookListType::const_iterator itr = _initHookList.begin(); itr != _initHookList.end(); ++itr)
    {
        uint32 index = 3;
        LuaVal hookBlock(TTABLE);

        hookBlock[1] = static_cast<unsigned int>(itr->argsList.size()) + 1u;
        hookBlock[2] = itr->scriptKey;
        hookBlock[3] = itr->handlerKey;
        for (std::list<ArgFunc>::const_iterator it = itr->argsList.begin(); it != itr->argsList.end(); ++it)
            hookBlock[static_cast<int>(++index)] = (*it)(sender);

        argsToSend[static_cast<unsigned int>(++blockIndex)] = hookBlock;
    }

    LuaVal AIOInitBlock(TTABLE);
    AIOInitBlock[1] = 5;
    AIOInitBlock[2] = "AIO";
    AIOInitBlock[3] = "Init";
    AIOInitBlock[4] = AIO_VERSION;
    AIOInitBlock[5] = static_cast<unsigned int>(nAddons);
    AIOInitBlock[6] = addonTable;
    AIOInitBlock[7] = cacheTable;

    argsToSend[1] = AIOInitBlock;

    for (AIOScript::InitMessageFunc const& hook : _initMessageHooks)
        hook(sender, argsToSend);

    sender->SendSimpleAIOMessage(argsToSend.dumps());
}

void AIOHandlers::HandleError(Player* sender, LuaVal const& args)
{
    LuaVal msgVal = args.get(4);
    if (!msgVal.isstring())
        return;

    sLog->outAIOMessage(sender->GetGUID().GetCounter(), LOG_LEVEL_ERROR, "{} Received client addon error: {}", sender->GetSession()->GetPlayerInfo(), msgVal.str());
}

AIOHandlers* CreateAIOHandlers()
{
    return new AIOHandlers();
}

void DestroyAIOHandlers(AIOHandlers* handlers)
{
    delete handlers;
}

void RegisterAIOInitHookOnHandlers(AIOHandlers* handlers, AIOScript::InitMessageFunc func)
{
    if (handlers)
        handlers->RegisterInitMessageHook(std::move(func));
}

void RegisterAIOInitArgsOnHandlers(AIOHandlers* handlers, LuaVal const& scriptKey, LuaVal const& handlerKey,
    AIOScript::ArgFunc a1, AIOScript::ArgFunc a2, AIOScript::ArgFunc a3,
    AIOScript::ArgFunc a4, AIOScript::ArgFunc a5, AIOScript::ArgFunc a6)
{
    if (handlers)
        handlers->RegisterInitArgs(scriptKey, handlerKey, std::move(a1), std::move(a2), std::move(a3), std::move(a4), std::move(a5), std::move(a6));
}
