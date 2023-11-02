#include "ScoresWindow.h"
#include "ui_ScoresWindow.h"

#include <QLayout>
#include <QDebug>
#include <QScreen>
#include "LttoComms.h"

ScoresWindow::ScoresWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScoresWindow)
{
    ui->setupUi(this);

    ui->scoreTable->setSortingEnabled(false);
    ui->btn_ViewMode->setText("Player Grid View");
	ui->btn_NameNumberMode->setVisible(false);

    displayTeamRanks();
    displayMode = SUMMARY_VIEW;
    calibrateScreen(SUMMARY_VIEW);
    addColumnLabels(SUMMARY_VIEW);
    addPlayerRows();
    populateScores(SUMMARY_VIEW);
    setOrder(SUMMARY_VIEW);

	deBrief	= DeBrief::getInstance();
	lttoComms = LttoComms::getInstance();

	timerSendRankReports = new QTimer(this);
	timerSendRankReports->start(1500);
	connect(timerSendRankReports,          SIGNAL(timeout() ),	this, SLOT(sendRankReports() )	);
	ui->btn_Close->setEnabled(false);
}

void ScoresWindow::sendRankReports()
{
	timerSendRankReports->stop();

	lttoComms->sendLCDtext("Sending"            , 1, false);
	lttoComms->sendLCDtext("Rank"				, 2, false);
	lttoComms->sendLCDtext("Reports"			, 3,  true);

	deBrief->sendRankReport();

	lttoComms->sendLCDtext(""					, 1, false);
	lttoComms->sendLCDtext("Game"               , 2, false);
	lttoComms->sendLCDtext("Over"				, 3,  true);

	ui->btn_Close->setEnabled(true);
}

ScoresWindow::~ScoresWindow()
{
    delete ui;
}

void ScoresWindow::on_btn_Close_clicked()
{
	emit closingScoresWindow();
	deleteLater();
}


void ScoresWindow::calibrateScreen(int modus)
{
    //2560 X 1596   QSize(2520, 1512)
    //2048 X 1276   QSize(2008, 1192)
    //1650 x 1046   QSize(1610, 962)
    //1440 X  896   QSize(1400, 812)
    //1280 X  796   QSize(1240, 712)
    //1152 X  716   QSize(1112, 632)
    //1024 x  764   QSize( 984, 680)
    // 800 x  596   QSize( 760, 512)

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    screenWidth   = screenGeometry.width()  - 40;           // allows for the horizontal border
    screenHeight  = screenGeometry.height() - 84;           // allows for the vertical border
    if(gameInfo.getNumberOfTeams() > 0) screenHeight -=50;  // allows for the Ranking Info frame
    qDebug() << "\nScoresWindow::calibrateScreen W x H:\n\t ScreenqDebug Size =" << screenGeometry.width() << "x" << screenGeometry.height()
             << "\n\t Table area  =" << screenWidth << "x" << screenHeight;
    switch (modus)
    {

    case ALL_IN_ONE_VIEW:
        //TODO: these are untested!
        columnWidth = screenWidth  / (gameInfo.getNumberOfPlayersInGame() + COLUMNS_IN_SUMMARY);
        rowHeight   = screenHeight / (gameInfo.getNumberOfPlayersInGame() + 1); // + 1 allows for title row
        break;
    case SUMMARY_VIEW:
        columnWidth = (screenWidth  / (COLUMNS_IN_SUMMARY) ) + 3;
        rowHeight   = screenHeight / (gameInfo.getNumberOfPlayersInGame() + 0); // + 1 allows for title row
        break;
    case PLAYER_GRID_VIEW:
        columnWidth = screenWidth  / (gameInfo.getNumberOfPlayersInGame() + 5); // +2 allows for ID and PlayerName
        rowHeight   = screenHeight / (gameInfo.getNumberOfPlayersInGame() + 0); // +1 allows for title row
        break;
    }

    tableFont.setPointSize(gameInfo.getScoreTableFontSize());
    headerFont.setPointSize(gameInfo.getScoreHeaderFontSize());
    //headerFont.setPointSize(tableFont.pointSize());

    //TODO: Set font size more accurately

//    qDebug() << "ScoresWindow::calibrateScreen \n\t Column Width =" << columnWidth
//             << "\n\t Row Height =" << rowHeight;
//    if(columnWidth < 25)                        tableFont.setPointSize(8);
//	if(columnWidth > 24 && columnWidth < 50)    tableFont.setPointSize(12);
//	if(columnWidth > 49 && columnWidth < 75)    tableFont.setPointSize(16);
//	if(columnWidth > 74 && columnWidth < 100)   tableFont.setPointSize(20);
//    if(columnWidth > 100)                       tableFont.setPointSize(17);

//    qDebug() << "\t Font Point Size =" << tableFont.pointSize();

    //tableFont.setPointSize(gameInfo.getScoreTableFontSize());
    //headerFont.setPointSize(gameInfo.getScoreHeaderFontSize());

    ui->scoreTable->setRowCount(gameInfo.getNumberOfPlayersInGame());
	tableFont.setFamily("Verdana");
    headerFont.setFamily("Verdana");
    headerFont.setBold(true);
}

