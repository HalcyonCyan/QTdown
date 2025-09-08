// Network.cpp
#include "network.h"

#include <QDebug>

Network::Network(QObject* parent) : QObject(parent) {
    tcpServer = nullptr;
    tcpSocket = nullptr;
    isHost    = false;
}

Network::~Network() {
    disconnect();
}

bool Network::createHost(quint16 port) {
    if (tcpServer) {
        delete tcpServer;
        tcpServer = nullptr;
    }

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, port)) {
        emit errorOccurred(tcpServer->errorString());
        delete tcpServer;
        tcpServer = nullptr;
        return false;
    }

    connect(tcpServer, &QTcpServer::newConnection, this, &Network::onNewConnection);
    isHost = true;
    return true;
}

bool Network::connectToHost(QString const& ip, quint16 port) {
    if (tcpSocket) {
        delete tcpSocket;
        tcpSocket = nullptr;
    }

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected, this, &Network::connected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Network::onReadyRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Network::onDisconnected);
    connect(tcpSocket,
            QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this,
            &Network::onError);

    tcpSocket->connectToHost(ip, port);
    return true;
}

void Network::onNewConnection() {
    if (tcpServer && tcpServer->hasPendingConnections()) {
        tcpSocket = tcpServer->nextPendingConnection();
        connect(tcpSocket, &QTcpSocket::readyRead, this, &Network::onReadyRead);
        connect(tcpSocket, &QTcpSocket::disconnected, this, &Network::onDisconnected);
        connect(tcpSocket,
                QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
                this,
                &Network::onError);

        // 发送连接接受消息和当前账号信息
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << currentAccount.getGameId() << currentAccount.getUsername() << currentAccount.getHighScore();
        sendMessage(ConnectionAccepted, data);

        emit connected();
    }
}

void Network::onReadyRead() {
    if (tcpSocket) {
        while (tcpSocket->bytesAvailable() > 0) {
            QByteArray message = tcpSocket->readAll();
            processMessage(message);
        }
    }
}

void Network::sendMessage(MessageType type, QByteArray const& data) {
    if (tcpSocket && tcpSocket->state() == QAbstractSocket::ConnectedState) {
        QByteArray packet;
        QDataStream stream(&packet, QIODevice::WriteOnly);
        stream << static_cast<quint32>(type) << data;
        tcpSocket->write(packet);
    }
}

void Network::processMessage(QByteArray const& message) {
    QDataStream stream(message);
    quint32 type;
    QByteArray data;
    stream >> type >> data;

    switch (static_cast<MessageType>(type)) {
    case PlayerPosition: {
        double x, y, velocity;
        QDataStream posStream(data);
        posStream >> x >> y >> velocity;
        emit playerPositionReceived(x, y, velocity);
        break;
    }
    case GameState:
        emit gameStateReceived(data);
        break;
    case ScoreUpdate: {
        int score;
        QDataStream scoreStream(data);
        scoreStream >> score;
        emit scoreUpdateReceived(score);
        break;
    }
    case LeaderboardSync: {
        QList<Account> leaderboard;
        QDataStream lbStream(data);
        while (!lbStream.atEnd()) {
            QString gameId, username;
            int highScore;
            lbStream >> gameId >> username >> highScore;
            Account acc;
            acc.createAccount(username, gameId);
            acc.setHighScore(highScore);
            leaderboard.append(acc);
        }
        emit leaderboardReceived(leaderboard);
        break;
    }
    case ConnectionRequest: {
        // 处理连接请求
        QDataStream connStream(data);
        QString gameId, username;
        int highScore;
        connStream >> gameId >> username >> highScore;

        // 发送接受连接和当前账号信息
        QByteArray response;
        QDataStream responseStream(&response, QIODevice::WriteOnly);
        responseStream << currentAccount.getGameId() << currentAccount.getUsername() << currentAccount.getHighScore();
        sendMessage(ConnectionAccepted, response);
        break;
    }
    case ConnectionAccepted: {
        QDataStream accStream(data);
        QString gameId, username;
        int highScore;
        accStream >> gameId >> username >> highScore;

        Account remoteAccount;
        remoteAccount.createAccount(username, gameId);
        remoteAccount.setHighScore(highScore);

        // 这里可以保存远程账号信息
        break;
    }
    }
}

void Network::sendPlayerPosition(double x, double y, double velocity) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << x << y << velocity;
    sendMessage(PlayerPosition, data);
}

void Network::sendGameState(QByteArray const& state) {
    sendMessage(GameState, state);
}

void Network::sendScoreUpdate(int score) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << score;
    sendMessage(ScoreUpdate, data);
}

void Network::sendLeaderboard(QList<Account> const& leaderboard) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    for (Account const& acc : leaderboard) {
        stream << acc.getGameId() << acc.getUsername() << acc.getHighScore();
    }
    sendMessage(LeaderboardSync, data);
}

void Network::disconnect() {
    if (tcpSocket) {
        tcpSocket->disconnectFromHost();
        if (tcpSocket->state() != QAbstractSocket::UnconnectedState) {
            tcpSocket->waitForDisconnected();
        }
        delete tcpSocket;
        tcpSocket = nullptr;
    }

    if (tcpServer) {
        tcpServer->close();
        delete tcpServer;
        tcpServer = nullptr;
    }

    isHost = false;
}

void Network::onDisconnected() {
    emit disconnected();
}

void Network::onError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error)
    if (tcpSocket) {
        emit errorOccurred(tcpSocket->errorString());
    }
}

bool Network::isConnected() const {
    return tcpSocket && tcpSocket->state() == QAbstractSocket::ConnectedState;
}

void Network::setCurrentAccount(Account const& account) {
    currentAccount = account;
}

Account Network::getCurrentAccount() const {
    return currentAccount;
}
