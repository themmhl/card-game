#ifndef PLAYER_H
#define PLAYER_H
#include <QObject>

#include "Match.h"

class Player: public QObject
{
    Q_OBJECT
    QString name, surname,  email, phone_number, username, password_hashed;
    Match history[3];
    public:
    Player(QObject *parent = nullptr):QObject(parent){};

};



#endif //PLAYER_H