void ScoresWindow::addColumnLabels(int modus)
{
    ui->scoreTable->clearContents();
    ui->scoreTable->setColumnCount(2);
    int columnIndex = 0;

    QTableWidgetItem *playerID = new QTableWidgetItem("ID");
	playerID->setFont(headerFont);
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, playerID);
    switch (modus)
    {
    case PLAYER_GRID_VIEW:
    //case ALL_IN_ONE_VIEW:
        ui->scoreTable->setColumnWidth(columnIndex-1, columnWidth * 1.25);
        break;
    case SUMMARY_VIEW:
        ui->scoreTable->setColumnWidth(columnIndex-1, columnWidth / 3.0);
        break;
    }
    QTableWidgetItem *PlayersName = new QTableWidgetItem("Players\nName");
	PlayersName->setFont(headerFont);
    ui->scoreTable->setHorizontalHeaderItem(columnIndex++, PlayersName);
	int sizeAdjust = screenWidth - ((gameInfo.getNumberOfPlayersInGame()+2)*columnWidth);  // adds any spare pixels into the PlayerName column

    switch (modus)
    {
    case PLAYER_GRID_VIEW:
    //case ALL_IN_ONE_VIEW:
        ui->scoreTable->setColumnWidth(columnIndex-1, (columnWidth * 4.0)); // + sizeAdjust);
        break;
    case SUMMARY_VIEW:
        ui->scoreTable->setColumnWidth(columnIndex-1,  columnWidth * 1.08);
        break;
    }

    switch (modus)
    {
    case SUMMARY_VIEW:
    //case ALL_IN_ONE_VIEW:
        {
        if      (modus == SUMMARY_VIEW) ui->scoreTable->setColumnCount(COLUMNS_IN_SUMMARY);
        //else if (modus == ALL_IN_ONE_VIEW) ui->scoreTable->setColumnCount(COLUMNS_IN_SUMMARY + gameInfo.getNumberOfPlayersInGame());

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
        break;
        }
    case PLAYER_GRID_VIEW:
        {
            if      (modus == PLAYER_GRID_VIEW) ui->scoreTable->setColumnCount(gameInfo.getNumberOfPlayersInGame() + 2);
            //else if (modus == ALL_IN_ONE_VIEW)  ui->scoreTable->setColumnCount(COLUMNS_IN_SUMMARY + gameInfo.getNumberOfPlayersInGame());
            int offsetForNonPlayers = 0;
            Q_UNUSED(offsetForNonPlayers);

            for (int index = 1; index < 25; index++)
            {
                if(gameInfo.getIsThisPlayerInTheGame(index) == true)
                {
                    if(playerNameDisplayInsteadOfNumber)
                    {
                        ui->scoreTable->setHorizontalHeaderItem(columnIndex++, new QTableWidgetItem(playerInfo[index].getExtendedPlayerName()));
                    }
                    else
                    {
                        if(gameInfo.getNumberOfTeams() == 0)
                            ui->scoreTable->setHorizontalHeaderItem(columnIndex++, new QTableWidgetItem(QString::number(index)));
                        else
                        {
                            if		(index <= 8)
                            {
                                QString	playerText = "1." +  QString::number(index);
                                ui->scoreTable->setHorizontalHeaderItem(columnIndex++, new QTableWidgetItem(playerText));
                            }
                            else if (index <=16)
                            {
                                QString	playerText = "2." +  QString::number(index-8);
                                ui->scoreTable->setHorizontalHeaderItem(columnIndex++, new QTableWidgetItem(playerText));
                            }
                            else
                            {
                                QString	playerText = "3." +  QString::number(index-16);
                                ui->scoreTable->setHorizontalHeaderItem(columnIndex++, new QTableWidgetItem(playerText));
                            }
                        }
                    }
                    ui->scoreTable->setColumnWidth(columnIndex-1, columnWidth);
                }
                else offsetForNonPlayers++;
            }
        break;
        }
    }

    ui->scoreTable->horizontalHeader()->setFont(headerFont);
    ui->scoreTable->horizontalHeader()->setStyleSheet("QHeaderView { font-size: " + QString::number(headerFont.pointSize()) + "pt; }");

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
			//playerNumber[index]->setData(Qt::EditRole, index);
            playerNumber[index]->setTextAlignment(Qt::AlignCenter);
            playerNumber[index]->setFont(tableFont);

			if(gameInfo.getNumberOfTeams() == 0)
				playerNumber[index]->setData(Qt::EditRole, index);
			else
			{
				if		(index <= 8) playerNumber[index]->setData(Qt::EditRole, 1 + (index/10.0));

				else if (index <=16) playerNumber[index]->setData(Qt::EditRole, 2 + (index-8)/10.0);

				else				 playerNumber[index]->setData(Qt::EditRole, 3 + (index-16)/10.0);
			}
			ui->scoreTable->setItem(thisRow, 0, playerNumber[index]);

            //add player name
            playerTaggerName[index] = new QTableWidgetItem(playerInfo[index].getExtendedPlayerName());
            playerTaggerName[index]->setTextAlignment(Qt::AlignCenter);
            //tableFont.setPointSize( (tableFont.pointSize()+10) );
            playerTaggerName[index]->setFont(tableFont);
            //tableFont.setPointSize( (tableFont.pointSize()-10) );
            ui->scoreTable->setItem(thisRow, 1, playerTaggerName[index]);
        }
        else offsetForNonPlayers++;
    }
}

