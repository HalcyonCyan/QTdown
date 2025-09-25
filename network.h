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
        ConnectionAccepted,
        GameStart, 
        PlatformData,       
        GameInitData
    };

    explicit Network(QObject *parent = nullptr);
    ~Network();

    bool createHost(quint16 port);
    bool connectToHost(const QString &ip, quint16 port);
    void disconnect();
    bool isConnected() const;
    void sendGameStart();
 void requestGameStart();
    void sendPlayerPosition(double x, double y, double velocity);
    void sendGameState(const QByteArray &state);
    void sendScoreUpdate(int score);
    void sendLeaderboard(const QList<Account> &leaderboard);

    void setCurrentAccount(const Account &account);
    Account getCurrentAccount() const;

    QString errorString() const;
    QString getLocalIPAddress(); 
    void sendPlatformData(const QByteArray& data);
    void sendGameInitData(const QByteArray& data);
signals:
    void hostCreated(const QString &ip, quint16 port); 
    void connected();
    void disconnected();
    void playerPositionReceived(double x, double y, double velocity);
    void gameStateReceived(const QByteArray &state);
    void scoreUpdateReceived(int score);
    void leaderboardReceived(const QList<Account> &leaderboard);
    void errorOccurred(const QString &error);
    void connectionTimeout();

    void platformDataReceived(const QByteArray& data);
    void gameInitDataReceived(const QByteArray& data);


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
    QString lastError;

    void sendMessage(MessageType type, const QByteArray &data);
    void processMessage(const QByteArray &message);
};

#endif 
