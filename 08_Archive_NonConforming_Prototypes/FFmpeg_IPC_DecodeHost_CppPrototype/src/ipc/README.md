# IPC transport — OPEN DECISION

`IpcChannel` is the interface everything codes against. The concrete
transport is not chosen yet. Options considered:

1. **Named pipes / local sockets, custom framed protocol**
   - Pros: minimal dependency footprint, full control over framing and
     latency, easy to pair with `DuplicateHandle` for passing D3D11 shared
     texture handles alongside control messages.
   - Cons: have to hand-roll message framing, versioning, and (de)serialization.

2. **gRPC (over local socket)**
   - Pros: schema via protobuf, generated (de)serialization, built-in
     streaming support (useful for a continuous frame-ready notification
     stream).
   - Cons: heavier dependency, protobuf messages aren't a natural fit for
     passing raw OS handles (would still need a side-channel for that part).

Recommendation for whoever picks this up: prototype the shared-D3D11-texture
handle-passing path first under whichever transport is chosen — that's the
part most likely to constrain the decision, since it's Windows-handle-passing
that has to ride alongside whatever control-message format is picked.

Until this is decided, `StubIpcChannel` in `IpcChannel.h` is a no-op
implementation so `app` and `decode_host` build and run end-to-end.
