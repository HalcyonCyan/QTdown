
#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include "network.h"

class WaitingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitingDialog(Network* network, bool isHost, QWidget *parent = nullptr);
    ~WaitingDialog();

signals:
    void gameStarted();
    void cancelled();

private slots:
    void updateStatus();
    void onConnected();
    void onError(const QString &error);
    void onCancelClicked();

private:
    Network* network;
    bool isHost;
    QLabel *statusLabel;
    QLabel *ipLabel;
    QPushButton *cancelButton;
    QTimer *statusTimer;
    int elapsedSeconds;
};

#endif 
