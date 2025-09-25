
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

    
    int getHighScore() const;
    void setHighScore(int score);

    
    QString getGameId() const;
    QString getUsername() const;
    QString getPassword() const;

    
    void setUsername(const QString &username);
    void setPassword(const QString &password);

    
    static QList<Account> getAllAccounts();
    static QString generateRandomId(int length = 6);

private:
    QString gameId;
    QString username;
    QString password;
    int highScore;

    QString getAccountFilePath() const;
};

#endif 
