#include "gamehistory.h"

GameHistory::GameHistory() {
    gameDate = QDateTime::currentDateTime();
}

GameHistory::GameHistory(const QString &opponentUsername, const QString &finalResult, const QList<QString> &roundResults){
    this->opponent=opponentUsername;
    this->finalResult = finalResult;
    this->roundResults = roundResults;
    this->gameDate = QDateTime::currentDateTime();
}

QString GameHistory::getOpponentUsername() const
{
    return this->opponent;
}

QString GameHistory::getFinalResult() const
{
    return this->finalResult;
}

QList<QString> GameHistory::getRoundResults() const
{
    return this->roundResults;
}

QDateTime GameHistory::getGameDateTime() const
{
    return this->gameDate;
}

void GameHistory::setOpponentUsername(const QString &username)
{
    this->opponent = username;
}

void GameHistory::setFinalResult(const QString &result)
{
    this->finalResult = result;
}

void GameHistory::setRoundResults(const QList<QString> &results)
{
    this->roundResults = results;
}

void GameHistory::setGameDateTime(const QDateTime &dateTime)
{
    this->gameDate = dateTime;
}
