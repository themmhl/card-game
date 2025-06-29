#ifndef PLAYER_H
#define PLAYER_H
#include <QObject>

#include "Match.h"

class Player: public QObject
{
    Q_OBJECT
public:
    QString name, surname,  email, phone_number, username, password_hashed;
    bool turn;
    Match history[3];

    Player(QObject *parent = nullptr):QObject(parent){};

};



#endif //PLAYER_H
