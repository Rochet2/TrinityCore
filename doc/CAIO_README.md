## Introduction

CAIO is a server-client communication system for WoW AddOns. It is an extension of [AIO](https://github.com/Rochet2/AIO) to support C++ server side handling.
AIO is designed for sending lua addons and data between players and server.

Currently CAIO only supports TrinityCore 3.3.5. This work lives on [Rochet2/TrinityCore](https://github.com/Rochet2/TrinityCore) (branch `c_aio_new` / PR #183).

## License

+ TrinityCore base: GPLv2 text in `COPYING`; existing TrinityCore sources keep their per-file headers (**GPLv2 or later**).
+ CAIO: GPLv3 text in `CAIO LICENSE.txt`.
+ CAIO-owned units (e.g. `src/server/game/AIO/*`, `AIOMsg.*`, `AIOScript.*`, `cs_caio.cpp`, `src/server/scripts/AIO/*`, and CAIO tests) intentionally have **no** per-file license banner — same as original SaiFi CAIO; their license is via `CAIO LICENSE.txt`.
+ Because TrinityCore headers permit “or later”, a combined TrinityCore+CAIO tree may be distributed under **GPLv3**.

## Supported AIO version

AIO protocol version **1.75** — must match `AIO_VERSION` in your server and client `AIO.lua` files (see [Rochet2/AIO](https://github.com/Rochet2/AIO)). Init still uses exact float equality (`|a-b| > 0.01`), so stock 1.75 clients keep working.

CAIO mirrors messaging hardening from [AIO PR #27](https://github.com/Rochet2/AIO/pull/27) (cache TTL in ms, hole-safe reassembly, buffered-byte accounting, framing helpers/tests) **without** bumping the Init version — those fixes are wire-compatible.

**Future (not in this branch):** semantic versioning (`MAJOR.MINOR.PATCH`) with Init matching major+minor only so patch releases can diverge. That should land in the AIO client first, then CAIO.

## Security notes (untrusted client → server)

+ Treat all client→server AIO payloads as untrusted: validate block `n`, script/handler keys, sizes, and part metadata before dispatch.
+ Production toggles: `AIO.MsgRateLimitMs`, `AIO.MaxIncomingMessageSize`, `AIO.MaxBlocks`, `AIO.MaxParts`, `AIO.MaxBufferSize`, `AIO.MaxParseFailures`, `AIO.MsgCacheTime` / `AIO.MsgCacheDelay`.
+ Addon file paths are restricted via `IsSafeAddonRelativePath` (no `..`, absolute paths, or shell metacharacters).

## PR #27 parity (already in CAIO vs newly covered)

+ Already present: ms message-cache TTL/sweep, hole-safe long-message completion, per-message `BufferedBytes` caps, middle-`nil` `AIOMsg` packing, path safety, rate/parse abuse gates.
+ This work: extracted framing encode/split + reassembler (unit-tested), basename/expiry helpers, `AIO.ForceReloadOnStartup`, expanded Catch2 coverage mirroring AIO’s framing/reassembler/util tests.

## Install

+ Clone this repository/branch or merge with your own TrinityCore 3.3.5 branch
+ `git submodule update --init --recursive` (required for `dep/smallfolk_cpp/smallfolk_cpp` — [smallfolk_cpp](https://github.com/Rochet2/smallfolk_cpp); pin the submodule commit in production, currently `750580db99847f6043fdf952d572f0c40acc3e55`)
+ Build/Install TrinityCore
+ [Install(Add) (C)AIO scripts](#api-reference) — see also `doc/CAIO_SCRIPT_EXAMPLE.md` and `doc/CAIO_MESSAGE_FORMAT.md`
+ Run SQL files from `TrinityCore_Installation_Dir/sql/CAIO` (`Auth.sql` on auth DB, `World.sql` on world DB)
+ Copy `AIO_Client` from your AIO tree to `WoW_Installation_Dir/Interface/AddOns/AIO_Client` (use the same AIO repo/commit as the server expects)
+ Copy server-side client addon sources into `TrinityCore_Installation_Dir/lua_client_scripts` (one folder per addon, e.g. `lua_client_scripts/ExampleWindow/ExampleWindow.lua`)
+ Set `AIO.MsgMaxLen` to **255** in `worldserver.conf` (WoW addon whisper limit; matches client `AIO.lua` when `AIO_SERVER` is false)
+ Ensure `Logger.AIO` is enabled in the logging config if you want AIO diagnostics
+ Optional: build with `-DWITH_CAIO_EXAMPLES=ON` to include the `ExampleWindow` test script (ships `worldserver/lua_client_scripts/ExampleWindow/ExampleWindow.lua` for install; try `/caioexample` in-game)

## Build notes (TrinityCore 3.3.5 + CAIO)

+ OpenSSL **3.x** is detected by upstream `cmake/macros/FindOpenSSL.cmake`.
+ Boost **1.86** with MSVC may still fail on very old 3.3.5 code paths; **Boost 1.81** is a safe choice if you hit `std::_snprintf` errors in Boost headers.
+ CI builds use recursive submodules and `-DWITH_CAIO_EXAMPLES=1` (see `.github/workflows/linux-build.yml`).

## Stock AIO server parity (C++ vs `AIO.lua` with `AIO_SERVER = true`)

+ **Transport:** `LANG_ADDON` whispers with `S`/`C` prefix are required on 3.3.5; the client receives them as `CHAT_MSG_ADDON`.
+ **Init hooks:** C++ `AddInitArgs` appends extra handler blocks to the init reply. `AddOnInit` on `AIOScript` mutates the full outgoing init table before send (**done** — stock `AIO.AddOnInit` parity).
+ **Pre-init gating:** Stock server does **not** queue pre-init blocks (`AIO_INITED` is client-only). CAIO matches that.
+ **Block arg limit:** Server rejects blocks with invalid `n` (non-integer / out of range / `n > 15`).
+ **Message cache:** `AIO.MsgCacheTime` / `AIO.MsgCacheDelay` match `AIO_MSG_CACHE_TIME` / `AIO_MSG_CACHE_DELAY` in `AIO.lua`.

## Todo (out of scope for v1 unless noted)

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

+ `.caio version`
+ `.caio addaddon $addonName "$addonFile" [$permission]`
+ `.caio removeaddon $addonName`
+ `.caio reloadaddons`
+ `.caio forcereload $playerName`
+ `.caio forcereset $playerName`
+ `.caio forcereloadall [$permission]`
+ `.caio forceresetall [$permission]`
+ `.caio send $playerName "Message"`
+ `.caio sendall "Message" [$permission]`

`$permission` is an **exact** RBAC permission id (`HasPermission`), not “permission and above”. Default player permission is typically `195` when omitted.

## Reporting issues and submitting fixes

Issues and pull requests: [Rochet2/TrinityCore](https://github.com/Rochet2/TrinityCore).

## Authors, Contributors & Thanks

+ Saif — CAIO
+ Rochet2 — [AIO](https://github.com/Rochet2/AIO), [smallfolk_cpp](https://github.com/Rochet2/smallfolk_cpp)
