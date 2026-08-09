#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
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

    return app.exec();
}
