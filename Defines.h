#ifndef DEFINES_H
#define DEFINES_H

//macdeployqt terminal command (copy into terminal window to build a .dmg)
//          Source - https://dragly.org/2012/01/13/deploy-qt-applications-for-mac-os-x/
//      Navigate to the QT Build directory for the App. (e.g. build-LTTO_Host-Desktop..........)
//      Right click on the folder and go to 'services', then 'New Terminal At Folder'.
//      Copy/paste the following line into the terminal window and wait.....
//              /Users/Richie/Qt/5.9.1/clang_64/bin/macdeployqt LTTO_Host.app -dmg
//
//      Here is the current build directory
//              build-Combobulator_Mac-Release

//windeployqt
//      Copy the Combobulator.exe
//          from the        Qt_projects\build-Combobulator_Windows-Release\Combobulator\release
//          to the          Qt_projects\build-Combobulator_Windows-Release\Combobulator\Combobulator folder
//
//      Open CMD.exe prompt
//      cd\
//      W:
// STOP !!!!! (id you do step 1 above - copy the file to a new location first !!!!
//      cd Qt_projects\build-Combobulator_Windows-Release\Combobulator
//      C:\Qt\5.9.1\msvc2015_64\bin\windeployqt Combobulator.exe
//      Zip up the folder and publish to Dropbox (or whereever)

// If enabling this #define, also remove the # from the command 'serialport' in line 9 of the LTTO_host.pro file
#define INCLUDE_SERIAL_USB


//  Build Number
const QString BUILD_NUMBER  = "Build : 17.10.02a";

// Recoil Game Hub_28f537d14fca




//  WiFi stuff
const QString   HOST_IP_ADDRESS     = "192.168.4.1";
const QString   WIFI_PASSWORD       = "Lasertag42";
const int       TCP_IP_PORT         = 8000;

//LTTO library packet type indentifiers
const char  PACKET      = 'P';
const char  DATA        = 'D';
const char  CHECKSUM    = 'C';
const char  TAG         = 'T';
const char  BEACON      = 'B';

//Internal timers (in mSec)
const int   INTERPACKET_DELAY_MSEC  = 50;
const int   HOST_TIMER_MSEC         = 1500;
const int   PRESS_AND_HOLD_TIME     = 500;
const int   BEACON_TIMER_MSEC       = 500;

//Packet Header Type Codes
const int   ANNOUNCE    = 0x00;

const int   COUNTDOWN               = 0x00;
const int   ASSIGN_PLAYER           = 0X01;
const int   ANNOUNCE_CUSTOM_LTAG    = 0x02;
const int   ANNOUCE_SPECIAL_GAME    = 0x0C;

const int   ASSIGN_PLAYER_FAIL      = 0x0F;
const int   REQUEST_JOIN_GAME       = 0x10;
const int   ACK_PLAYER_ASSIGN       = 0x11;
const int   REQUEST_ASSISTANCE      = 0x20;
const int   REQUEST_TAG_REPORT      = 0x31;
const int   TAG_SUMMARY             = 0x40;
const int   TEAM_1_TAG_REPORT       = 0x41;
const int   TEAM_2_TAG_REPORT       = 0x42;
const int   TEAM_3_TAG_REPORT       = 0x43;

const int   ANNOUNCE_LTAR_GAME      = 0x81;
const int   REQUEST_JOIN_LTAR_GAME  = 0x82;
const int   ASSIGN_LTAR_PLAYER      = 0x83;
const int   ACK_LTAR_PLAYER_ASSIGN  = 0x84;
const int   LTAR_NAME               = 0x85;     //smart device app generated
const int   LTAR_WHODAT             = 0x86;     //smart device app generated
const int   ASSIGN_LTAR_PLAYER_OK   = 0x87;
const int   COUNTDOWN_LTAR          = 0x88;
const int   ASSIGN_LTAR_PLAYER_FAIL = 0x8F;

const int   TEXT_MESSAGE            = 0x80;
const int   SPECIAL_ATTACK          = 0x90;

const bool  DECIMAL                 = false;
const bool  BCD                     = true;

