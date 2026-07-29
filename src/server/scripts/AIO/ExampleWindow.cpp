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

#ifdef WITH_CAIO_EXAMPLES

#include "PlayerAIO.h"
#include "Player.h"
#include "Chat.h"
#include "smallfolk.h"

// Optional CAIO test script. Pairs with lua_client_scripts/ExampleWindow/ExampleWindow.lua
class ExampleWindowScript : public AIOScript
{
    public:
        ExampleWindowScript()
            : AIOScript("AIOExample"), counter(0)
        {
            AddAddon("ExampleWindow", "ExampleWindow/ExampleWindow.lua");
            AddHandler("Print", std::bind(&ExampleWindowScript::HandlePrint, this, std::placeholders::_1, std::placeholders::_2));
            AddHandler("StressTest", std::bind(&ExampleWindowScript::HandleStressTest, this, std::placeholders::_1, std::placeholders::_2));
            AddInitArgs("AIOExample", "Init", std::bind(&ExampleWindowScript::InitArg, this, std::placeholders::_1), std::bind(&ExampleWindowScript::InitArg, this, std::placeholders::_1));
            AddInitArgs("AIOExample", "Init", std::bind(&ExampleWindowScript::InitArg2, this, std::placeholders::_1));
            AddInitArgs("AIOExample", "InitB");
        }

    private:
        void HandlePrint(Player* sender, LuaVal const& args)
        {
            LuaVal btn = args.get(4);
            LuaVal inp = args.get(5);
            LuaVal val = args.get(6);
            if (!btn.isstring() || !inp.isstring() || !val.isnumber())
                return;

            ChatHandler(sender->GetSession()).PSendSysMessage("HandlePrint -> Button Name: %s, Input: %s, Slider Value: %f",
                btn.str().c_str(), inp.str().c_str(), val.num());

            try
            {
                long size = std::stol(inp.str());
                if (size < 0 || size > 8192)
                {
                    ChatHandler(sender->GetSession()).SendSysMessage("ExampleWindow: payload size must be between 0 and 8192.");
                    return;
                }
                std::string payload(size_t(size), 'b');
                Trinity::AIO::Handle(sender, "AIOExample", "StressTest", payload);
            }
            catch (...)
            {
                ChatHandler(sender->GetSession()).SendSysMessage("ExampleWindow: invalid payload size.");
            }
        }

        void HandleStressTest(Player* sender, LuaVal const& args)
        {
            LuaVal payload = args.get(4);
            if (!payload.isstring())
                return;

            ChatHandler(sender->GetSession()).PSendSysMessage("Received StressTest block (%u bytes).", uint32(payload.str().size()));
        }

        LuaVal InitArg(Player* /*sender*/)
        {
            return (counter++ % 2 == 0) ? LuaVal("Inited 1") : LuaVal("Inited 2");
        }

        LuaVal InitArg2(Player* /*sender*/)
        {
            return (counter % 2 != 0) ? LuaVal("Inited 1") : LuaVal("Inited 2");
        }

        size_t counter;
};

void AddSC_ExampleWindow()
{
    new ExampleWindowScript();
}

#else

void AddSC_ExampleWindow()
{
}

#endif
