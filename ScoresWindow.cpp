#include "ScoresWindow.h"
#include "ui_ScoresWindow.h"

#include <QLayout>
#include <QDebug>
#include <QScreen>

ScoresWindow::ScoresWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScoresWindow)
{
    ui->setupUi(this);

    calibrateScreen();
    addColumnLabels();
    addPlayerRows();
    populateScores();
    ui->scoreTable->horizontalHeader()->setSortIndicatorShown(true);
    ui->scoreTable->horizontalHeader()->setSortIndicator(1, Qt::DescendingOrder);
    ui->scoreTable->setSortingEnabled(true);
}

ScoresWindow::~ScoresWindow()
{
    delete ui;
}

void ScoresWindow::on_btn_Close_clicked()
{
    deleteLater();
}

int cellWidth = 0;
int cellHeight = 0;
void ScoresWindow::calibrateScreen()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int screenWidth   = screenGeometry.width();
    int screenHeight  = screenGeometry.height() - 75;

    cellWidth   = screenWidth  / (gameInfo.getNumberOfPlayersInGame() + 6);
    cellHeight  = screenHeight / (gameInfo.getNumberOfPlayersInGame() + 1);

    qDebug() << "ScoresWindow::calibrateScreen() W x H" << cellWidth << cellHeight;

    //TODO: Set font and table size to match screen resolution.
    if(cellWidth < 25)                      tableFont.setPointSize(8);
    if(cellWidth > 24 && cellWidth < 50)    tableFont.setPointSize(12);
    if(cellWidth > 49 && cellWidth < 75)    tableFont.setPointSize(16);
    if(cellWidth > 74 && cellWidth < 100)   tableFont.setPointSize(20);
    if(cellWidth > 100)                     tableFont.setPointSize(24);

    ui->scoreTable->setColumnCount(6 + gameInfo.getNumberOfPlayersInGame());
    ui->scoreTable->setRowCount(gameInfo.getNumberOfPlayersInGame());

    tableFont.setFamily("Verdana");
    rowHeight   = cellHeight - 5;
    columnWidth = cellWidth - 7;

    headerFont.setPointSize(12);
}

void ScoresWindow::addColumnLabels()
{
    int columnIndex = 0;
    QTableWidgetItem *PlayersName = new QTableWidgetItem("Players\nName");
    PlayersName->setFont(headerFont);
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, PlayersName);
    ui->scoreTable->setColumnWidth(columnIndex-1, columnWidth);

    QTableWidgetItem *Score = new QTableWidgetItem("Score");
    Score->setFont(headerFont);
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, Score);
    ui->scoreTable->setColumnWidth(columnIndex-1, columnWidth);

    QTableWidgetItem *HitsTaken = new QTableWidgetItem("Hits\nTaken");
    HitsTaken->setFont(headerFont);
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, HitsTaken);
    ui->scoreTable->setColumnWidth(columnIndex-1, columnWidth);

    QTableWidgetItem *HitsLanded = new QTableWidgetItem("Hits\nLanded");
    HitsLanded->setFont(headerFont);
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, HitsLanded);
    ui->scoreTable->setColumnWidth(columnIndex-1, columnWidth);

    QTableWidgetItem *SurvivalTime = new QTableWidgetItem("Survival\nTime");
    SurvivalTime->setFont(headerFont);
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, SurvivalTime);
    ui->scoreTable->setColumnWidth(columnIndex-1, columnWidth);

    QTableWidgetItem *ZoneTime = new QTableWidgetItem("Zone\nTime");
    ZoneTime->setFont(headerFont);
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, ZoneTime);
    ui->scoreTable->setColumnWidth(columnIndex-1, columnWidth);

    int offsetForNonPlayers = 0;
    for (int index = 1; index < 25; index++)
    {
        if(gameInfo.getIsThisPlayerInTheGame(index) == true)
        {
            ui->scoreTable->setHorizontalHeaderItem(columnIndex++, new QTableWidgetItem(playerInfo[index].getPlayerName()));
            ui->scoreTable->setColumnWidth(columnIndex-1, cellWidth);
        }
        else offsetForNonPlayers++;
    }
}

