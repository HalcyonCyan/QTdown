// account.h
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDir>
#include <QRandomGenerator>
#include <QDebug>

class Account
{
public:
    Account();
    ~Account();

    bool createAccount(const QString &username, const QString &password, const QString &gameId = "");
    bool loadAccount(const QString &gameId);
    bool saveAccount();
    bool deleteAccount();
    bool checkPassword(const QString &password) const;

    // 获取和设置最高分
    int getHighScore() const;
    void setHighScore(int score);

    // 获取账号信息
    QString getGameId() const;
    QString getUsername() const;
    QString getPassword() const;

    // 设置账号信息
    void setUsername(const QString &username);
    void setPassword(const QString &password);

    // 静态方法：获取所有账号
    static QList<Account> getAllAccounts();
    static QString generateRandomId(int length = 6);

private:
    QString gameId;
    QString username;
    QString password;
    int highScore;

    QString getAccountFilePath() const;
};

#endif // ACCOUNT_H
