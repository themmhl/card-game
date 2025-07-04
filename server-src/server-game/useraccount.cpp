#include "useraccount.h"
#include <QCryptographicHash>
#include <QJsonObject>
#include <QJsonArray>

UserAccount::UserAccount() {}
UserAccount::UserAccount(const QString &username, const QString &plainPassword, const QString &email)
{
    this->username = username;
    setPassword(plainPassword);
    this->email = email;
}

QJsonObject UserAccount::toJson() const {
    QJsonObject obj;
    obj["username"] = username;
    obj["hashedPassword"] = hashedPassword;
    obj["email"] = email;
    obj["phoneNumber"] = phoneNumber;
    obj["firstName"] = firstName;
    obj["lastName"] = lastName;


    return obj;
}

UserAccount UserAccount::fromJson(const QJsonObject& obj) {
    UserAccount acc;
    acc.username = obj["username"].toString();
    acc.hashedPassword = obj["hashedPassword"].toString();
    acc.email = obj["email"].toString();
    acc.phoneNumber = obj["phoneNumber"].toString();
    acc.firstName = obj["firstName"].toString();
    acc.lastName = obj["lastName"].toString();

    return acc;
}
QString UserAccount::getUsername() const {
    return username; }
QString UserAccount::getHashedPassword() const {
    return hashedPassword; }
QString UserAccount::getEmail() const {
    return email; }
QString UserAccount::getPhoneNumber() const {
    return phoneNumber; }
QString UserAccount::getFirstName() const {
    return firstName; }
QString UserAccount::getLastName() const {
    return lastName; }
QList<GameHistory> UserAccount::getGameHistory() const {
        return gameHistory; }

void UserAccount::setUsername(const QString &username) { this->username = username; }
void UserAccount::setEmail(const QString &email) { this->email = email; }
void UserAccount::setPhoneNumber(const QString &phoneNumber) { this->phoneNumber = phoneNumber; }
void UserAccount::setFirstName(const QString &firstName) { this->firstName = firstName; }
void UserAccount::setLastName(const QString &lastName) { this->lastName = lastName; }
void UserAccount::setPassword(const QString &password){
    this->hashedPassword = password;
}

void UserAccount::setHashedPassword(const QString &password)
{
    hashedPassword = password;
}

bool UserAccount::checkPassword(const QString &plainPassword) const {

    return this->hashedPassword == plainPassword;
}


void UserAccount::addGameToHistory(const GameHistory &history) {
    gameHistory.append(history);

    while (gameHistory.size() > 3) {
        gameHistory.removeFirst();
    }
}
