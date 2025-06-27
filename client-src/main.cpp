#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Player.h"
#include "Card.h"
#include "Match.h"
#include "Round.h"
#include "ServerHandler.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("card-game", "Main");

    return app.exec();
}
