#pragma once

/**
 * @file IPlaybackBackend.h
 * @brief Contract between MediaEngine and whatever actually plays media.
 *
 * Core stays framework-agnostic (no Qt, no mpv) by only depending on this
 * interface. The concrete implementation — currently MpvObject, wrapping
 * libmpv — lives in the app layer where those dependencies are allowed.
 * If the engine is ever swapped for a custom FFmpeg backend, only a new
 * implementation of this interface is needed; MediaEngine doesn't change.
 */

#include "../Common/Types.h"

namespace velnix::core
{

class IPlaybackBackend
{
public:

    virtual ~IPlaybackBackend() = default;

    virtual void Open(const Path& file) = 0;

    virtual void Close() = 0;

    virtual void Play() = 0;

    virtual void Pause() = 0;

    virtual void Stop() = 0;

    virtual void Seek(Milliseconds position) = 0;

    [[nodiscard]] virtual Milliseconds GetPosition() const = 0;

    [[nodiscard]] virtual Milliseconds GetDuration() const = 0;

    [[nodiscard]] virtual bool IsPaused() const = 0;
};

}