void ScoresWindow::populateScores(int modus)
{
	qDebug() << "ScoresWindow::populateScores(1=Summary, 2 = PlayerGrid :" << modus;
	int columnIndex;
    int offsetForNonPlayers = 0;
	for(int thisPlayer = 1; thisPlayer <= MAX_PLAYERS; thisPlayer++)
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

				//Populate the Scores for each player
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
				for (int index = 1; index <= MAX_PLAYERS; index++)
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
    //ui->scoreTable->resizeColumnsToContents();
}

void ScoresWindow::setOrder(int modus)
{
    qDebug() << "ScoresWindow::setOrder() - TableSize = " << ui->scoreTable->size();

    ui->scoreTable->horizontalHeader()->setSortIndicatorShown(true);
    if (modus == PLAYER_GRID_VIEW)  ui->scoreTable->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
    else                            ui->scoreTable->horizontalHeader()->setSortIndicator(2, Qt::AscendingOrder);
    ui->scoreTable->setSortingEnabled(true);
}

void ScoresWindow::on_btn_ViewMode_clicked(bool refreshOnly)
{
	if(refreshOnly)
    {
        qDebug() << "ScoresWindow::on_btn_ViewMode_clicked() - Refresh Only";
        if		(displayMode == SUMMARY_VIEW)		displayMode = PLAYER_GRID_VIEW;
		else if	(displayMode == PLAYER_GRID_VIEW)	displayMode = SUMMARY_VIEW;
	}
    else
    {
        qDebug() << "ScoresWindow::on_btn_ViewMode_clicked() - Changing Font Size";
        //if		(displayMode == SUMMARY_VIEW)       tableFont.setPointSize( (tableFont.pointSize()+10) );
        //else if	(displayMode == PLAYER_GRID_VIEW)   tableFont.setPointSize( (tableFont.pointSize()-10) );
    }

	if		(displayMode == PLAYER_GRID_VIEW)
    {
        ui->scoreTable->setSortingEnabled(false);
        calibrateScreen(SUMMARY_VIEW);
        addColumnLabels(SUMMARY_VIEW);
        addPlayerRows();
        populateScores(SUMMARY_VIEW);
        setOrder(SUMMARY_VIEW);
		displayMode = SUMMARY_VIEW;
        ui->btn_ViewMode->setText("Player Grid View");
        ui->btn_NameNumberMode->setVisible(false);
    }
	else if (displayMode == SUMMARY_VIEW)
    {
        ui->scoreTable->setSortingEnabled(false);
        calibrateScreen(PLAYER_GRID_VIEW);
        addColumnLabels(PLAYER_GRID_VIEW);
        addPlayerRows();
        populateScores(PLAYER_GRID_VIEW);
        setOrder(PLAYER_GRID_VIEW);
		displayMode = PLAYER_GRID_VIEW;
        ui->btn_ViewMode->setText("Summary View");
        ui->btn_NameNumberMode->setVisible(true);
    }
}

