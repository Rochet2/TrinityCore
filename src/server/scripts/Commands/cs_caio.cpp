/* ScriptData
Name: caio_commandscript
%Complete : 100
Comment : All AIO related server side commands
Category : commandscripts
EndScriptData */

#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "Language.h"

class caio_commandscript : public CommandScript
{
	public:
		caio_commandscript() : CommandScript("caio_commandscript") { }

		ChatCommand* GetCommands() const
		{
			static ChatCommand caioCommandTable[] =
			{
				{ "version", rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleVersionCommand, "", NULL },
				{ "send", rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleSendCommand, "", NULL },
				{ "forcereload", rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleReloadCommand, "", NULL },
				{ "forcereset", rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleResetCommand, "", NULL },
				{ "sendall", rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleSendAllCommand, "", NULL },
				{ "forcereloadall", rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleReloadAllCommand, "", NULL },
				{ "forceresetall", rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleResetAllCommand, "", NULL },
				{ "reloadaddons", rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleReloadAddonsCommand, "", NULL },
				{ "addaddon", rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleAddAddonCommand, "", NULL },
				{ "removeaddon", rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleRemoveAddonCommand, "", NULL },
				{ NULL, 0, false, NULL, "", NULL }
			};
			static ChatCommand commandTable[] =
			{
				{ "caio", rbac::RBAC_PERM_COMMAND_CAIO, true, 0, "", caioCommandTable },
				{ NULL, 0, false, NULL, "", NULL }
			};
			return commandTable;
		}
		
		static bool HandleVersionCommand(ChatHandler* handler, char const* /*args*/)
		{
			handler->PSendSysMessage("AIO version %s (protocol %.2f).", AIO_VERSION_STRING, AIO_VERSION);
			return true;
		}

		static bool HandleSendCommand(ChatHandler* handler, char const* args)
		{
			//Player name
			Player* target;
			if(!handler->extractPlayerTarget((char*)args, &target))
				return false;

			//Quoted message
			char* tailStr = strtok(NULL, "");
			if(!tailStr)
				return false;

			char* msg = handler->extractQuotedArg(tailStr);
			if(!msg)
				return false;

			target->SendSimpleAIOMessage(msg);
			handler->PSendSysMessage(LANG_SENDMESSAGE, target->GetName().c_str(), msg);
			return true;
		};

		static bool HandleReloadCommand(ChatHandler* handler, char const* args)
		{
			Player *target = 0;
			if(!handler->extractPlayerTarget((char*)args, &target, 0, 0))
				return false;

			target->ForceReloadAddons();
			handler->PSendSysMessage(LANG_CAIO_FORCERELOAD_SENT, target->GetName().c_str());
			return true;
		};

		static bool HandleResetCommand(ChatHandler* handler, char const* args)
		{
			Player *target = 0;
			if(!handler->extractPlayerTarget((char*)args, &target, 0, 0))
				return false;

			target->ForceResetAddons();
			handler->PSendSysMessage(LANG_CAIO_FORCERESET_SENT, target->GetName().c_str());
			return true;
		};

		static bool HandleSendAllCommand(ChatHandler* handler, char const* args)
		{
			if(!*args)
				return false;

			//Get message
			char *msg = handler->extractQuotedArg((char*)args);
			if(!msg)
				return false;

			char *permission = strtok(NULL, "");
			uint32 perm = 195;
			if(permission) //Get permission if its there
			{
				try
				{
					perm = std::stoi(permission);
				}
				catch(std::exception &)
				{
					return false;
				}
			}

			sWorld->SendAllSimpleAIOMessage(msg, perm);
			handler->PSendSysMessage(LANG_SENDMESSAGE, "all players", msg);
			return true;
		};

		static bool HandleReloadAllCommand(ChatHandler* handler, char const* args)
		{
			uint32 perm = 195;
			if(args && *args) //Get permission if its there
			{
				try
				{
					perm = std::stoi(args);
				}
				catch(std::exception &)
				{
					return false;
				}
			}

			//Force reload required players
			sWorld->ForceReloadPlayerAddons(perm);
			handler->PSendSysMessage(LANG_CAIO_FORCERELOAD_SENT, "all players");
			return true;
		};

		static bool HandleResetAllCommand(ChatHandler* handler, char const* args)
		{
			uint32 perm = 195;
			if(args && *args) //Get permission if its there
			{
				try
				{
					perm = std::stoi(args);
				}
				catch(std::exception &)
				{
					return false;
				}
			}

			//Force reset required players
			sWorld->ForceResetPlayerAddons(perm);
			handler->PSendSysMessage(LANG_CAIO_FORCERESET_SENT, "all players");
			return true;
		};

		static bool HandleReloadAddonsCommand(ChatHandler* handler, char const* args)
		{

			//Reload and force reload required players
			bool success = sWorld->ReloadAddons();
			if(success)
			{
				sWorld->ForceReloadPlayerAddons();
			}
			else
			{
				handler->SendSysMessage(LANG_CAIO_RELOADADDONS_ERROR);
			}
			return true;
		}

		static bool HandleAddAddonCommand(ChatHandler* handler, char const* args)
		{
			if(!*args)
				return false;

			//Addon name
			char *addonName = strtok((char*)args, " ");
			if(!addonName || addonName[0] == '"')
				return false;

			//File
			char *tailStr = strtok(NULL, "");
			char *addonFile = handler->extractQuotedArg(tailStr);
			if(!addonFile)
				return false;

			//Permission
			char *permission = strtok(NULL, "");
			uint32 perm = 195;
			if(permission)
			{
				try
				{
					perm = std::stoi(permission);
				}
				catch(std::exception &)
				{
					return false;
				}
			}

			//Add
			World::AIOAddon newAddon(addonName, addonFile, perm);
			bool added = sWorld->AddAddon(newAddon);
			if(added)
			{
				sWorld->ForceReloadPlayerAddons(perm);
			}
			else
			{
				handler->PSendSysMessage(LANG_CAIO_ADDADDON_ERROR, addonName);
			}
			return true;
		}

		static bool HandleRemoveAddonCommand(ChatHandler* handler, char const* args)
		{
			if(!*args)
				return false;

			//Remove and reload required players
			uint32 perm = sWorld->RemoveAddon(args);
			if(perm != 0)
			{
				sWorld->ForceReloadPlayerAddons(perm);
			}
			else
			{
				handler->PSendSysMessage(LANG_CAIO_REMOVEADDON_ERROR, args);
			}
			return true;
		}
};

void AddSC_caio_commandscript()
{
	new caio_commandscript();
}