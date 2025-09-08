// leaderboard.h
#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <QList>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include "account.h"

class Leaderboard
{
public:
    Leaderboard();
    ~Leaderboard();

    bool load();
    bool save();

    void addAccount(const Account &account);
    void updateAccount(const Account &account);
    void mergeLeaderboard(const QList<Account> &otherLeaderboard);

    QList<Account> getTopScores(int count = 10) const;
    int getRank(const QString &gameId) const;

    static Leaderboard* instance();

private:
    QList<Account> accounts;

    void sort();
};

#endif // LEADERBOARD_H
