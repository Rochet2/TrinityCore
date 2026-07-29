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

#include "AIOCodec.h"

using namespace Trinity::AIO::Codec;

TEST_CASE("AIO codec byte decoding", "[AIO]")
{
    REQUIRE(DecodeByte('\x01') == 0);
    REQUIRE(DecodeByte('\xFF') == 254);
    REQUIRE(DecodePair('\x01', '\x01') == 0);
    REQUIRE(DecodePair('\x02', '\x02') == 255);
}

TEST_CASE("AIO client prefix detection", "[AIO]")
{
    SECTION("matches configured wire prefix")
    {
        size_t delimPos = 0;
        REQUIRE(IsClientPrefix("CAIO", "CAIO\tpayload", delimPos));
        REQUIRE(delimPos == 4);
    }

    SECTION("rejects server prefix")
    {
        size_t delimPos = 0;
        REQUIRE_FALSE(IsClientPrefix("CAIO", "SAIO\tpayload", delimPos));
    }

    SECTION("rejects missing tab")
    {
        size_t delimPos = 0;
        REQUIRE_FALSE(IsClientPrefix("AIO", "CAIOpayload", delimPos));
    }
}
