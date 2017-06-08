#-------------------------------------------------
#
# Project created by QtCreator 2016-09-05T20:24:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network serialport bluetooth

TARGET = LTTO_Host
TEMPLATE = app


SOURCES +=  main.cpp\
            lttomainwindow.cpp \
            PlayersWindow.cpp \
            Game.cpp \
            Players.cpp \
            BluetoothWindow.cpp \
            HostGameWindow.cpp \
	    LazerSwarm.cpp \
	    SerialComms.cpp \
	    PortConnect.cpp \
	    TCPComms.cpp

HEADERS  += lttomainwindow.h \
            PlayersWindow.h \
            Game.h \
            Players.h \
            BluetoothWindow.h \
            HostGameWindow.h \
	    LazerSwarm.h \
	    SerialComms.h \
	    PortConnect.h \
	    TCPComms.h \
	    Defines.h

FORMS    += lttomainwindow.ui \
            PlayersWindow.ui \
            synctaggers.ui \
            BluetoothWindow.ui \
	    HostGameWindow.ui \
	    PortConnect.ui \
	    TCPComms.ui
