#include "chathandler.h"

#include "sharedDefs.h"

ChatHandler::ChatHandler(QObject *parent) : QObject(parent) {}
void ChatHandler::processMessage(Player* sender,const QJsonObject &message) {
    qInfo()<<"got a new message";
    QJsonObject payload;
    payload["sender"] = sender->getUsername();
    payload["content"] = message["payload"]["content"];
    emit broadcastMessage(payload);
}