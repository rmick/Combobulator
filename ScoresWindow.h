#ifndef SCORESWINDOW_H
#define SCORESWINDOW_H

#include <QDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>

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
	explicit ScoresWindow(QWidget *parent = nullptr);
    ~ScoresWindow();

signals:
	void closingScoresWindow();

//void sendSignal();

private slots:
    void on_btn_Close_clicked();

	void on_btn_ViewMode_clicked(bool refreshOnly = false);

	void sendRankReports();

	void on_btn_NameNumberMode_clicked();

	void on_btn_EnlargeFont_clicked();

	void on_btn_ReduceFont_clicked();

private:
    Ui::ScoresWindow *ui;

    void calibrateScreen(int modus);
    void addColumnLabels(int modus);
    void addPlayerRows();
    void populateScores(int modus);
    void setOrder(int modus);


	QTimer			 *timerSendRankReports;
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
	QPointer<DeBrief> deBrief;
	LttoComms		  *lttoComms;
    int               screenWidth;
    int               screenHeight;
    int               rowHeight;
    int               columnWidth;
    int               displayMode;
	bool			  playerNameDisplayInsteadOfNumber;

    const int         COLUMNS_IN_SUMMARY = 8;
};

#endif // SCORESWINDOW_H
