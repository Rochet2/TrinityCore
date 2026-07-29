# CAIO script example

Inherit `AIOScript` to register server-side AIO handlers. See [smallfolk_cpp](https://github.com/Rochet2/smallfolk_cpp) for `LuaVal` usage.

```cpp
class ExampleAIOScript : public AIOScript
{
    public:
        ExampleAIOScript()
            : AIOScript("ExampleScriptName")
        {
            using namespace std::placeholders;

            // Loads addon files; path from AIO.ClientScriptPath in worldserver.conf
            AddAddon("ExampleAddon", "example_addon.lua");

            // Optional permission-gated addon (RBAC id)
            AddAddon("AnotherAddon", "example_addon.lua", 192);

            AddHandler("Print", std::bind(&ExampleAIOScript::HandlePrint, this, _1, _2));
            AddHandler("Save", std::bind(&ExampleAIOScript::HandleSave, this, _1, _2));

            AddInitArgs("ExampleScriptName", "Init", std::bind(&ExampleAIOScript::InitArg, this, _1), std::bind(&ExampleAIOScript::InitArg, this, _1));
            AddInitArgs("ExampleScriptName", "Init", std::bind(&ExampleAIOScript::InitArg2, this, _1));
            AddInitArgs("AnotherScript", "InitB");
        }

        void HandlePrint(Player* sender, LuaVal const& args)
        {
            LuaVal inputVal = args.get(4);
            LuaVal sliderVal = args.get(5);

            if (!inputVal.isstring() || !sliderVal.isnumber())
                return;

            sender->GetSession()->SendNotification("HandlePrint -> Stored String: %s, Input: %s, Slider Value: %f",
                storedString.c_str(), inputVal.str().c_str(), sliderVal.num());
        }

        void HandleSave(Player* sender, LuaVal const& args)
        {
            LuaVal saveVal = args.get(4);

            if (!saveVal.isstring())
                return;

            storedString = saveVal.str();
            sender->GetSession()->SendNotification("Saved");
        }

        LuaVal InitArg(Player* /*sender*/)
        {
            LuaVal arg(TTABLE);
            arg.set("key", 12.3);
            arg["key2"] = false;
            return arg;
        }

        LuaVal InitArg2(Player* /*sender*/)
        {
            return LuaVal("LuaVal will implicitly create a string LuaVal for this arg");
        }

    private:
        std::string storedString;
};
```

Register the script in a loader (see `src/server/scripts/AIO/aio_script_loader.cpp` and `WITH_CAIO_EXAMPLES`).

Handler `args` is always a table; handler parameters start at index **4**. Always check value types before use.
