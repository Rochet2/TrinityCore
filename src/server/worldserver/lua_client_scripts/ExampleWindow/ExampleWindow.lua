-- Minimal client-side companion for ExampleWindow.cpp (WITH_CAIO_EXAMPLES).
-- Requires AIO_Client in Interface/AddOns with matching AIO 1.75.
-- Place under lua_client_scripts/ExampleWindow/ (or your AIO.ClientScriptPath).

local AIO = AIO or require("AIO")

if not AIO.AddAddon then
    return
end

AIO.AddAddon("ExampleWindow")

local handlers = {}

function handlers.Init(...)
    -- Server AddInitArgs on AIOExample/Init are delivered here.
    local args = { ... }
    print("ExampleWindow: Init received", #args, "arg(s)")
end

function handlers.StressTest(data)
    if type(data) == "string" then
        print("ExampleWindow: StressTest received", #data, "bytes")
    end
end

-- Echo Print back to the server (button name, input text, slider value).
function handlers.Print(button, input, slider)
    AIO.Msg():Add("AIOExample", "Print", button or "?", input or "", slider or 0):Send()
end

for name, fn in pairs(handlers) do
    AIO.RegisterEvent("AIOExample", name, fn)
end

-- Simple slash command to exercise the server Print handler without a full UI frame.
SLASH_CAIOEXAMPLE1 = "/caioexample"
SlashCmdList.CAIOEXAMPLE = function(msg)
    local size = tonumber(msg) or 16
    handlers.Print("SlashCmd", tostring(size), 0.5)
end
