
#include "waitingdialog.h"
#include <QApplication>
#include <QStyle>
#include <QScreen>

WaitingDialog::WaitingDialog(Network* network, bool isHost, QWidget *parent)
    : QDialog(parent), network(network), isHost(isHost), elapsedSeconds(0)
{
    setWindowTitle(isHost ? tr("等待玩家连接") : tr("连接中..."));
    setFixedSize(300, 200);
    setStyleSheet("QLabel { color: black; } QPushButton { color: black; }");
    setModal(true);

    
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    QVBoxLayout *layout = new QVBoxLayout(this);

    
    if (isHost) {
        QString ipAddress = network->getLocalIPAddress();
        ipLabel = new QLabel(tr("主机IP: %1").arg(ipAddress), this);
        ipLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(ipLabel);
    }

    
    statusLabel = new QLabel(isHost ? tr("等待玩家连接...") : tr("正在连接主机..."), this);
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);

    
    cancelButton = new QPushButton(tr("取消"), this);
    connect(cancelButton, &QPushButton::clicked, this, &WaitingDialog::onCancelClicked);
    layout->addWidget(cancelButton);

    
    connect(network, &Network::connected, this, &WaitingDialog::onConnected);
    connect(network, &Network::errorOccurred, this, &WaitingDialog::onError);

    
    statusTimer = new QTimer(this);
    connect(statusTimer, &QTimer::timeout, this, &WaitingDialog::updateStatus);
    statusTimer->start(1000); 
}

WaitingDialog::~WaitingDialog()
{
}

void WaitingDialog::updateStatus()
{
    elapsedSeconds++;
    if (isHost) {
        statusLabel->setText(tr("等待玩家连接... %1秒").arg(elapsedSeconds));
    } else {
        QString dots;
        for (int i = 0; i < (elapsedSeconds % 4); i++) {
            dots += ".";
        }
        statusLabel->setText(tr("正在连接主机%1").arg(dots));
    }
}

void WaitingDialog::onConnected()
{
    statusTimer->stop();
    accept();
    emit gameStarted();
}

void WaitingDialog::onError(const QString &error)
{
    statusTimer->stop();
    reject();
    
}

void WaitingDialog::onCancelClicked()
{
    statusTimer->stop();
    network->disconnect();
    reject();
    emit cancelled();
}