void ScoresWindow::on_btn_NameNumberMode_clicked()
{
	//playerNameDisplayInsteadOfNumber = !playerNameDisplayInsteadOfNumber;
	if(ui->btn_NameNumberMode->isChecked()) playerNameDisplayInsteadOfNumber = true;
	else playerNameDisplayInsteadOfNumber = false;
	qDebug() << "ScoresWindow::on_btn_NameNumberMode_clicked()" << playerNameDisplayInsteadOfNumber;
	//redraw the display without changing the mode :-)
	on_btn_ViewMode_clicked(true);
}

void ScoresWindow::on_btn_EnlargeFont_clicked()
{
    tableFont.setPointSize(tableFont.pointSize()+2);
    headerFont.setPointSize(headerFont.pointSize()+2);
	gameInfo.setScoreTableFontSize(tableFont.pointSize());
	gameInfo.setScoreHeaderFontSize(headerFont.pointSize());
	on_btn_ViewMode_clicked(true);
    qDebug() << "ScoresWindow::on_btn_EnlargeFont_clicked()";
    qDebug() << gameInfo.getScoreTableFontSize();
    qDebug() << gameInfo.getScoreHeaderFontSize();
}

void ScoresWindow::on_btn_ReduceFont_clicked()
{
    tableFont.setPointSize(tableFont.pointSize()-2);
    headerFont.setPointSize(headerFont.pointSize()-2);
	gameInfo.setScoreTableFontSize(tableFont.pointSize());
    gameInfo.setScoreHeaderFontSize(headerFont.pointSize());
    on_btn_ViewMode_clicked(true);
    qDebug() << "ScoresWindow::on_btn_ReduceFont_clicked()";
    qDebug() << gameInfo.getScoreTableFontSize();
    qDebug() << gameInfo.getScoreHeaderFontSize();
}

void ScoresWindow::displayTeamRanks()
{
    switch(gameInfo.getNumberOfTeams())
    {
    case 0:
        ui->frame->hide();
        break;

    case 2:
        ui->frame    ->show();
        ui->lbl_Rank3->hide();
        break;

    case 3:
        ui->frame    ->show();
        ui->lbl_Rank3->show();
        break;
    }

    if      (gameInfo.getTeam1rank() == 1)    ui->lbl_Rank1->setText("1st: Team 1");
    else if (gameInfo.getTeam1rank() == 2)    ui->lbl_Rank2->setText("2nd: Team 1");
    else if (gameInfo.getTeam1rank() == 3)    ui->lbl_Rank3->setText("3rd: Team 1");

    if      (gameInfo.getTeam2rank() == 1)    ui->lbl_Rank1->setText("1st: Team 2");
    else if (gameInfo.getTeam2rank() == 2)    ui->lbl_Rank2->setText("2nd: Team 2");
    else if (gameInfo.getTeam2rank() == 3)    ui->lbl_Rank3->setText("3rd: Team 2");

    if      (gameInfo.getTeam3rank() == 1)    ui->lbl_Rank1->setText("1st: Team 3");
    else if (gameInfo.getTeam3rank() == 2)    ui->lbl_Rank2->setText("2nd: Team 3");
    else if (gameInfo.getTeam3rank() == 3)    ui->lbl_Rank3->setText("3rd: Team 3");
}
