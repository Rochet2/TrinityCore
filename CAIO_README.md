## Introduction

CAIO is a server-client communication system for WoW AddOns. It is an extension of [AIO](https://github.com/Rochet2/AIO) to support C++ server side handling.
AIO is designed for sending lua addons and data between players and server.

Currently CAIO only supports TrinityCore 3.3.5 branch. [Compare and review](https://github.com/TrinityCore/TrinityCore/compare/3.3.5...SaiFi0102:CAIO-3.3.5).

## Supported AIO version

AIO version **1.75** — must match `AIO_VERSION` in your server and client `AIO.lua` files (for example `C:\Users\tqbat\Documents\Cores\stuff\AIO`).

## Install

+ Clone this repository/branch or merge with your own TrinityCore 3.3.5 branch
+ `git submodule update --init --recursive` (required for `dep/smallfolk_cpp/smallfolk_cpp`, tracks [smallfolk_cpp](https://github.com/Rochet2/smallfolk_cpp) **master**, currently v2.x)
+ Build/Install TrinityCore
+ [Install(Add) (C)AIO scripts](#api-reference) — see also `doc/CAIO_SCRIPT_EXAMPLE.md`
+ Run SQL files from `TrinityCore_Installation_Dir/sql/CAIO` (`Auth.sql` on auth DB, `World.sql` on world DB)
+ Copy `AIO_Client` from your AIO tree to `WoW_Installation_Dir/Interface/AddOns/AIO_Client` (use the same AIO repo/commit as the server expects)
+ Copy server-side client addon sources into `TrinityCore_Installation_Dir/lua_client_scripts` (one folder per addon, e.g. `lua_client_scripts/ExampleWindow/ExampleWindow.lua`)
+ Set `AIO.MsgMaxLen` to **255** in `worldserver.conf` (WoW addon whisper limit; matches client `AIO.lua` when `AIO_SERVER` is false)
+ Optional: build with `-DWITH_CAIO_EXAMPLES=ON` to include the `ExampleWindow` test script

## Build notes (TrinityCore 3.3.5 + CAIO)

+ OpenSSL **3.x** is detected by upstream `cmake/macros/FindOpenSSL.cmake` after the 3.3.5 merge (no manual `-D_OPENSSL_VERSION` needed in most cases).
+ Boost **1.86** with MSVC may still fail on very old 3.3.5 code paths; **Boost 1.81** is a safe choice if you hit `std::_snprintf` errors in Boost headers.
+ Example CMake configure:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DSCRIPTS=static -DTOOLS=0 `
  -DOPENSSL_INCLUDE_DIR="C:/Program Files/OpenSSL-Win64/include" `
  -DSSL_EAY_RELEASE="C:/Program Files/OpenSSL-Win64/lib/VC/x64/MD/libssl.lib" `
  -DLIB_EAY_RELEASE="C:/Program Files/OpenSSL-Win64/lib/VC/x64/MD/libcrypto.lib" `
  -DMYSQL_INCLUDE_DIR="C:/Program Files/MySQL/MySQL Server 8.0/include" `
  -DMYSQL_LIBRARY="C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.lib" `
  -DBoost_DIR="C:/local/boost_1_81_0/lib64-msvc-14.3/cmake/Boost-1.81.0"
```

## Stock AIO server parity (C++ vs `AIO.lua` with `AIO_SERVER = true`)

+ **Transport:** `LANG_ADDON` whispers with `S`/`C` prefix are required on 3.3.5; the client receives them as `CHAT_MSG_ADDON` (same as stock Eluna server). Not a CAIO gap.
+ **Init hooks:** C++ `AddInitArgs` appends extra handler blocks to the init reply. `AddOnInit` on `AIOScript` mutates the full outgoing init table before send (stock `AIO.AddOnInit` parity).
+ **Pre-init gating:** Stock server does **not** queue pre-init blocks (`AIO_INITED` is client-only). CAIO matches that.
+ **Block arg limit:** Server rejects blocks with `n > 15` (same as stock server Lua).
+ **Message cache:** `AIO.MsgCacheTime` / `AIO.MsgCacheDelay` match `AIO_MSG_CACHE_TIME` / `AIO_MSG_CACHE_DELAY` in `AIO.lua`.

## Todo

+ Implement obfuscation (optional, deferred)
+ Implement compression (optional, deferred)
+ Add individual RBAC permissions per `.caio` subcommand (optional; all subcommands use `RBAC_PERM_COMMAND_CAIO` today)

## API reference

### Creating a CAIO script

```cpp
class ExampleCAIOScript : public AIOScript
{
    public:
        ExampleCAIOScript()
            : AIOScript("ExampleScriptName")
        {
            using namespace std::placeholders;

            // Loads addon files; path from AIO.ClientScriptPath in worldserver.conf
            AddAddon("ExampleAddon", "example_addon.lua");
            AddAddon("AnotherAddon", "example_addon.lua", 192);

            AddHandler("Print", std::bind(&ExampleCAIOScript::HandlePrint, this, _1, _2));
            AddInitArgs("ExampleScriptName", "Init", std::bind(&ExampleCAIOScript::InitArg, this, _1));
        }

        void HandlePrint(Player* sender, LuaVal const& args)
        {
            LuaVal inputVal = args.get(4);
            if (!inputVal.isstring())
                return;
        }

    private:
        std::string storedString;
};
```

### smallfolk_cpp LuaVal reference

https://github.com/Rochet2/smallfolk_cpp (v2.x on `master` — linked as CMake target `smallfolk_cpp::smallfolk`, include `smallfolk.h`)

Use `LuaVal::nil` (not `LuaVal::nil()`) for default optional arguments. Type tag accessor is `typetag()`.

### CAIO reference and functions

**AIOScript.h** (included from `ScriptMgr.h`)

```cpp
class AIOScript : public ScriptObject
{
    protected:
        AIOScript(LuaVal const& scriptKey);
        void AddHandler(LuaVal const& handlerKey, HandlerFunc function);
        void AddInitArgs(LuaVal const& scriptKey, LuaVal const& handlerKey, ...);
        void AddOnInit(InitMessageFunc func); // mutates full init reply (AIO.AddOnInit parity)
        bool AddAddon(std::string const& addonName, std::string const& addonFile, uint32 permission = AIO_DEFAULT_ADDON_PERMISSION);
};
```

**AIOMsg.h**

```cpp
class AIOMsg
{
    public:
        AIOMsg();
        AIOMsg(LuaVal const& scriptKey, LuaVal const& handlerKey,
            LuaVal const& a1 = LuaVal::nil, LuaVal const& a2 = LuaVal::nil, LuaVal const& a3 = LuaVal::nil,
            LuaVal const& a4 = LuaVal::nil, LuaVal const& a5 = LuaVal::nil, LuaVal const& a6 = LuaVal::nil);
        AIOMsg& Add(LuaVal const& scriptKey, LuaVal const& handlerKey,
            LuaVal const& a1 = LuaVal::nil, LuaVal const& a2 = LuaVal::nil, LuaVal const& a3 = LuaVal::nil,
            LuaVal const& a4 = LuaVal::nil, LuaVal const& a5 = LuaVal::nil, LuaVal const& a6 = LuaVal::nil);
        AIOMsg& AppendLast(LuaVal const& a1 = LuaVal::nil, LuaVal const& a2 = LuaVal::nil, LuaVal const& a3 = LuaVal::nil,
            LuaVal const& a4 = LuaVal::nil, LuaVal const& a5 = LuaVal::nil, LuaVal const& a6 = LuaVal::nil);
        std::string dumps() const;
};
```

**Player.h** / **PlayerAIO.h**

```cpp
// Player.h — no LuaVal in the public player API
void Player::SendSimpleAIOMessage(std::string const& message);
void Player::ForceReloadAddons();
void Player::ForceResetAddons();

// PlayerAIO.h — use from CAIO scripts
namespace Trinity::AIO {
    void Message(Player* player, AIOMsg& msg);
    void Handle(Player* player, LuaVal const& scriptKey, LuaVal const& handlerKey, ...);
    void Handle(Player* player, char const* scriptKey, char const* handlerKey, ...);
}
```

**World.h**

```cpp
struct AIOAddon { /* name, file, permission */ };

std::string GetAIOPrefix() const;
std::string GetAIOClientScriptPath() const;
void ForceReloadPlayerAddons(uint32 permission = AIO_DEFAULT_ADDON_PERMISSION);
void ForceResetPlayerAddons(uint32 permission = AIO_DEFAULT_ADDON_PERMISSION);
void AIOMessageAll(AIOMsg& msg, uint32 permission = AIO_DEFAULT_ADDON_PERMISSION);
void SendAllSimpleAIOMessage(std::string const& message, uint32 permission = AIO_DEFAULT_ADDON_PERMISSION);
bool ReloadAddons();
bool AddAddon(AIOAddon const& addon);
bool RemoveAddon(std::string const& addonName, uint32* permission = nullptr);
```

## CAIO game commands

+ .caio version
+ .caio addaddon $addonName [$permission] "$addonFile"
+ .caio removeaddon $addonName
+ .caio reloadaddons
+ .caio forcereload $playerName
+ .caio forcereset $playerName
+ .caio forcereloadall [$permission]
+ .caio forceresetall [$permission]
+ .caio send $playerName "Message"
+ .caio sendall [$permission] "Message"

Note: By default every player has permission 195. Permission 195 will be used if not specified.

## Reporting issues and submitting fixes

Issues can be reported via the [Github issue tracker](https://github.com/SaiFi0102/TrinityCore/issues). Fixes can be submitted as pull requests on the Github repository.

## Authors, Contributors &amp; Thanks

+ Saif
  + CAIO
+ Rochet2
  + [AIO](https://github.com/Rochet2/AIO)
  + [smallfolk_cpp](https://github.com/Rochet2/smallfolk_cpp) to handle and transmit Lua data in C++
