#pragma once

#include <QObject>
#include <QString>

#include <Engine/MediaEngine.h>

class MpvObject;

// Exposes velnix::core::MediaEngine to QML. This is the piece that makes
// Core's architecture "real": QML calls into here (open/play/pause/seek),
// which go through MediaEngine's command-queue + state-machine, which in
// turn drives MpvObject via the IPlaybackBackend interface — rather than
// QML talking to MpvObject directly and bypassing Core entirely.
//
// Position/duration are still read directly off MpvObject's own properties
// in QML (see Main.qml) — those are continuous telemetry, not commands,
// so binding straight to mpv's own change notifications is simpler and
// more responsive than proxying every tick through Core.
class EngineController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)

public:
    explicit EngineController(QObject *parent = nullptr);
    ~EngineController() override;

    // Called once from main.cpp after the QML MpvObject instance exists.
    void attachBackend(MpvObject *backend);

    Q_INVOKABLE void open(const QString &path);
    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void togglePause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void seek(double seconds);

    QString state() const;

signals:
    void stateChanged();
    // Emitted when a MediaEngine call fails (e.g. backend not attached
    // yet, or an invalid state transition) — lets QML surface real errors
    // instead of failing silently.
    void engineError(const QString &message);

private:
    void reportIfFailed(const velnix::core::EngineResult &result);

    velnix::core::MediaEngine m_engine;
    MpvObject *m_backend = nullptr;
};
