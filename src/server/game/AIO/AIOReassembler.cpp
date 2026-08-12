#include "AIOReassembler.h"

namespace Trinity::AIO
{
void Reassembler::Clear()
{
    _messages.clear();
}

void Reassembler::Remove(uint32 messageId)
{
    _messages.erase(messageId);
}

uint32 Reassembler::GetBufferedBytes(uint32 messageId) const
{
    MessageMap::const_iterator itr = _messages.find(messageId);
    return itr == _messages.end() ? 0u : itr->second.BufferedBytes;
}

uint32 Reassembler::Sweep(uint32 sweepDelayMs, uint32 cacheTimeMs)
{
    uint32 expired = 0;
    for (MessageMap::iterator itr = _messages.begin(); itr != _messages.end();)
    {
        itr->second.Timer += sweepDelayMs;
        if (itr->second.Timer >= cacheTimeMs)
        {
            _messages.erase(itr++);
            ++expired;
        }
        else
            ++itr;
    }
    return expired;
}

Reassembler::IngestResult Reassembler::Ingest(uint32 messageId, uint32 parts, uint32 partId, std::string partPayload,
    uint32 maxConcurrent, uint32 maxBufferBytes)
{
    IngestResult result;

    MessageMap::iterator messagePartsItr = _messages.find(messageId);
    if (messagePartsItr == _messages.end())
    {
        if (_messages.size() >= maxConcurrent)
        {
            result.error = IngestError::TooManyConcurrent;
            return result;
        }
        messagePartsItr = _messages.insert(std::make_pair(messageId, MessageBuffer())).first;
    }
    else if (parts != messagePartsItr->second.Parts)
        messagePartsItr->second = MessageBuffer();

    if (messagePartsItr->second.Map.find(partId) != messagePartsItr->second.Map.end())
    {
        result.error = IngestError::DuplicatePart;
        return result;
    }

    messagePartsItr->second.Parts = parts;
    messagePartsItr->second.Timer = 0;
    messagePartsItr->second.BufferedBytes += uint32(partPayload.size());
    if (messagePartsItr->second.BufferedBytes > maxBufferBytes)
    {
        result.error = IngestError::BufferExceeded;
        _messages.erase(messagePartsItr);
        return result;
    }

    messagePartsItr->second.Map[partId] = std::move(partPayload);

    bool haveAllParts = messagePartsItr->second.Map.size() >= static_cast<size_t>(messagePartsItr->second.Parts);
    if (haveAllParts)
    {
        for (uint32 expectedPart = 1; expectedPart <= parts; ++expectedPart)
        {
            if (messagePartsItr->second.Map.find(expectedPart) == messagePartsItr->second.Map.end())
            {
                haveAllParts = false;
                break;
            }
        }
    }

    if (!haveAllParts)
        return result;

    result.payload.reserve(messagePartsItr->second.BufferedBytes);
    for (uint32 expectedPart = 1; expectedPart <= parts; ++expectedPart)
        result.payload += messagePartsItr->second.Map.find(expectedPart)->second;

    _messages.erase(messagePartsItr);
    result.complete = true;
    return result;
}
}