//Default Game Tagger settings
const int   DEFAULT_GAME_TYPE       = 0x02;
const int   DEFAULT_GAME_ID         = 0x64;
const int   DEFAULT_GAME_LENGTH     = 10;
const int   DEFAULT_COUNTDOWN_TIME  = 5;
const int   DEFAULT_RELOADS         = 100;        // 100 = Unlimited
const int   DEFAULT_RELOADS2        = 0;         // 0xFFFF = LTAR unlimited
const int   DEFAULT_HEALTH          = 25;
const int   DEFAULT_SHEILDS         = 15;
const int   DEFAULT_MEGAS           = 20;         // 100 - Unlimited
const int   DEFAULT_FLAGS1          = 0x20;
const int   DEFAULT_FLAGS2          = 0x01;
const int   DEFAULT_FLAGS3          = 0x5C;
const int   DEFAULT_START_AMMO      = 10;         // FF - Unlimited
const int   DEFAULT_SLEEP_TIME_OUT  = 0;          // 0 - Disabled

//Packed byte, FLAGS1
const int   NEUTRALISE_10_FLAG      = 7;
const int   LIMITED_RELOADS_FLAG    = 6;
const int   LIMITED_MEGAS_FLAG      = 5;
const int   TEAM_TAGS_FLAG          = 4;
const int   MEDIC_MODE_FLAG         = 3;
const int   SLOW_TAG_FLAG           = 2;
const int   HUNT_THE_PREY_FLAG      = 1;
const int   REVERSE_HUNT_DIR_FLAG   = 0;

//Packed byte, FLAGS2
const int   CONTESTED_ZONES_FLAG            = 7;
const int   ZONES_TO_TEAMS_FLAG             = 6;
const int   NEUTRALISE_15s_TAGGED_FLAG      = 5;
const int   SUPPLY_ZONES_REVIVE_FLAG        = 4;
const int   SUPPLY_ZONES_REFILL_TAGS_FLAG   = 3;
const int   HOSTILE_ZONES_FLAG              = 2;
const int   TEAM_COUNT_MSB                  = 1;
const int   TEAM_COUNT_LSB                  = 0;

//Packed byte, FLAGS3
const int   DISABLE_LTAR_ENHANCED_BEACONS   = 7;
const int   DISABLE_IFF_BEACONS             = 6;
const int   DISABLE_TAG_RECVD_BEACON        = 5;
const int   SHIELD_BUTTON_HOLD_MODE         = 4;
const int   SUPPLY_ZONES_REFILL_AMMO        = 3;
const int   SUPPLY_ZONES_REFILL_SHIELDS     = 2;
const int   AUTO_RELOAD                     = 1;
const int   ALLOW_FULL_AUTO_FIRE            = 0;

//Debrief stuff
const int   REQUEST_TAG_SUMMARY_BIT         = 1;
const int   REQUEST_TEAM1_REPORT_BIT        = 2;
const int   REQUEST_TEAM2_REPORT_BIT        = 4;
const int   REQUEST_TEAM3_REPORT_BIT        = 8;

//States for PlayersWindow::SetPlayerControls
const int   CURRENT_MODE                    = 2;
const int   HANDICAP_MODE                   = 0;
const int   EDIT_SETTINGS_MODE              = 1;

//States for HostGameWindow
const int   HOST_MODE                       = 1;
const int   COUNTDOWN_MODE                  = 2;
const int   GAME_MODE                       = 3;
const int   REHOST_MODE                     = 4;
const int   DEBRIEF_MODE                    = 5;
const int   SCOREBOARD_MODE                 = 6;

//Classes for calledByWho
const int   LTTO_MAIN_WINDOW                = 2;
const int   PLAYERS_WINDOW                  = 3;

const QString   BUTTON_SELECTED             = "border-color: cyan; color: cyan;";       //"font: bold;";
const QString   BUTTON_UNSELECTED           = "color: black;";                          //"font: normal;";
const QString   BUTTON_CHECKED              = "border-color: cyan; color: cyan;";       //"background-color: rgb(50,220,200)";
const QString   BUTTON_UNCHECKED            = "border-color: grey; color: black;";      //"background-color: cyan";
const QString   BUTTON_LAST_PRESSED         = "color:red;";                             //"background-color: rgb(50,250,220)";


//        QToolBar
//        {
//            border: grey;
//            border-width: 2px;
//            background-color: rgb(50,50,50);
//            spacing: 20px;
//            padding: 10px;
//        }

