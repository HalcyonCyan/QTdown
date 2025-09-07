#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gamewindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();
    void on_restartButton_clicked();
    void on_settingsButton_clicked();
    void on_exitButton_clicked();
    void gameFinished();

private:
    Ui::MainWindow *ui;
    GameWindow *gameWindow;
    bool gameRunning;
};

#endif // MAINWINDOW_H
