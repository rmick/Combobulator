#ifndef SCORESWINDOW_H
#define SCORESWINDOW_H

#include <QDialog>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "Game.h"
#include "Players.h"

namespace Ui {
class ScoresWindow;
}

class ScoresWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ScoresWindow(QWidget *parent = 0);
    ~ScoresWindow();

private slots:
    void on_btn_Close_clicked();

private:
    Ui::ScoresWindow *ui;

    void addColumnLabels();
    void addPlayers();
    void populateScores();

    QTableWidgetItem *playerTaggerName[25];
    QTableWidgetItem *playerTotalTagsTaken[25];
    QTableWidgetItem *playerTotalHitsLanded[25];
    QTableWidgetItem *playerRating[25];
    QTableWidgetItem *playerSurvivalTime[25];
    QTableWidgetItem *playerZoneTime[25];
    QTableWidgetItem *tagsByPlayer[25] [25];
    QFont             tableFont;
    QFont             headerFont;
    int               rowHeight;

};

#endif // SCORESWINDOW_H
