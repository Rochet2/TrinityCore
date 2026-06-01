# CAIO / AIO message format

This matches [Rochet2 AIO](https://github.com/Rochet2/AIO) block layout as used by CAIO and `smallfolk_cpp`.

## Serialized message

One whisper addon payload is a **smallfolk** dump of an array of blocks:

```lua
{
  { n, scriptKey, handlerKey, arg1, arg2, ... },
  { n, scriptKey2, handlerKey2, ... },
}
```

- `n` — number of arguments **including** `handlerKey` (CAIO sets `block[1]` accordingly). Server rejects `n > 15` (stock `AIO_SERVER` limit).
- `scriptKey` — block name (`AIO`, `AIOExample`, …); must be registered with `AIO.RegisterEvent` on the client or `AIOScript` on the server.
- Arguments from index 4 onward are handler parameters (`unpack(data, 3, n+2)` on the Lua side).

Use `AIOMsg` / `AIO.Msg():Add(...)` rather than building tables manually.

## Wire encoding (3.3.5)

On TrinityCore, AIO uses **`CHAT_MSG_WHISPER` with `LANG_ADDON`** (not a separate addon channel packet). The client still sees **`CHAT_MSG_ADDON`**; this matches stock `AIO.lua` server behaviour.

- Prefix: `S` + `AIO.Prefix` + `\t` (server→client) or `C` + prefix + `\t` (client→server).
- Short message: two bytes `\1\1` then the smallfolk string.
- Long message: 2-byte message id + 2-byte part count + 2-byte part id + chunk.
- Each whisper packet must fit in **255 bytes** total (WoW addon limit). Configure `AIO.MsgMaxLen` (default **255**). Long payloads split using `chunkLen = MsgMaxLen - headerBytes` where `headerBytes = 1 + len(prefix) + 1 + 6`.

## Version handshake

On init, client sends `AIO` / `Init` with protocol version **1.75**. Server defines `AIO_VERSION` / `AIO_VERSION_STRING` in `src/server/game/AIO/AIO.h`.

## Compression / obfuscation

Not implemented on this branch. Addon files are sent with an `U` (uncompressed) prefix only.
