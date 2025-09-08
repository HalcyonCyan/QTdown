#include "account.h"
#include <QDebug>

Account::Account() : highScore(0)
{
}

Account::~Account()
{
}

bool Account::createAccount(const QString &username, const QString &password, const QString &gameId)
{
    this->username = username;
    this->password = password;
    this->gameId = gameId.isEmpty() ? generateRandomId() : gameId;
    this->highScore = 0;

    QDir dir("accounts");
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "Failed to create accounts directory";
            return false;
        }
    }


    QFile file(getAccountFilePath());
    if (file.exists()) {
        qDebug() << "Account already exists:" << getAccountFilePath();
        return false;
    }

    return saveAccount();
}

bool Account::loadAccount(const QString &gameId)
{
    this->gameId = gameId;

    QFile file(getAccountFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open account file:" << getAccountFilePath();
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "Invalid JSON in account file:" << getAccountFilePath();
        return false;
    }

    QJsonObject obj = doc.object();
    this->username = obj["username"].toString();
    this->password = obj["password"].toString();
    this->highScore = obj["highScore"].toInt();

    return true;
}

bool Account::saveAccount()
{

    QDir dir("accounts");
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "Failed to create accounts directory";
            return false;
        }
    }

    QJsonObject obj;
    obj["gameId"] = gameId;
    obj["username"] = username;
    obj["password"] = password;
    obj["highScore"] = highScore;

    QJsonDocument doc(obj);

    QFile file(getAccountFilePath());
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open account file for writing:" << getAccountFilePath();
        return false;
    }

    file.write(doc.toJson());
    file.close();

    return true;
}

bool Account::deleteAccount()
{
    QFile file(getAccountFilePath());
    return file.remove();
}

bool Account::checkPassword(const QString &password) const
{
    return this->password == password;
}

int Account::getHighScore() const
{
    return highScore;
}

void Account::setHighScore(int score)
{
    highScore = score;
    saveAccount();
}

QString Account::getGameId() const
{
    return gameId;
}

QString Account::getUsername() const
{
    return username;
}

QString Account::getPassword() const
{
    return password;
}

void Account::setUsername(const QString &username)
{
    this->username = username;
    saveAccount();
}

void Account::setPassword(const QString &password)
{
    this->password = password;
    saveAccount();
}

QList<Account> Account::getAllAccounts()
{
    QList<Account> accounts;

    // 确保目录存在
    QDir dir("accounts");
    if (!dir.exists()) {
        dir.mkpath(".");
        return accounts; // 目录不存在，返回空列表
    }

    QStringList filters;
    filters << "*.json";
    dir.setNameFilters(filters);

    QStringList files = dir.entryList();

    for (const QString &file : files) {
        Account acc;
        QString gameId = file.left(file.lastIndexOf('.'));
        if (acc.loadAccount(gameId)) {
            accounts.append(acc);
        }
    }

    return accounts;
}

QString Account::generateRandomId(int length)
{
    QString id;
    const QString possibleCharacters = "0123456789";

    for (int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
        QChar nextChar = possibleCharacters.at(index);
        id.append(nextChar);
    }

    return id;
}

QString Account::getAccountFilePath() const
{
    return QString("accounts/%1.json").arg(gameId);
}
