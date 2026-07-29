
#ifndef TRINITY_AIO_UTIL_H
#define TRINITY_AIO_UTIL_H

#include "Define.h"
#include "smallfolk.h"
#include <string>

namespace Trinity::AIO
{
    constexpr uint32 MAX_BLOCK_ARGS = 15;

    TC_GAME_API bool IsSafeAddonRelativePath(std::string const& path);

    enum class LoadMessageResult : uint8
    {
        Ok,
        Oversize,
        ParseError,
        NotTable,
        TooManyBlocks
    };

    struct LoadMessageOutcome
    {
        LoadMessageResult result = LoadMessageResult::ParseError;
        LuaVal table;
    };

    TC_GAME_API LoadMessageOutcome TryLoadIncomingMessage(std::string const& message, uint32 maxBytes, uint32 maxBlocks);
}

#endif
