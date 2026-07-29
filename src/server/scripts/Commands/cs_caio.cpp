/* ScriptData
Name: caio_commandscript
%Complete : 100
Comment : All AIO related server side commands
Category : commandscripts
EndScriptData */

#include "Chat.h"
#include "Language.h"
#include "Player.h"
#include "RBAC.h"
#include "ScriptMgr.h"
#include "World.h"
#include "smallfolk.h"

class caio_commandscript : public CommandScript
{
public:
    caio_commandscript() : CommandScript("caio_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> caioCommandTable =
        {
            { "version",        rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleVersionCommand,        "" },
            { "send",           rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleSendCommand,           "" },
            { "forcereload",    rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleReloadCommand,         "" },
            { "forcereset",     rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleResetCommand,          "" },
            { "sendall",        rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleSendAllCommand,        "" },
            { "forcereloadall", rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleReloadAllCommand,      "" },
            { "forceresetall",  rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleResetAllCommand,       "" },
            { "reloadaddons",   rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleReloadAddonsCommand,   "" },
            { "addaddon",       rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleAddAddonCommand,       "" },
            { "removeaddon",    rbac::RBAC_PERM_COMMAND_CAIO, true, &HandleRemoveAddonCommand,    "" },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "caio", rbac::RBAC_PERM_COMMAND_CAIO, true, nullptr, "", caioCommandTable },
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
        Player* target = nullptr;
        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        char* tailStr = strtok(nullptr, "");
        if (!tailStr)
            return false;

        char* msg = handler->extractQuotedArg(tailStr);
        if (!msg)
            return false;

        if (!LuaVal::loads(msg).istable())
        {
            handler->SendSysMessage("CAIO: message must be smallfolk-serialized table data (use AIOMsg or AIO.Msg on the server).");
            return false;
        }

        target->SendSimpleAIOMessage(msg);
        handler->PSendSysMessage(LANG_SENDMESSAGE, target->GetName().c_str(), msg);
        return true;
    }

    static bool HandleReloadCommand(ChatHandler* handler, char const* args)
    {
        Player* target = nullptr;
        if (!handler->extractPlayerTarget((char*)args, &target, nullptr, nullptr))
            return false;

        target->ForceReloadAddons();
        handler->PSendSysMessage(LANG_CAIO_FORCERELOAD_SENT, target->GetName().c_str());
        return true;
    }

    static bool HandleResetCommand(ChatHandler* handler, char const* args)
    {
        Player* target = nullptr;
        if (!handler->extractPlayerTarget((char*)args, &target, nullptr, nullptr))
            return false;

        target->ForceResetAddons();
        handler->PSendSysMessage(LANG_CAIO_FORCERESET_SENT, target->GetName().c_str());
        return true;
    }

    static bool HandleSendAllCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* msg = handler->extractQuotedArg((char*)args);
        if (!msg)
            return false;

        if (!LuaVal::loads(msg).istable())
        {
            handler->SendSysMessage("CAIO: message must be smallfolk-serialized table data (use AIOMsg or AIO.Msg on the server).");
            return false;
        }

        char* permission = strtok(nullptr, "");
        uint32 perm = AIO_DEFAULT_ADDON_PERMISSION;
        if (permission)
        {
            try
            {
                perm = std::stoi(permission);
            }
            catch (std::exception&)
            {
                return false;
            }
        }

        sWorld->SendAllSimpleAIOMessage(msg, perm);
        handler->PSendSysMessage(LANG_SENDMESSAGE, "all players", msg);
        return true;
    }

    static bool HandleReloadAllCommand(ChatHandler* handler, char const* args)
    {
        uint32 perm = AIO_DEFAULT_ADDON_PERMISSION;
        if (args && *args)
        {
            try
            {
                perm = std::stoi(args);
            }
            catch (std::exception&)
            {
                return false;
            }
        }

        sWorld->ForceReloadPlayerAddons(perm);
        handler->PSendSysMessage(LANG_CAIO_FORCERELOAD_SENT, "all players");
        return true;
    }

    static bool HandleResetAllCommand(ChatHandler* handler, char const* args)
    {
        uint32 perm = AIO_DEFAULT_ADDON_PERMISSION;
        if (args && *args)
        {
            try
            {
                perm = std::stoi(args);
            }
            catch (std::exception&)
            {
                return false;
            }
        }

        sWorld->ForceResetPlayerAddons(perm);
        handler->PSendSysMessage(LANG_CAIO_FORCERESET_SENT, "all players");
        return true;
    }

    static bool HandleReloadAddonsCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (sWorld->ReloadAddons())
            sWorld->ForceReloadPlayerAddons();
        else
            handler->SendSysMessage(LANG_CAIO_RELOADADDONS_ERROR);

        return true;
    }

    static bool HandleAddAddonCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* addonName = strtok((char*)args, " ");
        if (!addonName || addonName[0] == '"')
            return false;

        char* tailStr = strtok(nullptr, "");
        char* addonFile = handler->extractQuotedArg(tailStr);
        if (!addonFile)
            return false;

        char* permission = strtok(nullptr, "");
        uint32 perm = AIO_DEFAULT_ADDON_PERMISSION;
        if (permission)
        {
            try
            {
                perm = std::stoi(permission);
            }
            catch (std::exception&)
            {
                return false;
            }
        }

        World::AIOAddon newAddon(addonName, addonFile, perm);
        if (sWorld->AddAddon(newAddon))
            sWorld->ForceReloadPlayerAddons(perm);
        else
            handler->PSendSysMessage(LANG_CAIO_ADDADDON_ERROR, addonName);

        return true;
    }

    static bool HandleRemoveAddonCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 perm = AIO_DEFAULT_ADDON_PERMISSION;
        if (sWorld->RemoveAddon(args, &perm))
            sWorld->ForceReloadPlayerAddons(perm);
        else
            handler->PSendSysMessage(LANG_CAIO_REMOVEADDON_ERROR, args);

        return true;
    }
};

void AddSC_caio_commandscript()
{
    new caio_commandscript();
}
