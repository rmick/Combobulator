#ifndef SCORESWINDOW_H
#define SCORESWINDOW_H

#include <QDialog>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "Game.h"
#include "Players.h"
#include "Defines.h"
#include "DeBrief.h"

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

    void on_btn_ViewMode_clicked();

private:
    Ui::ScoresWindow *ui;

    void calibrateScreen(int modus);
    void addColumnLabels(int modus);
    void addPlayerRows();
    void populateScores(int modus);
    void setOrder(int modus);

    QTableWidgetItem *playerNumber[25];
    QTableWidgetItem *playerTaggerName[25];
    QTableWidgetItem *playerRanking[25];
    QTableWidgetItem *playerTotalTagsTaken[25];
    QTableWidgetItem *playerTotalHitsLanded[25];
    QTableWidgetItem *playerScore[25];
    QTableWidgetItem *playerSurvivalTime[25];
    QTableWidgetItem *playerZoneTime[25];
    QTableWidgetItem *tagsByPlayer[25] [25];
    QFont             tableFont;
    QFont             headerFont;
    int               screenWidth;
    int               screenHeight;
    int               rowHeight;
    int               columnWidth;
    int               displayMode;

    const int         COLUMNS_IN_SUMMARY = 8;

};

#endif // SCORESWINDOW_H
