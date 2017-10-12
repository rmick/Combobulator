#include "ScoresWindow.h"
#include "ui_ScoresWindow.h"

#include <QLayout>
#include <QDebug>

ScoresWindow::ScoresWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScoresWindow)
{
    ui->setupUi(this);

    //TODO: Set font and table size to match screen resolution.
    tableFont.setPointSize(16);
    headerFont.setPointSize(6);
    rowHeight = 30;
    tableFont.setFamily("Verdana");

    addColumnLabels();
    addPlayers();
    populateScores();
}

ScoresWindow::~ScoresWindow()
{
    delete ui;
}

void ScoresWindow::on_btn_Close_clicked()
{
    delete(this);
}

void ScoresWindow::addColumnLabels()
{
    ui->scoreTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //ui->scoreTable->horizontalHeader()->setFont(headerFont); //TODO: not working ! - but useful to undersize and force smaller margins

    int columnIndex = 0;
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, new QTableWidgetItem("Players\nName"));
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, new QTableWidgetItem("Hits\nTaken"));
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, new QTableWidgetItem("Hits\nLanded"));
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, new QTableWidgetItem("Survival\nTime"));
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, new QTableWidgetItem("Zone\nTime"));
    for (int index = 1; index < 25; index++)
    {
        ui->scoreTable->setHorizontalHeaderItem(columnIndex++, new QTableWidgetItem(playerInfo[index].getPlayerName()));
    }
}

void ScoresWindow::addPlayers()
{
    for (int index = 0; index < 25; index++)
    {
        ui->scoreTable->setRowHeight(index, rowHeight);

        if (index == 0)
        //adds a blankline at bottom for scrollbar
        {
             playerTaggerName[index] = new QTableWidgetItem("");
             ui->scoreTable->setItem(25, 0, playerTaggerName[index]);
        }
        else
        {
            playerTaggerName[index] = new QTableWidgetItem(playerInfo[index].getPlayerName());
            playerTaggerName[index]->setFont(tableFont);
            ui->scoreTable->setItem(index-1, 0, playerTaggerName[index]);
            playerTaggerName[index]->setFont(tableFont);
        }
    }
}

void ScoresWindow::populateScores()
{
    int columnIndex;
    for(int thisPlayer = 1; thisPlayer < 25; thisPlayer++)
    {
        columnIndex = 1;
        //Populate the Total Tags for each player
        int totalTagsTaken = playerInfo[thisPlayer].getTagsTaken(0); // Index 0 is total tags taken (and should equal the sum of all the other indexes)
        playerTotalTagsTaken[thisPlayer] = new QTableWidgetItem(QString::number(0-totalTagsTaken));
        playerTotalTagsTaken[thisPlayer]->setTextAlignment(Qt::AlignCenter);
        playerTotalTagsTaken[thisPlayer]->setFont(tableFont);
        ui->scoreTable->setItem(thisPlayer-1, columnIndex++, playerTotalTagsTaken[thisPlayer]);

        //Populate the Total Hits Landed on currentPlayer by every other player
        playerTotalHitsLanded[thisPlayer] = new QTableWidgetItem(playerInfo[thisPlayer].getTotalTagsLanded(thisPlayer));
        playerTotalHitsLanded[thisPlayer]->setTextAlignment(Qt::AlignCenter);
        playerTotalHitsLanded[thisPlayer]->setFont(tableFont);
        ui->scoreTable->setItem(thisPlayer-1, columnIndex++, playerTotalHitsLanded[thisPlayer]);

        //Populate the Survival Time
        playerSurvivalTime[thisPlayer] = new QTableWidgetItem(playerInfo[thisPlayer].getSurvivalTimeString());
        playerSurvivalTime[thisPlayer]->setTextAlignment(Qt::AlignCenter);
        playerSurvivalTime[thisPlayer]->setFont(tableFont);
        ui->scoreTable->setItem(thisPlayer-1, columnIndex++, playerSurvivalTime[thisPlayer]);

        //Populate the Zone Controlled Time
        playerZoneTime[thisPlayer] = new QTableWidgetItem(playerInfo[thisPlayer].getZoneTimeString());
        playerZoneTime[thisPlayer]->setTextAlignment(Qt::AlignCenter);
        playerZoneTime[thisPlayer]->setFont(tableFont);
        ui->scoreTable->setItem(thisPlayer-1, columnIndex++, playerZoneTime[thisPlayer]);

        //Populate hits and tags section of grid.
        for (int index = 1; index < 25; index++)
        {
            tagsByPlayer[thisPlayer][index] = new QTableWidgetItem(playerInfo[thisPlayer].getHitsAndTags(thisPlayer, index));
            tagsByPlayer[thisPlayer][index]->setTextAlignment(Qt::AlignCenter);
            tagsByPlayer[thisPlayer][index]->setFont(tableFont);
            ui->scoreTable->setItem(thisPlayer-1, columnIndex++, tagsByPlayer[thisPlayer][index]);
        }
        //columnIndex++;


    }
}
