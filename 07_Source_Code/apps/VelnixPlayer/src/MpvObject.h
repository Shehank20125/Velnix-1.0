#pragma once

#include <QQuickFramebufferObject>
#include <QVariant>

#include <Interfaces/IPlaybackBackend.h>

extern "C" {
#include <mpv/client.h>
#include <mpv/render_gl.h>
}

class MpvRenderer;

// Wraps a libmpv instance and exposes it to QML as a video-rendering item.
// This is the bridge between the C-style mpv API and Qt's property/signal
// system — everything the UI touches (position, duration, pause, volume)
// goes through here.
//
// Also implements velnix::core::IPlaybackBackend, so MediaEngine (in Core)
// can drive this same mpv instance — commands issued through MediaEngine
// (Play/Pause/Open/Seek) and direct QML property bindings both end up
// operating on the one mpv_handle, never two competing instances.
class MpvObject : public QQuickFramebufferObject, public velnix::core::IPlaybackBackend
{
    Q_OBJECT
    Q_PROPERTY(double position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(bool pause READ pause WRITE setPause NOTIFY pauseChanged)
    Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaTitleChanged)

public:
    explicit MpvObject(QQuickItem *parent = nullptr);
    ~MpvObject() override;

    Renderer *createRenderer() const override;

    // Generic escape hatches — lets QML call any mpv command/property
    // without needing a new C++ method for every feature (e.g. filters,
    // playlist ops, subtitle track switching).
    Q_INVOKABLE void command(const QVariant &params);
    Q_INVOKABLE void setMpvProperty(const QString &name, const QVariant &value);
    Q_INVOKABLE QVariant getMpvProperty(const QString &name) const;

    Q_INVOKABLE void loadFile(const QString &path);

    // velnix::core::IPlaybackBackend — lets MediaEngine drive this same
    // mpv instance via Core's Path/Milliseconds types, independent of the
    // QML-facing QString/double properties above.
    void Open(const velnix::core::Path &file) override;
    void Close() override;
    void Play() override;
    void Pause() override;
    void Stop() override;
    void Seek(velnix::core::Milliseconds position) override;
    velnix::core::Milliseconds GetPosition() const override;
    velnix::core::Milliseconds GetDuration() const override;
    bool IsPaused() const override;

    double position() const { return m_position; }
    void setPosition(double value);

    double duration() const { return m_duration; }
    bool pause() const { return m_pause; }
    void setPause(bool value);

    QString mediaTitle() const { return m_mediaTitle; }

signals:
    void positionChanged();
    void durationChanged();
    void pauseChanged();
    void mediaTitleChanged();

private slots:
    void onMpvEvents();

private:
    void handleEvent(mpv_event *event);

    mpv_handle *m_mpv = nullptr;
    mpv_render_context *m_mpvGL = nullptr;

    double m_position = 0.0;
    double m_duration = 0.0;
    bool m_pause = false;
    QString m_mediaTitle;

    friend class MpvRenderer;
};
