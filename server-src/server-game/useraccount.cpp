#include "useraccount.h"
#include <QCryptographicHash>


UserAccount::UserAccount() {}
UserAccount::UserAccount(const QString &username, const QString &plainPassword, const QString &email)
{
    this->username = username;
    setPassword(plainPassword);
    this->email = email;
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
    QByteArray passwordBytes = password.toUtf8();
    QByteArray hashedBytes = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);
    this->hashedPassword = hashedBytes.toHex();
}

void UserAccount::setHashedPassword(const QString &password)
{
    hashedPassword = password;
}

bool UserAccount::checkPassword(const QString &plainPassword) const {

    QString hashedInputPassword =QString(QCryptographicHash::hash(plainPassword.toUtf8(), QCryptographicHash::Sha256).toHex());
    return this->hashedPassword == hashedInputPassword;
}


void UserAccount::addGameToHistory(const GameHistory &history) {
    gameHistory.append(history);

    while (gameHistory.size() > 3) {
        gameHistory.removeFirst();
    }
}
