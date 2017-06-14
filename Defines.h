#ifndef DEFINES_H
#define DEFINES_H

const char  PACKET      = 'P';
const char  DATA        = 'D';
const char  CHECKSUM    = 'C';
const char  TAG         = 'T';
const char  BEACON      = 'B';



const int   ANNOUNCE    = 0x00;

const int   COUNTDOWN               = 0x00;
const int   ASSIGN_PLAYER           = 0X01;
const int   ANNOUNCE_CUSTOM_LTAG    = 0x02;
const int   ANNOUCE_SPECIAL_GAME    = 0x0C;

const int   ASSIGN_PLAYER_FAIL      = 0x0F;
const int   REQUEST_JOIN_GAME       = 0x10;
const int   ACK_PLAYER_ASSIGN       = 0x11;
const int   REQUEST_ASSISTANCE      = 0x20;

const int   TEXT_MESSAGE            = 0x80;
const int   SPECIAL_ATTACK          = 0x90;

const bool  DECIMAL                 = false;
const bool  BCD                     = true;

const int   DEFAULT_GAME_TYPE       = 0x02;
const int   DEFAULT_GAME_ID         = 0x64;
const int   DEFAULT_GAME_LENGTH     = 10;
const int   DEFAULT_COUNTDOWN_TIME  = 5;
const int   DEFAULT_RELOADS         = 100;        // 100 = Unlimited
const int   DEFAULT_HEALTH          = 25;
const int   DEFAULT_SHEILDS         = 15;
const int   DEFAULT_MEGAS           = 20;
const int   DEFAULT_FLAGS1          = 0x20;
const int   DEFAULT_FLAGS2          = 0x1;


#endif // DEFINES_H
