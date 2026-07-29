## Introduction

CAIO is a server-client communication system for WoW AddOns. It is an extension of [AIO](https://github.com/Rochet2/AIO) to support C++ server side handling.
AIO is designed for sending lua addons and data between players and server.

Currently CAIO only supports TrinityCore 3.3.5 branch. [Compare and review](https://github.com/TrinityCore/TrinityCore/compare/3.3.5...SaiFi0102:CAIO-3.3.5).

## Supported AIO version

AIO version **1.75** — must match `AIO_VERSION` in your server and client `AIO.lua` files (for example `C:\Users\tqbat\Documents\Cores\stuff\AIO`).

## Install

+ Clone this repository/branch or merge with your own TrinityCore 3.3.5 branch
+ `git submodule update --init --recursive` (required for `dep/smallfolk_cpp`)
+ Build/Install TrinityCore
+ [Install(Add) (C)AIO scripts](#api-reference)
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

## Todo

+ Add CAIO error time-out configuration (distinct from `AIO.BufferTimeout` reassembly timeout)
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
	
		// Loads addon files to addons list and sends them on AIO client initialization
		// Looks for the file in path config AIO.ClientScriptPath
		AddAddon(World::AIOAddon("ExampleAddon", "example_addon.lua"));
		
		// You can also add addons to be sent to players with specific permission
		AddAddon(World::AIOAddon("AnotherAddon", "example_addon.lua", 192)); //192 refers to gm level 3 RBAC permission

		// Handler function signature: void HandlerFunction(Player *sender, const LuaVal &args)
		AddHandler("Print", std::bind(&ExampleCAIOScript::HandlePrint, this, _1, _2));
		AddHandler("Save", std::bind(&ExampleCAIOScript::HandleSave, this, _1, _2));

		// Initialization handler and arguments
		AddInitArgs("ExampleScriptName", "Init", std::bind(&ExampleCAIOScript::InitArg, this, _1), std::bind(&ExampleCAIOScript::InitArg, this, _1));
		//Adds additional argument to send to handler
		AddInitArgs("ExampleScriptName", "Init", std::bind(&ExampleCAIOScript::InitArg2, this, _1));
		AddInitArgs("AnotherScript", "InitB"); //Arguments are not necessary
	}

	void HandlePrint(Player *sender, const LuaVal &args)
	{
	    //LuaVal args in a handler function is always a table
		//Handler arguments index starts from 4
		LuaVal &InputVal = args[4];
		LuaVal &SliderVal = args[5];

		//MUST check if the value type is valid or else smallfolk_cpp will
		//throw on obtaining that type
		if(!InputVal.isstring() || !SliderVal.isnumber())
		{
			return;
		}

		sender->GetSession()->SendNotification("HandlePrint -> Stored String: %s, Input: %s, Slider Value: %f",
			storedString.c_str(), InputVal.str().c_str(), SliderVal.num());
	}

	void HandleSave(Player *sender, const LuaVal &args)
	{
	    //LuaVal args in a handler function is always a table
		//Handler arguments index starts from 4
		LuaVal &SaveVal = args[4];

		//MUST check if the value type is valid
		if(!SaveVal.isstring())
		{
			return;
		}

		storedString = SaveVal.str();
		sender->GetSession()->SendNotification("Saved");
	}

	LuaVal InitArg(Player *sender)
	{
		LuaVal arg = LuaVal(TTABLE);
		arg.set("key", 12.3);
		arg["key2"] = false;

		return arg;
	}

	LuaVal InitArg2(Player *sender)
	{
		return "LuaVal will implicitly create a string LuaVal for this arg";
	}

private:
	std::string storedString;
};
```

### smallfolk_cpp LuaVal reference

https://github.com/Rochet2/smallfolk_cpp

### CAIO reference and functions

**ScriptMgr.h**

```cpp
class AIOScript : public ScriptObject
{
public:
	virtual ~AIOScript();
	
	// Returns the key of this CAIO script
	LuaVal GetKey() const;

protected:
	// Registers an AIO Handler script of scriptName
	AIOScript(const LuaVal &scriptKey);

	// Registers a handler function to call when handling
	// handleKey of this script.
	void AddHandler(const LuaVal &handlerKey, HandlerFunc function);

	// Adds a client side handler to call and adds arguments
	// to sends with it for AIO client initialization.
	//
	// You can add additional arguments to the handler by
	// calling this function again
	void AddInitArgs(const LuaVal &scriptKey, const LuaVal &handlerKey,
		ArgFunc a1 = ArgFunc(), ArgFunc a2 = ArgFunc(), ArgFunc a3 = ArgFunc(),
		ArgFunc a4 = ArgFunc(), ArgFunc a5 = ArgFunc(), ArgFunc a6 = ArgFunc());

	// Adds a WoW addon file to the list of addons with a unique
	// addon key to send on AIO client initialization.
	// Returns true if addon was added, false if addon key is taken.
	//
	// It is required to call World::ForceReloadPlayerAddons()
	// if addons are added after server is fully initialized
	// for online players to load the added addons.
	bool AddAddon(const World::AIOAddon &addon);

	// Returns pointer to an AIO script by its key and typename.
	// Returns null if scriptName doesn't exist or typename was incorrect.
	template<class ScriptClass>
	ScriptClass *GetScript(const LuaVal &key);
}
```

**AIOMsg.h**

```cpp
class AIOMsg
{
public:
	//Creates an empty AIOMsg
	AIOMsg();

	//Creates a AIO message and adds one block
	AIOMsg(const LuaVal &scriptKey, const LuaVal &handlerKey,
		const LuaVal &a1 = LuaVal::nil(), const LuaVal &a2 = LuaVal::nil(), const LuaVal &a3 = LuaVal::nil(),
		const LuaVal &a4 = LuaVal::nil(), const LuaVal &a5 = LuaVal::nil(), const LuaVal &a6 = LuaVal::nil());

	//Adds another block
	//Another block will call another handler in one message
	AIOMsg &Add(cconst LuaVal &scriptKey, const LuaVal &handlerKey,
		const LuaVal &a1 = LuaVal::nil(), const LuaVal &a2 = LuaVal::nil(), const LuaVal &a3 = LuaVal::nil(),
		const LuaVal &a4 = LuaVal::nil(), const LuaVal &a5 = LuaVal::nil(), const LuaVal &a6 = LuaVal::nil());

	//Appends the last block
	//You can add additional arguments to the last block
	AIOMsg &AppendLast(const LuaVal &a1 = LuaVal::nil(), const LuaVal &a2 = LuaVal::nil(), const LuaVal &a3 = LuaVal::nil(),
		const LuaVal &a4 = LuaVal::nil(), const LuaVal &a5 = LuaVal::nil(), const LuaVal &a6 = LuaVal::nil());

	//Returns smallfolk dump of the AIO message
	std::string dumps() const;
```

**Player.h**

```cpp
class Player
{
public:
	//Returns whether AIO client has been initialized
	bool AIOInitialized() const;

	// Sends an AIO message to the player
	// See: class AIOMsg
	void AIOMessage(AIOMsg &msg);

	// Triggers an AIO handler on the client
	// To trigger multiple handlers in one message or to send more
	// arguments use Player::AIOMessage
	void AIOHandle(const LuaVal &scriptKey, const LuaVal &handlerKey,
		const LuaVal &a1 = LuaVal::nil(), const LuaVal &a2 = LuaVal::nil(), const LuaVal &a3 = LuaVal::nil(),
		const LuaVal &a4 = LuaVal::nil(), const LuaVal &a5 = LuaVal::nil(), const LuaVal &a6 = LuaVal::nil());

	// AIO can only understand smallfolk LuaVal::dumps() format
	// Handler functions are called by creating a table as below
	// {
	//     {n, ScriptName, HandlerName(optional), Arg1..N(optional) },
	//     {n, AnotherScriptName, AnotherHandlerName(optional), Arg1..N(optional) }
	// }
	// Where n is number of arguments including handler name as an argument
	void SendSimpleAIOMessage(const std::string &message);

	// Forces reload on the player AIO addons
	// Syncs player AIO addons with server
	void ForceReloadAddons();

	// Force reset on the player AIO addons
	// Player AIO addons and addon data is deleted and downloaded again
	void ForceResetAddons();

	bool isAIOInitOnCooldown() const;
	void setAIOIntOnCooldown(bool cd);
}
```

**World.h**

```cpp
// AIOAddon container constructor
// Permission 195 will load the addon on every player
World::AIOAddon::AIOAddon(const std::string &addonName, const std::string &addonFile, uint32 permission = 195);

// AIO prefix configured in worldserver.conf
std::string World::GetAIOPrefix() cons;

// AIO client LUA files path configured in worldserver.conf
std::string World::GetAIOClientScriptPath() const;

// Forces reload on all player AIO addons
// Syncs player AIO addons with server
void World::ForceReloadPlayerAddons(uint32 permission = 195);

// Forces reset on all player AIO addons
// Player AIO addons and addon data is deleted and downloaded again
void World::ForceResetPlayerAddons(uint32 permission = 195);

// Sends an AIO message to all players
// See: class AIOMsg
void World::AIOMessageAll(AIOMsg &msg, uint32 permission = 195);

// Sends a simple string message to all players

// AIO can only understand smallfolk LuaVal::dumps() format
// Handler functions are called by creating a table as below
// {
//     {n, ScriptName, HandlerName(optional), Arg1..N(optional) },
//     {n, AnotherScriptName, AnotherHandlerName(optional), Arg1..N(optional) }
// }
// Where n is number of arguments including handler name as a argument
void World::SendAllSimpleAIOMessage(const std::string &message, uint32 permission = 195);

// Reloads client side AIO addon files and force reloads
// all player AIO addons
// Returns true if successful, false if an error occurred
bool World::ReloadAddons();

// Adds a WoW AIO addon file to the list of addons with a unique
// addon name to send on AIO client initialization.
// Returns true if addon was added, false if addon name is already taken
//
// It is required to call World::ForceReloadPlayerAddons()
// if addons are added after server is fully initialized
// for online players to load the added addons.
bool World::AddAddon(const AIOAddon &addon);

// Removes an addon from addon list and force reloads affected players
// Returns permission id if an addon was removed, 0 if addon not found
//
// It is required to call World::ForceReloadPlayerAddons()
// if addons are added after server is fully initialized
// for online players to load the added addons.
bool World::RemoveAddon(std::string const& addonName, uint32* permission = nullptr);
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
