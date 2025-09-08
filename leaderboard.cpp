#include "leaderboard.h"
#include <QDebug>
#include <algorithm>

Leaderboard::Leaderboard()
{
    load();
}

Leaderboard::~Leaderboard()
{
    save();
}

bool Leaderboard::load()
{
    QFile file("leaderboard.json");
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return false;
    }

    QJsonArray array = doc.array();
    accounts.clear();

    for (int i = 0; i < array.size(); i++) {
        QJsonObject obj = array[i].toObject();
        Account acc;
        acc.createAccount(
            obj["username"].toString(),
            obj["password"].toString(),
            obj["gameId"].toString()
            );
        acc.setHighScore(obj["highScore"].toInt());
        accounts.append(acc);
    }

    sort();
    return true;
}

bool Leaderboard::save()
{
    QJsonArray array;

    for (const Account &acc : accounts) {
        QJsonObject obj;
        obj["gameId"]   = acc.getGameId();
        obj["username"] = acc.getUsername();
        obj["password"] = acc.getPassword();
        obj["highScore"] = acc.getHighScore();
        array.append(obj);
    }

    QJsonDocument doc(array);

    QFile file("leaderboard.json");
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(doc.toJson());
    file.close();

    return true;
}

void Leaderboard::addAccount(const Account &account)
{
    for (int i = 0; i < accounts.size(); i++) {
        if (accounts[i].getGameId() == account.getGameId()) {
            if (account.getHighScore() > accounts[i].getHighScore()) {
                accounts[i].setHighScore(account.getHighScore());
            }
            sort();
            return;
        }
    }

    accounts.append(account);
    sort();
}

void Leaderboard::updateAccount(const Account &account)
{
    addAccount(account);
}

void Leaderboard::mergeLeaderboard(const QList<Account> &otherLeaderboard)
{
    for (const Account &acc : otherLeaderboard) {
        addAccount(acc);
    }
    save();
}

QList<Account> Leaderboard::getTopScores(int count) const
{
    if (count >= accounts.size()) {
        return accounts;
    }

    QList<Account> result;
    for (int i = 0; i < count; i++) {
        result.append(accounts[i]);
    }
    return result;
}

int Leaderboard::getRank(const QString &gameId) const
{
    for (int i = 0; i < accounts.size(); i++) {
        if (accounts[i].getGameId() == gameId) {
            return i + 1;
        }
    }
    return -1;
}

void Leaderboard::sort()
{
    std::sort(accounts.begin(), accounts.end(),
              [](const Account &a, const Account &b) {
                  return a.getHighScore() > b.getHighScore();
              });
}

void Leaderboard::clear()
{
    accounts.clear();
    QFile::remove("leaderboard.json");
}

Leaderboard* Leaderboard::instance()
{
    static Leaderboard inst;
    return &inst;
}
