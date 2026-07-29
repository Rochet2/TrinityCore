-- Optional CAIO example client script (pairs with ExampleWindow.cpp when built with WITH_CAIO_EXAMPLES).
-- Requires AIO_Client in Interface/AddOns with matching AIO 1.75.

local AIO = AIO or require("AIO")

if not AIO.AddAddon then
    return
end

AIO.AddAddon("ExampleWindow")

local function OnInit(initData)
    -- Server may pass init args via AddInitArgs on script "AIOExample" / handler "Init".
end

AIO.RegisterEvent("AIOExample", "Init", OnInit)

AIO.RegisterEvent("AIOExample", "StressTest", function(data)
    if type(data) == "string" then
        print("ExampleWindow: StressTest received", #data, "bytes")
    end
end)

AIO.RegisterEvent("AIOExample", "Print", function(button, input, slider)
    AIO.Msg():Add("AIOExample", "Print", button, input, slider):Send()
end)
