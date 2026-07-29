/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
