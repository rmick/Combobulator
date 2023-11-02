#-------------------------------------------------
#
# Project created by QtCreator 2016-09-05T20:24:28
#
#-------------------------------------------------

QT       += core gui\

CONFIG  += c++11 sdk_no_version_check

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network multimedia

iphoneos{greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network multimedia}
else    {greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network multimedia serialport}

TARGET = Combobulator
DEPLOYMENT.display_name = Combobulator
TEMPLATE = app

#RC_FILE = Combobulator.rc
ICON	= Combobulator.icns
#RC_ICONS = Combobulator.ico

QMAKE_MAC_SDK = macosx13.4
!host_build:QMAKE_MAC_SDK = macosx
#QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64


SOURCES +=  \
    main.cpp\
    PlayersWindow.cpp \
    Game.cpp \
    Players.cpp \
    HostGameWindow.cpp \
    LazerSwarm.cpp \
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
    SettingsWindow.cpp \
    Logger.cpp


HEADERS  += \
    PlayersWindow.h \
    Game.h \
    Players.h \
    HostGameWindow.h \
    LazerSwarm.h \
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
    SettingsWindow.h \
    Logger.h \
    Bugs.h


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
    SettingsWindow.ui \
    Logger.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

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

#Uncommenting this breaks the Android build with :-1: error: Could not determine java version from '11.0.20'.
#ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

#DESTDIR = ./
