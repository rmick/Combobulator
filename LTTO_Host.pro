#-------------------------------------------------
#
# Project created by QtCreator 2016-09-05T20:24:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network serialport
TARGET = LTTO_Host
TEMPLATE = app


SOURCES +=  main.cpp\
            PlayersWindow.cpp \
            Game.cpp \
            Players.cpp \
            HostGameWindow.cpp \
	    LazerSwarm.cpp \
	    PortConnect.cpp \
	    TCPComms.cpp \
            LttoMainWindow.cpp \
	    QObjectClass.cpp \
	    LttoComms.cpp\
	    SerialUSBcomms.cpp \
	    FlagsWindow.cpp


HEADERS  += \
            PlayersWindow.h \
            Game.h \
            Players.h \
            HostGameWindow.h \
	    LazerSwarm.h \
	    PortConnect.h \
	    TCPComms.h \
	    Defines.h \
	    LttoMainWindow.h \
	    QObjectClass.h \
	    LttoComms.h \
	    SerialUSBcomms.h \
	    FlagsWindow.h

FORMS    += \
            PlayersWindow.ui \
	    HostGameWindow.ui \
	    PortConnect.ui \
	    LttoMainWindow.ui \
	    SyncTaggers.ui \
	    FlagsWindow.ui
