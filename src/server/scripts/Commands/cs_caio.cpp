/* ScriptData
Name: caio_commandscript
%Complete : 100
Comment : All AIO related server side commands
Category : commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "AIO.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "Language.h"
#include "Player.h"
#include "RBAC.h"
#include "World.h"
#include "smallfolk.h"

using namespace Trinity::ChatCommands;

namespace
{
bool IsSafeAddonRelativePath(std::string const& path)
{
    if (path.empty() || path.front() == '/' || path.front() == '\\')
        return false;

    if (path.find("..") != std::string::npos)
        return false;

    return path.find_first_of(":*?\"<>|") == std::string::npos;
}
}

class caio_commandscript : public CommandScript
{
public:
    caio_commandscript() : CommandScript("caio_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable caioCommandTable =
        {
            { "version",        HandleVersionCommand,        rbac::RBAC_PERM_COMMAND_CAIO, Console::Yes },
            { "send",           HandleSendCommand,           rbac::RBAC_PERM_COMMAND_CAIO, Console::Yes },
            { "forcereload",    HandleReloadCommand,         rbac::RBAC_PERM_COMMAND_CAIO, Console::Yes },
            { "forcereset",     HandleResetCommand,          rbac::RBAC_PERM_COMMAND_CAIO, Console::Yes },
            { "sendall",        HandleSendAllCommand,        rbac::RBAC_PERM_COMMAND_CAIO, Console::Yes },
            { "forcereloadall", HandleReloadAllCommand,      rbac::RBAC_PERM_COMMAND_CAIO, Console::Yes },
            { "forceresetall",  HandleResetAllCommand,       rbac::RBAC_PERM_COMMAND_CAIO, Console::Yes },
            { "reloadaddons",   HandleReloadAddonsCommand,   rbac::RBAC_PERM_COMMAND_CAIO, Console::Yes },
            { "addaddon",       HandleAddAddonCommand,       rbac::RBAC_PERM_COMMAND_CAIO, Console::Yes },
            { "removeaddon",    HandleRemoveAddonCommand,    rbac::RBAC_PERM_COMMAND_CAIO, Console::Yes },
        };

        static ChatCommandTable commandTable =
        {
            { "caio", caioCommandTable },
        };

        return commandTable;
    }

    static bool HandleVersionCommand(ChatHandler* handler)
    {
        handler->PSendSysMessage("AIO version %s (protocol %.2f).", AIO_VERSION_STRING, AIO_VERSION);
        return true;
    }

    static bool HandleSendCommand(ChatHandler* handler, PlayerIdentifier const& target, QuotedString message)
    {
        if (!LuaVal::loads(message).istable())
        {
            handler->SendSysMessage("CAIO: message must be smallfolk-serialized table data (use AIOMsg or AIO.Msg on the server).");
            return false;
        }

        Player* player = target.GetConnectedPlayer();
        if (!player)
            return false;

        player->SendSimpleAIOMessage(message);
        handler->PSendSysMessage("CAIO: sent %zu bytes to %s.", message.size(), target.GetName().c_str());
        return true;
    }

    static bool HandleReloadCommand(ChatHandler* handler, PlayerIdentifier const& target)
    {
        Player* player = target.GetConnectedPlayer();
        if (!player)
            return false;

        player->ForceReloadAddons();
        handler->PSendSysMessage(LANG_CAIO_FORCERELOAD_SENT, target.GetName().c_str());
        return true;
    }

    static bool HandleResetCommand(ChatHandler* handler, PlayerIdentifier const& target)
    {
        Player* player = target.GetConnectedPlayer();
        if (!player)
            return false;

        player->ForceResetAddons();
        handler->PSendSysMessage(LANG_CAIO_FORCERESET_SENT, target.GetName().c_str());
        return true;
    }

    static bool HandleSendAllCommand(ChatHandler* handler, QuotedString message, Optional<uint32> permission)
    {
        if (!LuaVal::loads(message).istable())
        {
            handler->SendSysMessage("CAIO: message must be smallfolk-serialized table data (use AIOMsg or AIO.Msg on the server).");
            return false;
        }

        uint32 perm = permission.value_or(AIO_DEFAULT_ADDON_PERMISSION);
        sWorld->SendAllSimpleAIOMessage(message, perm);
        handler->PSendSysMessage("CAIO: sent %zu bytes to all players.", message.size());
        return true;
    }

    static bool HandleReloadAllCommand(ChatHandler* handler, Optional<uint32> permission)
    {
        sWorld->ForceReloadPlayerAddons(permission.value_or(AIO_DEFAULT_ADDON_PERMISSION));
        handler->PSendSysMessage(LANG_CAIO_FORCERELOAD_SENT, "all players");
        return true;
    }

    static bool HandleResetAllCommand(ChatHandler* handler, Optional<uint32> permission)
    {
        sWorld->ForceResetPlayerAddons(permission.value_or(AIO_DEFAULT_ADDON_PERMISSION));
        handler->PSendSysMessage(LANG_CAIO_FORCERESET_SENT, "all players");
        return true;
    }

    static bool HandleReloadAddonsCommand(ChatHandler* handler)
    {
        if (sWorld->ReloadAddons())
            sWorld->ForceReloadPlayerAddons();
        else
            handler->SendSysMessage(LANG_CAIO_RELOADADDONS_ERROR);

        return true;
    }

    static bool HandleAddAddonCommand(ChatHandler* handler, std::string addonName, QuotedString addonFile, Optional<uint32> permission)
    {
        if (!IsSafeAddonRelativePath(addonFile))
        {
            handler->SendSysMessage("CAIO: addon file path must be relative to AIO.ClientScriptPath and must not contain '..'.");
            return false;
        }

        uint32 perm = permission.value_or(AIO_DEFAULT_ADDON_PERMISSION);
        World::AIOAddon newAddon(addonName, addonFile, perm);
        if (sWorld->AddAddon(newAddon))
            sWorld->ForceReloadPlayerAddons(perm);
        else
            handler->PSendSysMessage(LANG_CAIO_ADDADDON_ERROR, addonName.c_str());

        return true;
    }

    static bool HandleRemoveAddonCommand(ChatHandler* handler, std::string addonName)
    {
        uint32 perm = AIO_DEFAULT_ADDON_PERMISSION;
        if (sWorld->RemoveAddon(addonName, &perm))
            sWorld->ForceReloadPlayerAddons(perm);
        else
            handler->PSendSysMessage(LANG_CAIO_REMOVEADDON_ERROR, addonName.c_str());

        return true;
    }
};

void AddSC_caio_commandscript()
{
    new caio_commandscript();
}
