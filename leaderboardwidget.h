#ifndef LEADERBOARDWIDGET_H
#define LEADERBOARDWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "leaderboard.h"

class LeaderboardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LeaderboardWidget(QWidget *parent = nullptr);

private slots:
    void loadLeaderboard();
    void clearLeaderboard();

private:
    QTableWidget *table;
    QPushButton *clearButton;
};

#endif // LEADERBOARDWIDGET_H
