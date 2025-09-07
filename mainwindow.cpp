#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , gameWindow(nullptr)
    , gameRunning(false)
{
    ui->setupUi(this);

    // 设置窗口标题和大小
    setWindowTitle(tr("DeepRock - Platform Jumping Game"));
    setFixedSize(400, 500);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (gameWindow) delete gameWindow;
}
void MainWindow::on_startButton_clicked()
{
    qDebug() << "Start button clicked";

    if (!gameWindow) {
        qDebug() << "Creating new GameWindow";
        gameWindow = new GameWindow(nullptr);
        connect(gameWindow, &GameWindow::gameFinished, this, &MainWindow::gameFinished);
    }

    qDebug() << "Starting game";
    gameWindow->startGame();
    qDebug() << "Showing game window";
    gameWindow->show();
    qDebug() << "Hiding main window";
    this->hide();
    gameRunning = true;
}

void MainWindow::on_restartButton_clicked()
{
    if (gameWindow && gameRunning) {
        gameWindow->restartGame();
        gameWindow->show();
        this->hide();
    }
}

void MainWindow::on_settingsButton_clicked()
{
    QMessageBox::information(this, tr("Game Settings"), tr("Game settings will be added in future versions"));
}

void MainWindow::on_exitButton_clicked()
{
    QApplication::quit();
}

void MainWindow::gameFinished()
{
    this->show();
    gameRunning = false;
}
