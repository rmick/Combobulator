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

    displayMode = SUMMARY_VIEW;
    ui->btn_ViewMode->setText("Player Grid View");

    calibrateScreen(SUMMARY_VIEW);
    addColumnLabels(SUMMARY_VIEW);
    addPlayerRows();
    populateScores(SUMMARY_VIEW);
    setOrder(SUMMARY_VIEW);
}

ScoresWindow::~ScoresWindow()
{
    delete ui;
}

void ScoresWindow::on_btn_Close_clicked()
{
    deleteLater();
}


void ScoresWindow::calibrateScreen(int modus)
{
    int cellWidth = 0;
    int cellHeight = 0;

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int screenWidth   = screenGeometry.width();
    int screenHeight  = screenGeometry.height() - 75;

    switch (modus)
    {
    case ALL_IN_ONE_VIEW:
        //TODO: these are untested!
        cellWidth   = screenWidth  / (gameInfo.getNumberOfPlayersInGame() + COLUMNS_IN_SUMMARY);
        cellHeight  = screenHeight / (gameInfo.getNumberOfPlayersInGame());
        break;
    case SUMMARY_VIEW:
        cellWidth   = screenWidth  / COLUMNS_IN_SUMMARY;
        cellHeight  = screenHeight / (gameInfo.getNumberOfPlayersInGame() + 1);
        break;
    case PLAYER_GRID_VIEW:
        //if ()
        cellWidth   = (screenWidth - (40 + gameInfo.getNumberOfPlayersInGame())) / (gameInfo.getNumberOfPlayersInGame() + 2);
        cellHeight  = screenHeight / (gameInfo.getNumberOfPlayersInGame() + 1);
        break;
    }

    qDebug() << "ScoresWindow::calibrateScreen() W x H" << cellWidth << cellHeight;

    //TODO: Set font and table size to match screen resolution.
    if(cellWidth < 25)                      tableFont.setPointSize(8);
    if(cellWidth > 24 && cellWidth < 50)    tableFont.setPointSize(12);
    if(cellWidth > 49 && cellWidth < 75)    tableFont.setPointSize(16);
    if(cellWidth > 74 && cellWidth < 100)   tableFont.setPointSize(20);
    if(cellWidth > 100)                     tableFont.setPointSize(24);

    //ui->scoreTable->setColumnCount(COLUMNS_IN_SUMMARY + gameInfo.getNumberOfPlayersInGame());
    ui->scoreTable->setRowCount(gameInfo.getNumberOfPlayersInGame());

    tableFont.setFamily("Verdana");

    if (modus == SUMMARY_VIEW)
    {
        rowHeight   = cellHeight;
        columnWidth = cellWidth - (COLUMNS_IN_SUMMARY + 1);
    }
    else if (modus == PLAYER_GRID_VIEW)
    {
        rowHeight   = cellHeight-1;
        columnWidth = cellWidth;
    }

    headerFont.setPointSize(12);
}

void ScoresWindow::addColumnLabels(int modus)
{
    ui->scoreTable->clearContents();
    ui->scoreTable->setColumnCount(2);
    int columnIndex = 0;

    QTableWidgetItem *playerID = new QTableWidgetItem("ID");
    playerID->setFont(headerFont);
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, playerID);
    ui->scoreTable->setColumnWidth(columnIndex-1, tableFont.pointSize()*2);  //      columnWidth/2);


    QTableWidgetItem *PlayersName = new QTableWidgetItem("Players\nName");
    PlayersName->setFont(headerFont);
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, PlayersName);
    if (modus != SUMMARY_VIEW)  ui->scoreTable->setColumnWidth(columnIndex-1, columnWidth + gameInfo.getNumberOfPlayersInGame());
    else                        ui->scoreTable->setColumnWidth(columnIndex-1, columnWidth);

    if (modus != PLAYER_GRID_VIEW)
    {
        if      (modus == SUMMARY_VIEW) ui->scoreTable->setColumnCount(COLUMNS_IN_SUMMARY);
        else if (modus == ALL_IN_ONE_VIEW) ui->scoreTable->setColumnCount(COLUMNS_IN_SUMMARY + gameInfo.getNumberOfPlayersInGame());

        QTableWidgetItem *Rank = new QTableWidgetItem("Rank");
        Rank->setFont(headerFont);
        ui->scoreTable->setHorizontalHeaderItem(columnIndex++, Rank);
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
    }

    if (modus != SUMMARY_VIEW)
    {
        if      (modus == PLAYER_GRID_VIEW) ui->scoreTable->setColumnCount(gameInfo.getNumberOfPlayersInGame() + 2);
        else if (modus == ALL_IN_ONE_VIEW)  ui->scoreTable->setColumnCount(COLUMNS_IN_SUMMARY + gameInfo.getNumberOfPlayersInGame());
        int offsetForNonPlayers = 0;
        for (int index = 1; index < 25; index++)
        {
            if(gameInfo.getIsThisPlayerInTheGame(index) == true)
            {
                ui->scoreTable->setHorizontalHeaderItem(columnIndex++, new QTableWidgetItem(playerInfo[index].getPlayerName()));
                ui->scoreTable->setColumnWidth(columnIndex-1, columnWidth);
            }
            else offsetForNonPlayers++;
        }
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

            //add playerID number
            playerNumber[index] = new QTableWidgetItem();
            playerNumber[index]->setData(Qt::EditRole, index);
            playerNumber[index]->setTextAlignment(Qt::AlignCenter);
            playerNumber[index]->setFont(tableFont);
            ui->scoreTable->setItem(thisRow, 0, playerNumber[index]);

            //add player name
            playerTaggerName[index] = new QTableWidgetItem(playerInfo[index].getPlayerName());
            playerTaggerName[index]->setTextAlignment(Qt::AlignCenter);
            playerTaggerName[index]->setFont(tableFont);
            ui->scoreTable->setItem(thisRow, 1, playerTaggerName[index]);
        }
        else offsetForNonPlayers++;
    }
}

