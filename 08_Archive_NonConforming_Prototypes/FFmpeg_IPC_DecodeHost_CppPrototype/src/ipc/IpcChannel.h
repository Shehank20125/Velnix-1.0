#pragma once
// IpcChannel is the transport boundary between the main app process and the
// isolated Decode Host process (see Threat Model item 1 in the Phase 2 plan).
//
// *** TRANSPORT NOT YET DECIDED ***
// Candidates on the table: named pipes / local Unix domain sockets with a
// custom framed protocol, or gRPC over a local socket. This header defines
// the interface the rest of the codebase should code against so that
// swapping the transport later doesn't ripple through app/ or decode/.
//
// Whoever picks the transport should implement IpcChannel in a new .cpp
// (e.g. NamedPipeChannel.cpp or GrpcChannel.cpp) and update CMakeLists.txt
// to build the chosen one instead of the current no-op StubIpcChannel.

#include <cstdint>
#include <functional>
#include <vector>

namespace mpe {

// Opaque message envelope. Real implementation will need a defined wire
// format (e.g. length-prefixed protobuf, or a hand-rolled struct) -- left
// generic here on purpose.
struct IpcMessage {
    uint32_t type = 0;
    std::vector<uint8_t> payload;
};

class IpcChannel {
public:
    virtual ~IpcChannel() = default;

    // Establish the channel. Returns false on failure.
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;

    // Fire-and-forget send. Real implementation needs to decide sync vs
    // async semantics and backpressure handling.
    virtual bool send(const IpcMessage& msg) = 0;

    // Register a handler invoked on the IPC thread when a message arrives.
    using MessageHandler = std::function<void(const IpcMessage&)>;
    virtual void setMessageHandler(MessageHandler handler) = 0;
};

// Temporary no-op implementation so app/ and decode_host/ link and run
// end-to-end before a real transport is chosen. Replace with a real
// implementation before frame delivery is wired up.
class StubIpcChannel : public IpcChannel {
public:
    bool connect() override { return true; }
    void disconnect() override {}
    bool isConnected() const override { return true; }
    bool send(const IpcMessage&) override { return true; /* no-op */ }
    void setMessageHandler(MessageHandler handler) override {
        handler_ = std::move(handler);
    }

private:
    MessageHandler handler_;
};

} // namespace mpe
