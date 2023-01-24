#ifndef DEFINES_H
#define DEFINES_H

#include <QString>

/*
macdeployqt terminal command (copy into terminal window to build a .dmg)
          Source - https://dragly.org/2012/01/13/deploy-qt-applications-for-mac-os-x/

	  1)	Navigate to the QT Build directory for the App.
			   (e.g. Qt_projects\build-Combobulator_Mac-Release..........)

	  2)	Right click on the folder and go to 'services', then 'New Terminal At Folder'.

	  3)	Copy/paste the following line into the terminal window and wait....

              /Users/Richie/Qt/5.12.12/clang_64/bin/macdeployqt Combobulator.app -dmg

              IF YOU GET AN ERROR - check that you have not upgraded to a newer version of Qt (e.g. 5.13)

      Here is the current build directory	Qt_projects\build-Combobulator_Mac/release


windeployqt
	  1)	Copy the Combobulator.exe
               from the        Qt_projects\build-Combobulator_Windows\release\
               to the          Qt_projects\build-Combobulator_Windows\release\Combobulator

	  2)	Open CMD.exe prompt
			   cd\
               W:  or   C:

	  3)	STOP !!!!! (Did you do step 1 above - copy the file to a new location first !!!!

      4)	cd C:\Users\Mickan-Dropbox\Dropbox\Qt_projects\build-Combobulator_Windows64\release\Combobulator
                W:\Users\richie\Dropbox\Qt_projects\build-Combobulator_Windows64\release\Combobulator>

            C:\Qt\5.12.12\mingw73_64\bin\windeployqt Combobulator.exe
			C:\Qt\5.11.0\mingw53_32\bin\windeployqt Combobulator.exe

            Copy the missing DLL files from C:\Qt\5.11.0\mingw53_32\bin to Combob folder

	  5)	Zip up the folder and publish to Dropbox


androiddeployqt
	  Do nothing, it all automatic :-)
      To get the APK file - Go to [Projects] tab in Qt Creator, Select Android Build,
                            Expand the [build apk] section and select [show apk after build] option.
*/


/*
  Arduino Updates
  Go to Sketch menu and 'Export Binary File'.
  Open Finder and navigate to Dropbox/Arduino/CombobHost
  Look for the file named    ESP-CombobHost.ino.heltec_wifi_kit_32.bin
  Rename it to				 ESP-CombobHost.ino.bin
  Open Amazon S3	https://s3.console.aws.amazon.com/s3/buckets/combobulator/?region=us-east-2&tab=overview#
  Upload the file, replacing the existing file.
  Click on the [Options] button and 'Make Public'
  Done !
*/





// If enabling this #define, also remove the # from the command 'serialport' in line 9 of the LTTO_host.pro file
#define INCLUDE_SERIAL_USB


//  Build Number
const QString	BUILD_NUMBER					= "r2212.18";
const QString	VERSION_NUMBER					= "v1.17f (gertBuild v6)";

const double	CURRENT_BASESTATION_FIRMWARE	= 1.15;

//Maximum number of players
//--------------------------
const int   MAX_PLAYERS = 24;
//--------------------------

//  WiFi stuff
const QString   HOST_IP_ADDRESS     = "192.168.42.42";
const QString   WIFI_PASSWORD       = "Lasertag42";
const int       TCP_IP_PORT         = 8000;
const float		LOW_BATT_LEVEL		= 3.5;

//LTTO library packet type indentifiers
const char  PACKET      = 'P';
const char  DATA        = 'D';
const char  CHECKSUM    = 'C';
const char  TAG         = 'T';
const char  BEACON      = 'Z';
const char	LTAR_BEACON = 'E';

//Internal timers (in mSec)
const int   INTERPACKET_DELAY_MSEC		= 50;
const int   HOST_TIMER_MSEC				= 1500;     //1500 is the standard, 1200 is fast (but causes issues with some LTAR taggers)
const int   DEBRIEF_TIMER_MSEC			= 1500;     //1500 is the standard
const int   TEXT_SENT_DELAY				= 15;
const int   PRESS_AND_HOLD_TIME			= 500;
const int   BEACON_TIMER_MSEC			= 500;
const int   ASSIGNED_PLAYER_FAIL_TIMER	= 500;
const int	HEART_BEAT_MSEC				= 2500;
const int	ACK_NOT_RECEIVED_MSEC		= 1800;

