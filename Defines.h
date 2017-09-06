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
//              build-LTTO_Host-Desktop_Qt_5_9_1_clang_64bit-Release


// If enabling this #define, also remove the # from the command 'serialport' in line 9 of the LTTO_host.pro file
#define INCLUDE_SERIAL_USB


//  Build Number
const QString BUILD_NUMBER  = "Build : 17.9.5a";

//  WiFi stuff
const QString   HOST_IP_ADDRESS     = "192.168.4.1";
const QString   WIFI_PASSWORD       = "LTAG42";
const int       TCP_IP_PORT         = 8000;

//LTTO library packet type indentifiers
const char  PACKET      = 'P';
const char  DATA        = 'D';
const char  CHECKSUM    = 'C';
const char  TAG         = 'T';
const char  BEACON      = 'B';

//Internal timers (in mSec)
const int   INTERPACKET_DELAY_MSEC  = 50;
const int   HOST_TIMER_MSEC         = 1800;
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
const int   DEFAULT_HEALTH          = 25;
const int   DEFAULT_SHEILDS         = 15;
const int   DEFAULT_MEGAS           = 20;         // 100 - Unlimited
const int   DEFAULT_FLAGS1          = 0x20;
const int   DEFAULT_FLAGS2          = 0x01;
const int   DEFAULT_FLAGS3          = 0x5C;

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
const int   USE_STANDARD_BEACONS            = 7;
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

#endif // DEFINES_H
