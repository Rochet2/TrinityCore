#ifndef TRINITY_AIO_REASSEMBLER_H
#define TRINITY_AIO_REASSEMBLER_H

#include "Define.h"
#include <map>
#include <string>

namespace Trinity::AIO
{
    class Reassembler
    {
    public:
        enum class IngestError : uint8
        {
            None,
            TooManyConcurrent,
            DuplicatePart,
            BufferExceeded
        };

        struct IngestResult
        {
            bool complete = false;
            IngestError error = IngestError::None;
            std::string payload;
        };

        void Clear();
        void Remove(uint32 messageId);
        size_t Size() const { return _messages.size(); }
        uint32 GetBufferedBytes(uint32 messageId) const;

        // Advance per-message timers by sweepDelayMs; erase those with Timer >= cacheTimeMs.
        // Returns number of expired messages removed.
        uint32 Sweep(uint32 sweepDelayMs, uint32 cacheTimeMs);

        IngestResult Ingest(uint32 messageId, uint32 parts, uint32 partId, std::string partPayload,
            uint32 maxConcurrent, uint32 maxBufferBytes);

    private:
        typedef std::map<uint32, std::string> PartMap;
        struct MessageBuffer
        {
            uint32 Parts = 0;
            uint32 Timer = 0;
            uint32 BufferedBytes = 0;
            PartMap Map;
        };

        typedef std::map<uint32, MessageBuffer> MessageMap;
        MessageMap _messages;
    };
}

#endif