void ScoresWindow::populateScores(int modus)
{
    int columnIndex;
    int offsetForNonPlayers = 0;
    for(int thisPlayer = 1; thisPlayer < 25; thisPlayer++)
    {
        if (gameInfo.getIsThisPlayerInTheGame(thisPlayer) == true)
        {
            columnIndex = 2;
            int thisRow = (thisPlayer-1) - offsetForNonPlayers;

            if (modus != PLAYER_GRID_VIEW)
            {
                //Populate the Rankings for each player
                int playerRank = playerInfo[thisPlayer].getRankingInGame();
                playerRanking[thisPlayer] = new QTableWidgetItem();
                playerRanking[thisPlayer]->setData(Qt::EditRole, playerRank);
                playerRanking[thisPlayer]->setTextAlignment(Qt::AlignCenter);
                playerRanking[thisPlayer]->setFont(tableFont);
                ui->scoreTable->setItem(thisRow, columnIndex++, playerRanking[thisPlayer]);

                //Populate the SCores for each player
                int playerScoring = playerInfo[thisPlayer].getGameScore();
                playerScore[thisPlayer] = new QTableWidgetItem();
                playerScore[thisPlayer]->setData(Qt::EditRole, playerScoring);
                playerScore[thisPlayer]->setTextAlignment(Qt::AlignCenter);
                playerScore[thisPlayer]->setFont(tableFont);
                ui->scoreTable->setItem(thisRow, columnIndex++, playerScore[thisPlayer]);

                //Populate the Total Tags for each player
                int totalTagsTaken = 0 - playerInfo[thisPlayer].getTagsTaken(0); // Index 0 is total tags taken (and should equal the sum of all the other indexes)
                playerTotalTagsTaken[thisPlayer] = new QTableWidgetItem();
                playerTotalTagsTaken[thisPlayer]->setData(Qt::EditRole, totalTagsTaken);
                playerTotalTagsTaken[thisPlayer]->setTextAlignment(Qt::AlignCenter);
                playerTotalTagsTaken[thisPlayer]->setFont(tableFont);
                ui->scoreTable->setItem(thisRow, columnIndex++, playerTotalTagsTaken[thisPlayer]);

                //Populate the Total Hits Landed on currentPlayer by every other player
                int totalTagsLanded = playerInfo[thisPlayer].getTotalTagsLanded(thisPlayer);
                playerTotalHitsLanded[thisPlayer] = new QTableWidgetItem();
                playerTotalHitsLanded[thisPlayer]->setData(Qt::EditRole, totalTagsLanded);
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
            }

            if (modus != SUMMARY_VIEW)
            {
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
        }
        else offsetForNonPlayers++;
    }
}

void ScoresWindow::setOrder(int modus)
{
    ui->scoreTable->horizontalHeader()->setSortIndicatorShown(true);
    if (modus == PLAYER_GRID_VIEW)  ui->scoreTable->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
    else                            ui->scoreTable->horizontalHeader()->setSortIndicator(2, Qt::AscendingOrder);
    ui->scoreTable->setSortingEnabled(true);
}

void ScoresWindow::on_btn_ViewMode_clicked()
{
    //Swap view from Summary to Player Grid
    if(displayMode == PLAYER_GRID_VIEW)
    {
        ui->scoreTable->setSortingEnabled(false);
        calibrateScreen(SUMMARY_VIEW);
        addColumnLabels(SUMMARY_VIEW);
        addPlayerRows();
        populateScores(SUMMARY_VIEW);
        setOrder(SUMMARY_VIEW);
        displayMode = SUMMARY_VIEW;
        ui->btn_ViewMode->setText("Player Grid View");
    }
    else
    {
        ui->scoreTable->setSortingEnabled(false);
        calibrateScreen(PLAYER_GRID_VIEW);
        addColumnLabels(PLAYER_GRID_VIEW);
        addPlayerRows();
        populateScores(PLAYER_GRID_VIEW);
        setOrder(PLAYER_GRID_VIEW);
        displayMode = PLAYER_GRID_VIEW;
        ui->btn_ViewMode->setText("Summary View");
    }
}

void ScoresWindow::on_btn_Debug_clicked()
{
    DeBrief debrifInst;
    debrifInst.calculateRankings();
}
