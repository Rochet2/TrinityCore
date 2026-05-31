#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "smallfolk_cpp/smallfolk.h"

// Optional CAIO test script (from Rochet2). Pairs with AIO Examples/TestWindow client addons.
// Place ExampleWindow.lua (and related client files) under lua_client_scripts/ExampleWindow/
class ExampleWindowScript : public AIOScript
{
public:
	ExampleWindowScript()
		: AIOScript("AIOExample"), counter(0)
	{
		AddAddon("ExampleWindow", "ExampleWindow.lua");
		AddHandler("Print", std::bind(&ExampleWindowScript::HandlePrint, this, std::placeholders::_1, std::placeholders::_2));
		AddHandler("bullshit", std::bind(&ExampleWindowScript::HandleBullshit, this, std::placeholders::_1, std::placeholders::_2));
		AddInitArgs("AIOExample", "Init", std::bind(&ExampleWindowScript::InitArg, this, std::placeholders::_1), std::bind(&ExampleWindowScript::InitArg, this, std::placeholders::_1));
		AddInitArgs("AIOExample", "Init", std::bind(&ExampleWindowScript::InitArg2, this, std::placeholders::_1));
		AddInitArgs("AIOExample", "InitB");
	}

private:
	void HandlePrint(Player* sender, const LuaVal& args)
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
			sender->AIOHandle("AIOExample", "bullshit", payload);
		}
		catch (...)
		{
			ChatHandler(sender->GetSession()).SendSysMessage("ExampleWindow: invalid payload size.");
		}
	}

	void HandleBullshit(Player* sender, const LuaVal& args)
	{
		LuaVal payload = args.get(4);
		if (!payload.isstring())
			return;

		ChatHandler(sender->GetSession()).PSendSysMessage("Received bullshit block (%u bytes).", uint32(payload.str().size()));
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
