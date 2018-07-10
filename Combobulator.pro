#-------------------------------------------------
#
# Project created by QtCreator 2016-09-05T20:24:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network multimedia serialport  # REMOVE the hash before the word serialport to enable USB for Lazerswarm
TARGET = Combobulator
DEPLOYMENT.display_name = Combobulator
TEMPLATE = app

#RC_FILE = Combobulator.rc
ICON	= Combobulator.icns

QMAKE_MAC_SDK = macosx10.13

SOURCES +=  \
    main.cpp\
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
    DeBrief.cpp \
    Hosting.cpp \
    RearrangePlayers.cpp \
    StyleSheet.cpp \
    ScoresWindow.cpp \
    SetScoreParameters.cpp \
    FileLoadSave.cpp \
    OtaWindow.cpp \
    SettingsWindow.cpp


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
    DeBrief.h \
    Hosting.h \
    StyleSheet.h \
    RearrangePlayers.h \
    ScoresWindow.h \
    SetScoreParameters.h \
    FileLoadSave.h \
    OtaWindow.h \
    SettingsWindow.h


FORMS    += \
    PlayersWindow.ui \
    HostGameWindow.ui \
    PortConnect.ui \
    LttoMainWindow.ui \
    SyncTaggers.ui \
    FlagsWindow.ui \
    ReHostTagger.ui \
    AboutForm.ui \
    RearrangePlayers.ui \
    ScoresWindow.ui \
    SetScoreParameters.ui \
    FileLoadSave.ui \
    OtaWindow.ui \
    SettingsWindow.ui


RESOURCES += \
    Resources.qrc


DISTFILES += \
    resources/images/Combobulator Logo.jpg \
    resources/audio/countdown/wav \
    resources/audio/game-over.wav \
    resources/audio/good-luck.wav \
    resources/audio/hosting-aborted.wav \
    resources/audio/hosting-join-complete.wav \
    resources/audio/hosting-listening.wav \
    resources/audio/hosting-problem.wav \
    resources/audio/menu-beep.wav \
    resources/audio/menu-blip.wav \
    resources/audio/power-up.wav \
    resources/audio/shut-down.wav \
    resources/audio/stinger-power-on.wav \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

DESTDIR = ./
