#ifndef USERACCOUNT_H
#define USERACCOUNT_H
#include <QString>
#include <QList>
#include "gamehistory.h"
class UserAccount
{
public:
    UserAccount();
    UserAccount(const QString &username, const QString &plainPassword, const QString &email);
    QString getUsername() const;
    QString getHashedPassword() const;
    QString getEmail() const;
    QString getPhoneNumber() const;
    QString getFirstName() const;
    QString getLastName() const;
    void setUsername(const QString &username);
    void setEmail(const QString &email);
    void setPhoneNumber(const QString &phoneNumber);
    void setFirstName(const QString &firstName);
    void setLastName(const QString &lastName);
    void setHashedPassword(const QString& password);
    void setPassword(const QString &password);
    bool checkPassword(const QString &plainPassword) const;
    QList<GameHistory> getGameHistory() const;
    void addGameToHistory(const GameHistory &history);
private:
    QString username;
    QString hashedPassword;
    QString email;
    QString phoneNumber;
    QString firstName;
    QString lastName;
    QList<GameHistory> gameHistory;

};

#endif // USERACCOUNT_H
