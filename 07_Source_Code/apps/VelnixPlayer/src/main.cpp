#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "EngineController.h"
#include "MpvObject.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("Velnix");
    app.setApplicationName("Velnix Player");

    qmlRegisterType<MpvObject>("VelnixPlayer", 1, 0, "MpvObject");

    QQmlApplicationEngine engine;
    engine.loadFromModule("VelnixPlayer", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    // The QML MpvObject instance ("mpv" in Main.qml) now exists — find it
    // and wire Core's MediaEngine to drive it. This is the connection
    // point: QML calls Engine.play()/pause()/open() (see EngineController),
    // which flow through MediaEngine's command queue and state machine,
    // which calls back into this same MpvObject via IPlaybackBackend.
    MpvObject *mpv = engine.rootObjects().first()->findChild<MpvObject *>("mpv");
    if (!mpv)
        qFatal("Could not find MpvObject 'mpv' in QML — check objectName in Main.qml");

    auto *controller = new EngineController(&app);
    controller->attachBackend(mpv);
    engine.rootContext()->setContextProperty("Engine", controller);

    return app.exec();
}
