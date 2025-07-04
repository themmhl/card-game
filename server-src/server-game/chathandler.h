#ifndef CHATHANDLER_H
#define CHATHANDLER_H
#include <QObject>
#include <QJsonObject>
#include "player.h"
class ChatHandler : public QObject {
    Q_OBJECT
    public:
    explicit ChatHandler(QObject *parent = nullptr);
    signals:
    void broadcastMessage(const QJsonObject& message);
public slots:
void processMessage(Player* sender,const QJsonObject& message);
};
#endif //CHATHANDLER_H