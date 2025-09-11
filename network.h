#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>
#include "account.h"

class Network : public QObject
{
    Q_OBJECT
public:
    enum MessageType {
        PlayerPosition,
        GameState,
        ScoreUpdate,
        LeaderboardSync,
        ConnectionRequest,
        ConnectionAccepted
    };

    explicit Network(QObject *parent = nullptr);
    ~Network();

    bool createHost(quint16 port);
    bool connectToHost(const QString &ip, quint16 port);
    void disconnect();
    bool isConnected() const;

    void sendPlayerPosition(double x, double y, double velocity);
    void sendGameState(const QByteArray &state);
    void sendScoreUpdate(int score);
    void sendLeaderboard(const QList<Account> &leaderboard);

    void setCurrentAccount(const Account &account);
    Account getCurrentAccount() const;

    QString errorString() const; // 添加此函数

signals:
    void connected();
    void disconnected();
    void playerPositionReceived(double x, double y, double velocity);
    void gameStateReceived(const QByteArray &state);
    void scoreUpdateReceived(int score);
    void leaderboardReceived(const QList<Account> &leaderboard);
    void errorOccurred(const QString &error);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);

private:
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    bool isHost;
    Account currentAccount;
    QString lastError; // 添加此变量

    void sendMessage(MessageType type, const QByteArray &data);
    void processMessage(const QByteArray &message);
};

#endif // NETWORK_H