//Packet Header Type Codes
const int   ANNOUNCE				= 0x00;

const int   COUNTDOWN               = 0x00;
const int   ASSIGN_PLAYER           = 0x01;
const int   ANNOUNCE_CUSTOM_LTAG    = 0x02;		//P02
const int   ANNOUNCE_SPECIAL_GAME   = 0x0C;		//P12

const int   ASSIGN_PLAYER_FAIL      = 0x0F;		//P15
const int   REQUEST_JOIN_GAME       = 0x10;		//P16
const int   ACK_PLAYER_ASSIGN       = 0x11;		//P17
const int   REQUEST_ASSISTANCE      = 0x20;		//P32
const int   REQUEST_TAG_REPORT      = 0x31;		//P49
const int   SEND_RANK_REPORT        = 0x32;		//P50
const int   TAG_SUMMARY             = 0x40;		//P64
const int   TEAM_1_TAG_REPORT       = 0x41;		//P65
const int   TEAM_2_TAG_REPORT       = 0x42;		//P66
const int   TEAM_3_TAG_REPORT       = 0x43;		//P67

const int   ANNOUNCE_LTAR_GAME      = 0x81;		//P129
const int   REQUEST_JOIN_LTAR_GAME  = 0x82;		//P130
const int   ASSIGN_LTAR_PLAYER      = 0x83;		//P131
const int   ACK_LTAR_PLAYER_ASSIGN  = 0x84;		//P132
const int   LTAR_NAME               = 0x85;     //P133		//smart device app generated
const int   LTAR_WHODAT             = 0x86;     //P134		//smart device app generated
const int   ASSIGN_LTAR_PLAYER_OK   = 0x87;		//P135
const int   COUNTDOWN_LTAR          = 0x88;		//P136
const int   ASSIGN_LTAR_PLAYER_FAIL = 0x8F;		//P143

const int   TEXT_MESSAGE            = 0x80;		//P128
const int   SPECIAL_ATTACK          = 0x90;		//P144

const bool  DECIMAL                 = false;
const bool  BCD                     = true;

//Default Game Tagger settings
const int   DEFAULT_GAME_TYPE       = 0x02;
const int   DEFAULT_GAME_ID         = 0x42;
const int   DEFAULT_GAME_LENGTH     = 10;
const int	DEBUG_COUNTDOWN_TIME	= 5;
const int   DEFAULT_COUNTDOWN_TIME  = 30;
const int   DEFAULT_RELOADS         = 100;        // 100 = Unlimited
const int   DEFAULT_RELOADS2        = 0;          // 0xFFFF = LTAR unlimited
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
const int   REQUEST_ALL_DEBRIEF_BITS        = 15;
const int   REQUEST_TAG_SUMMARY_BIT         = 1;
const int   REQUEST_ALL_TEAM_REPORTS_BITS   = 14;
const int   REQUEST_TEAM1_REPORT_BIT        = 2;
const int   REQUEST_TEAM2_REPORT_BIT        = 4;
const int   REQUEST_TEAM3_REPORT_BIT        = 8;

//Score points values
const int   POINTS_PER_TAG_LANDED           =  2;
const int   POINTS_PER_HITS_TAKEN           = -1;
const int   POINTS_PER_TAG_LANDED_OWN_TEAM  = -2;
const int   POINTS_PER_KING_HIT             =  4;
const int   POINTS_PER_OWN_KING_HIT         = -4;
const int   POINTS_PER_ZONE_MINUTE          = 10;
const int   POINTS_PER_SURVIVAL_MINUTE      = 10;

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

//ScoresWindow modes
const int   ALL_IN_ONE_VIEW                 = 0;
const int   SUMMARY_VIEW                    = 1;
const int   PLAYER_GRID_VIEW                = 2;

//FileLoadSave window modes
const int	SAVE_MODE						= 1;
const int	LOAD_MODE						= 2;

//Classes for calledByWho
const int   LTTO_MAIN_WINDOW                = 2;
const int   PLAYERS_WINDOW                  = 3;

//RGB LED Colours
const QString		RED						= "1,0,0";
const QString		GREEN					= "0,1,0";
const QString		BLUE					= "0,0,1";
const QString		CYAN					= "0,1,1";
const QString		MAGENTA					= "1,0,1";
const QString		YELLOW					= "1,1,0";
const QString		WHITE					= "1,1,1";

#endif // DEFINES_H
