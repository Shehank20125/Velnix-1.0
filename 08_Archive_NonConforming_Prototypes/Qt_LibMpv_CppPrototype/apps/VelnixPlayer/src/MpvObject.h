#pragma once

#include <QQuickFramebufferObject>
#include <QVariant>

extern "C" {
#include <mpv/client.h>
#include <mpv/render_gl.h>
}

class MpvRenderer;

// Wraps a libmpv instance and exposes it to QML as a video-rendering item.
// This is the bridge between the C-style mpv API and Qt's property/signal
// system — everything the UI touches (position, duration, pause, volume)
// goes through here.
class MpvObject : public QQuickFramebufferObject
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
