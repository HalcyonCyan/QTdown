#include "leaderboardwidget.h"
#include <QHeaderView>
#include <QMessageBox>

LeaderboardWidget::LeaderboardWidget(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    table = new QTableWidget(this);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"ID", "Username", "Password", "High Score"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(table);

    clearButton = new QPushButton("清空排行榜", this);
    layout->addWidget(clearButton);

    connect(clearButton, &QPushButton::clicked, this, &LeaderboardWidget::clearLeaderboard);

    loadLeaderboard();
}

void LeaderboardWidget::loadLeaderboard()
{
    QList<Account> accounts = Leaderboard::instance()->getTopScores(100);
    table->setRowCount(accounts.size());

    int row = 0;
    for (const Account &acc : accounts) {
        table->setItem(row, 0, new QTableWidgetItem(acc.getGameId()));
        table->setItem(row, 1, new QTableWidgetItem(acc.getUsername()));
        table->setItem(row, 2, new QTableWidgetItem(acc.getPassword()));
        table->setItem(row, 3, new QTableWidgetItem(QString::number(acc.getHighScore())));
        row++;
    }
}

void LeaderboardWidget::clearLeaderboard()
{
    auto *lb = Leaderboard::instance();
    lb->clear();
    QMessageBox::information(this, "提示", "排行榜已清空！");
    loadLeaderboard();
}
