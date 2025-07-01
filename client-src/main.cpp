#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Player.h"
#include "Card.h"
#include "Match.h"
#include "Round.h"
#include "ServerHandler.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    ServerHandler sh(nullptr);
    sh.set_port(10);
    engine.rootContext()->setContextProperty("server", &sh);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("card-game", "Main");

    return app.exec();
}
