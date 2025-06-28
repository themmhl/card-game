#ifndef GAMEHISTORY_H
#define GAMEHISTORY_H
#include <QString>
#include <QDateTime>

class GameHistory
{
public:
    GameHistory();
    GameHistory(const QString &opponentUsername, const QString &finalResult, const QList<QString> &roundResults);
    QString getOpponentUsername() const;
    QString getFinalResult() const;
    QList<QString> getRoundResults() const;
    QDateTime getGameDateTime() const;

    void setOpponentUsername(const QString &username);
    void setFinalResult(const QString &result);
    void setRoundResults(const QList<QString> &results);
    void setGameDateTime(const QDateTime &dateTime);

private:
    QString opponent;
    QString finalResult;
    QList<QString> roundResults;
    QDateTime gameDate;

};

#endif // GAMEHISTORY_H
