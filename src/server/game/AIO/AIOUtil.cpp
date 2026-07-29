
#include "AIOUtil.h"

#include <exception>

namespace Trinity::AIO
{
bool IsSafeAddonRelativePath(std::string const& path)
{
    if (path.empty() || path.front() == '/' || path.front() == '\\')
        return false;

    if (path.find("..") != std::string::npos)
        return false;

    return path.find_first_of(":*?\"<>|") == std::string::npos;
}

LoadMessageOutcome TryLoadIncomingMessage(std::string const& message, uint32 maxBytes, uint32 maxBlocks)
{
    LoadMessageOutcome outcome;

    if (message.size() > maxBytes)
    {
        outcome.result = LoadMessageResult::Oversize;
        return outcome;
    }

    try
    {
        outcome.table = LuaVal::loads(message);
    }
    catch (std::exception const&)
    {
        outcome.result = LoadMessageResult::ParseError;
        return outcome;
    }
    catch (...)
    {
        outcome.result = LoadMessageResult::ParseError;
        return outcome;
    }

    if (outcome.table.isnil())
    {
        outcome.result = LoadMessageResult::ParseError;
        return outcome;
    }

    if (!outcome.table.istable())
    {
        outcome.result = LoadMessageResult::NotTable;
        return outcome;
    }

    if (outcome.table.len() > maxBlocks)
    {
        outcome.result = LoadMessageResult::TooManyBlocks;
        return outcome;
    }

    outcome.result = LoadMessageResult::Ok;
    return outcome;
}
}
