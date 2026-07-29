#include "tc_catch2.h"

#include "AIOUtil.h"
#include "AIOMsg.h"

TEST_CASE("AIO addon path validation", "[AIO]")
{
    REQUIRE(Trinity::AIO::IsSafeAddonRelativePath("ExampleWindow/ExampleWindow.lua"));
    REQUIRE_FALSE(Trinity::AIO::IsSafeAddonRelativePath("../secret.lua"));
    REQUIRE_FALSE(Trinity::AIO::IsSafeAddonRelativePath("/etc/passwd"));
    REQUIRE_FALSE(Trinity::AIO::IsSafeAddonRelativePath(""));
}

TEST_CASE("AIO incoming message load", "[AIO]")
{
    AIOMsg msg("TestScript", "Handler", LuaVal("arg"));
    std::string const serialized = msg.dumps();

    SECTION("accepts valid table")
    {
        Trinity::AIO::LoadMessageOutcome const outcome = Trinity::AIO::TryLoadIncomingMessage(serialized, 4096, 8);
        REQUIRE(outcome.result == Trinity::AIO::LoadMessageResult::Ok);
        REQUIRE(outcome.table.istable());
        REQUIRE(outcome.table.len() == 1);
    }

    SECTION("rejects oversize payload")
    {
        Trinity::AIO::LoadMessageOutcome const outcome = Trinity::AIO::TryLoadIncomingMessage(serialized, 4, 8);
        REQUIRE(outcome.result == Trinity::AIO::LoadMessageResult::Oversize);
    }

    SECTION("rejects invalid smallfolk")
    {
        Trinity::AIO::LoadMessageOutcome const outcome = Trinity::AIO::TryLoadIncomingMessage("not smallfolk", 4096, 8);
        REQUIRE(outcome.result == Trinity::AIO::LoadMessageResult::ParseError);
    }
}

TEST_CASE("AIOMsg round trip", "[AIO]")
{
    AIOMsg msg;
    msg.Add("ScriptA", "HandlerA", LuaVal(1));
    msg.Add("ScriptB", "HandlerB", LuaVal("x"));

    Trinity::AIO::LoadMessageOutcome const outcome = Trinity::AIO::TryLoadIncomingMessage(msg.dumps(), 4096, 8);
    REQUIRE(outcome.result == Trinity::AIO::LoadMessageResult::Ok);
    REQUIRE(outcome.table.len() == 2);

    LuaVal block = outcome.table.get(1);
    REQUIRE(block.get(2).tostring() == "ScriptA");
    REQUIRE(block.get(3).tostring() == "HandlerA");
}

TEST_CASE("AIOMsg packs middle nil arguments", "[AIO]")
{
    AIOMsg msg;
    msg.Add("ScriptA", "HandlerA", LuaVal(1), LuaVal::nil, LuaVal(3));

    Trinity::AIO::LoadMessageOutcome const outcome = Trinity::AIO::TryLoadIncomingMessage(msg.dumps(), 4096, 8);
    REQUIRE(outcome.result == Trinity::AIO::LoadMessageResult::Ok);

    LuaVal block = outcome.table.get(1);
    // n includes handlerKey + three positional slots (middle nil kept)
    REQUIRE(uint32(block.get(1).num()) == 4u);
    REQUIRE(block.get(4).num() == 1.0);
    REQUIRE(block.get(5).isnil());
    REQUIRE(block.get(6).num() == 3.0);
}

