//#include "RearrangePlayers.h"
//#include "ui_RearrangePlayers.h"

//#include <QDebug>

//RearrangePlayers::RearrangePlayers(QWidget *parent) :
//    QDialog(parent),
//    ui(new Ui::RearrangePlayers)
//{
//    ui->setupUi(this);
//    //signalMapper = new QSignalMapper(this);
//    signalMapperClicked  = new QSignalMapper(this);
//    signalMapperPressed  = new QSignalMapper(this);
//    signalMapperReleased = new QSignalMapper(this);

//    mapPlayerButtons();
//    firstPass = true;

//    populateListWidgetWithPlayers();
//}

//RearrangePlayers::~RearrangePlayers()
//{
//    delete ui;
//}

//void RearrangePlayers::mouseMoveEvent(QMouseEvent *event)
//{
//    if(event->buttons() & Qt::LeftButton)
//    {
//        if (activeButton != 0)
//        {
//                   //   Store original position.
//            activeButton->move(event->pos() - offset);
//            //activeButton->move(activeButton->parentWidget()->mapFromGlobal((QCursor::pos() - offset)));
//            //if(activeButton->pos()) qRound(110)
//        }
//    }
//}

//void RearrangePlayers::mousePressEvent(QMouseEvent *event)
//{

//}

//void RearrangePlayers::mouseReleaseEvent(QMouseEvent *event)
//{
//    if(event->buttons() & Qt::LeftButton)
//    {
//        firstPass = true;
//        qDebug() << "RearrangePlayers::mouseReleaseEvent";
//                 // Check whether in Bench area or player zone
//                 // If in playerzone, is the slot empty
//                 // else, return to original position.
//                 // If in BenchZone, place in next empty slot.
//                 //
//                 // Transfer player data to/from bench/player
//    }
//}

//void RearrangePlayers::playerButtonPressed(int playerNumber)
//{
//    activeButton = playerButtons[playerNumber];
//    offset = this->pos();
//    qDebug() << "RearrangePlayers::playerButtonPressed" << playerNumber <<  offset;
//}

//void RearrangePlayers::playerButtonReleased(int playerNumber)
//{
//    activeButton = 0;
//    qDebug() << "RearrangePlayers::playerButtonReleased" << playerNumber;
//}
//void RearrangePlayers::on_btn_Close_clicked()
//{
//    close();
//}

//void RearrangePlayers::mapPlayerButtons()
//{
//    playerButtons.append(0);    // Fake button for Player 0
//    playerButtons.append(ui->btn_Player1);
//    playerButtons.append(ui->btn_Player2);
//    playerButtons.append(ui->btn_Player3);
//    playerButtons.append(ui->btn_Player4);
//    playerButtons.append(ui->btn_Player5);
//    playerButtons.append(ui->btn_Player6);
//    playerButtons.append(ui->btn_Player7);
//    playerButtons.append(ui->btn_Player8);
//    playerButtons.append(ui->btn_Player9);
//    playerButtons.append(ui->btn_Player10);
//    playerButtons.append(ui->btn_Player11);
//    playerButtons.append(ui->btn_Player12);
//    playerButtons.append(ui->btn_Player13);
//    playerButtons.append(ui->btn_Player14);
//    playerButtons.append(ui->btn_Player15);
//    playerButtons.append(ui->btn_Player16);
//    playerButtons.append(ui->btn_Player17);
//    playerButtons.append(ui->btn_Player18);
//    playerButtons.append(ui->btn_Player19);
//    playerButtons.append(ui->btn_Player20);
//    playerButtons.append(ui->btn_Player21);
//    playerButtons.append(ui->btn_Player22);
//    playerButtons.append(ui->btn_Player23);
//    playerButtons.append(ui->btn_Player24);

//    //Connect button array to SignalMapper
//    for (int index = 1; index < 25; index++)
//    {
//        connect (playerButtons[index],  SIGNAL(pressed()),  signalMapperPressed,  SLOT (map()) );
//        connect (playerButtons[index],  SIGNAL(released()), signalMapperReleased, SLOT (map()) );
//        signalMapperPressed ->setMapping(playerButtons[index] ,  index);
//        signalMapperReleased->setMapping(playerButtons[index] ,  index);
//    }
//    connect (signalMapperPressed,  SIGNAL(mapped(int)), this, SLOT(playerButtonPressed(int))   );
//    //connect (signalMapperReleased, SIGNAL(mapped(int)), this, SLOT(playerButtonReleased(int))   );
//}

//void RearrangePlayers::populateListWidgetWithPlayers()
//{

//    for(int index = 1;index < MAX_PLAYERS/3; index++)
//    {
//         ui->list_Team1->addItem(playerInfo[index].getPlayerName());
//         ui->list_Team2->addItem(playerInfo[index+8].getPlayerName());
//         ui->list_Team3->addItem(playerInfo[index+16].getPlayerName());
//    }





//    return;

//    //ui->tableWidget_Game->setColumnCount(2);

//    Title[1] = new QTableWidgetItem("Team 1");
//    ui->tableWidget_Game->setHorizontalHeaderItem(0,Title[1]);

//    Title[2] = new QTableWidgetItem("Team 2");
//    ui->tableWidget_Game->setHorizontalHeaderItem(1,Title[2]);

//    Title[3] = new QTableWidgetItem("Team 3");
//    ui->tableWidget_Game->setHorizontalHeaderItem(2,Title[3]);



//    for (int index = 1; index <= MAX_PLAYERS; index++)
//    {
//        //add player names
//        playerTaggerName[index] = new QTableWidgetItem(playerInfo[index].getPlayerName());
//        for (int columnIndex = 0; columnIndex < 4; columnIndex++)
//        {
//            ui->tableWidget_Game->setItem(index-1, columnIndex, playerTaggerName[index]);
//        }
//    }
//}
