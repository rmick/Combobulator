#-------------------------------------------------
#
# Project created by QtCreator 2016-09-05T20:24:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network serialport  # REMOVE the hash before the word serialport to enable USB for Lazerswarm
TARGET = LTTO_Host
DEPLOYMENT.display_name = Combobulator
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
	    LttoComms.cpp \
	    FlagsWindow.cpp \
	    SerialUSBcomms.cpp \
	    ReHostTagger.cpp \
	    AboutForm.cpp \
	    DeBrief.cpp



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
	    LttoComms.h \
	    FlagsWindow.h \
	    SerialUSBcomms.h \
	    ReHostTagger.h \
	    AboutForm.h \
	    DeBrief.h


FORMS    += \
            PlayersWindow.ui \
	    HostGameWindow.ui \
	    PortConnect.ui \
	    LttoMainWindow.ui \
	    SyncTaggers.ui \
	    FlagsWindow.ui \
	    ReHostTagger.ui \
	    AboutForm.ui