void ScoresWindow::addPlayerRows()
{
    int offsetForNonPlayers = 0;
    for (int index = 1; index < 25; index++)
    {
        if(gameInfo.getIsThisPlayerInTheGame(index) == true)
        {
            int thisRow = (index-1) - offsetForNonPlayers;
            ui->scoreTable->setRowHeight(thisRow, rowHeight);
            playerTaggerName[index] = new QTableWidgetItem(playerInfo[index].getPlayerName());
            playerTaggerName[index]->setFont(tableFont);
            ui->scoreTable->setItem(thisRow, 0, playerTaggerName[index]);
        }
        else offsetForNonPlayers++;
    }
}

void ScoresWindow::populateScores()
{
    int columnIndex;
    int offsetForNonPlayers = 0;
    for(int thisPlayer = 1; thisPlayer < 25; thisPlayer++)
    {
        if (gameInfo.getIsThisPlayerInTheGame(thisPlayer) == true)
        {
            columnIndex = 1;
            int thisRow = (thisPlayer-1) - offsetForNonPlayers;
            //Populate the Rankings for each player
            int playerScoring = playerInfo[thisPlayer].getGameScore();
            playerScore[thisPlayer] = new QTableWidgetItem(QString::number(playerScoring));
            playerScore[thisPlayer]->setTextAlignment(Qt::AlignCenter);
            playerScore[thisPlayer]->setFont(tableFont);
            ui->scoreTable->setItem(thisRow, columnIndex++, playerScore[thisPlayer]);

            //Populate the Total Tags for each player
            int totalTagsTaken = playerInfo[thisPlayer].getTagsTaken(0); // Index 0 is total tags taken (and should equal the sum of all the other indexes)
            playerTotalTagsTaken[thisPlayer] = new QTableWidgetItem(QString::number(0-totalTagsTaken));
            playerTotalTagsTaken[thisPlayer]->setTextAlignment(Qt::AlignCenter);
            playerTotalTagsTaken[thisPlayer]->setFont(tableFont);
            ui->scoreTable->setItem(thisRow, columnIndex++, playerTotalTagsTaken[thisPlayer]);

            //Populate the Total Hits Landed on currentPlayer by every other player
            playerTotalHitsLanded[thisPlayer] = new QTableWidgetItem(playerInfo[thisPlayer].getTotalTagsLanded(thisPlayer));
            playerTotalHitsLanded[thisPlayer]->setTextAlignment(Qt::AlignCenter);
            playerTotalHitsLanded[thisPlayer]->setFont(tableFont);
            ui->scoreTable->setItem(thisRow, columnIndex++, playerTotalHitsLanded[thisPlayer]);

            //Populate the Survival Time
            playerSurvivalTime[thisPlayer] = new QTableWidgetItem(playerInfo[thisPlayer].getSurvivalTimeString());
            playerSurvivalTime[thisPlayer]->setTextAlignment(Qt::AlignCenter);
            playerSurvivalTime[thisPlayer]->setFont(tableFont);
            ui->scoreTable->setItem(thisRow, columnIndex++, playerSurvivalTime[thisPlayer]);

            //Populate the Zone Controlled Time
            playerZoneTime[thisPlayer] = new QTableWidgetItem(playerInfo[thisPlayer].getZoneTimeString());
            playerZoneTime[thisPlayer]->setTextAlignment(Qt::AlignCenter);
            playerZoneTime[thisPlayer]->setFont(tableFont);
            ui->scoreTable->setItem(thisRow, columnIndex++, playerZoneTime[thisPlayer]);

            //Populate hits and tags section of grid.
            for (int index = 1; index < 25; index++)
            {
                if (gameInfo.getIsThisPlayerInTheGame(index) == true)
                {
                    tagsByPlayer[thisPlayer][index] = new QTableWidgetItem(playerInfo[thisPlayer].getHitsAndTags(thisPlayer, index));
                    tagsByPlayer[thisPlayer][index]->setTextAlignment(Qt::AlignCenter);
                    tagsByPlayer[thisPlayer][index]->setFont(tableFont);
                    ui->scoreTable->setItem(thisRow, columnIndex++, tagsByPlayer[thisPlayer][index]);
                }
            }
        }
        else offsetForNonPlayers++;
    }
}