//        /*---------------------------------------------------------------------------*/



//        QtoolButton:hover
//        {
//            color: rgb(250,200,200)
//        }

//        QToolButton:checked
//        {
//            font: red;
//        border-color: cyan
//        }

//        /*---------------------------------------------------------------------------*/

//        QPushButton
//        {
//            background-color: grey;
//            border-style: outset;
//            border-width: 2px;
//            border-radius: 10px;
//            border-color: grey;
//            min-width: 50;
//            min-height: 30;
//        }

//        QPushButton:disabled
//        {
//            /*background-color: rgb(200,230	,230); */
//            background-color: rgb(50, 50, 50);
//        }

//        QPushButton:checked
//        {
//            border-color: cyan;
//            /*background-color: rgb(50,220,200);*/
//            color: cyan;
//            /*font-weight: bold;*/
//         }

//        /*---------------------------------------------------------------------------*/

//        QSlider
//        {
//            min-height: 35px;
//            background: grey;
//            border-style: outset;
//            border-width: 1px;
//            border-radius: 10px;
//            border-color: black;
//        }

//        QSlider::groove:horizontal {
//            border-style: outset;
//            border-width: 2px;
//            border-radius: 10px;
//            border-color: black;
//        }

//        QSlider::handle:horizontal {
//            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);
//            border: 1px solid #5c5c5c;
//            width: 18px;
//            margin: -2px 0; /* handle is placed by default on the contents rect of the groove. Expand outside the groove*/
//            border-style: outset;
//            border-width: 2px;
//            border-radius: 5px;
//            border-color: cyan;
//        }"


#endif // DEFINES_H



//        QSpinBox
//        {
//        color: white;
//        }

//        /*---------------------------------------------------------------------------*/

//        QMessageBox
//        {
//            font:white;
//        }

//        /*---------------------------------------------------------------------------*/

//        QMenu
//        {
//            font: white;
//            color: white;
//        }

//        /*---------------------------------------------------------------------------*/

//        QWidget
//        {
//            background-color: black;
//            font: white;
//        }

//        /*---------------------------------------------------------------------------*/

//        QLabel
//        {
//            color: white;
//        }

//        /*---------------------------------------------------------------------------*/

//        QListWidget
//        {
//            color: white;
//        }

//        /*---------------------------------------------------------------------------*/

//        QToolBar
//        {
//            border: grey;
//            border-width: 2px;
//            background-color: rgb(50,50,50);
//            spacing: 20px;
//            padding: 10px;
//        }

//        /*---------------------------------------------------------------------------*/

//        QToolButton
//        {
//            border-width: 2px;
//                border-radius: 10px;
//                border-color: cyan;
//            background-color:grey;
//            padding: 5px;
//            spacing: 20px;
//            font: bold;
//        }

//        QtoolButton:hover
//        {
//            color: rgb(250,200,200)
//        }

//        QToolButton:checked
//        {
//            font: red;
//        border-color: cyan
//        }

//        /*---------------------------------------------------------------------------*/

//        QPushButton
//        {
//            background-color: grey;
//            border-style: outset;
//            border-width: 2px;
//            border-radius: 10px;
//            border-color: grey;
//            min-width: 50;
//            min-height: 30;
//        }

//        QPushButton:disabled
//        {
//            /*background-color: rgb(200,230	,230); */
//            background-color: rgb(50, 50, 50);
//        }

//        QPushButton:checked
//        {
//            border-color: cyan;
//            /*background-color: rgb(50,220,200);*/
//            color: cyan;
//            /*font-weight: bold;*/
//         }

//        /*---------------------------------------------------------------------------*/

//        QSlider
//        {
//            min-height: 35px;
//            background: grey;
//            border-style: outset;
//            border-width: 1px;
//            border-radius: 10px;
//            border-color: black;
//        }

//        QSlider::groove:horizontal {
//            border-style: outset;
//            border-width: 2px;
//            border-radius: 10px;
//            border-color: black;
//        }

//        QSlider::handle:horizontal {
//            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);
//            border: 1px solid #5c5c5c;
//            width: 18px;
//            margin: -2px 0; /* handle is placed by default on the contents rect of the groove. Expand outside the groove*/
//            border-style: outset;
//            border-width: 2px;
//            border-radius: 5px;
//            border-color: cyan;
//        }

