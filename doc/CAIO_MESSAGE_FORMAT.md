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

- `n` — number of arguments **including** `handlerKey` (CAIO sets `block[1]` accordingly).
- `scriptKey` — block name (`AIO`, `AIOExample`, …); must be registered with `AIO.RegisterEvent` on the client or `AIOScript` on the server.
- Arguments from index 4 onward are handler parameters (`unpack(data, 3, n+2)` on the Lua side).

Use `AIOMsg` / `AIO.Msg():Add(...)` rather than building tables manually.

## Wire encoding (3.3.5)

- Prefix: `S` + `AIO.Prefix` + `\t` (server→client) or `C` + prefix + `\t` (client→server).
- Short message: two bytes `\1\1` then the smallfolk string.
- Long message: `messageId` (2) + `parts` (2) + `partId` (2) + chunk; chunk size is `AIO.MsgMaxLen` in `worldserver.conf` (default **2560**, same as `AIO_MsgLen` in server `AIO.lua`).

## Version handshake

On init, client sends `AIO` / `Init` with `AIO_VERSION`. Server must define the same `AIO_VERSION` in `World.h` (currently **1.75**).
