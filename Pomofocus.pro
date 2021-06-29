QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Components/minutessecondsbox.cpp \
    Components/roundprogressbar.cpp \
    Components/soundscontrolpanel.cpp \
    Components/spinsliderbox.cpp \
    Components/toggleswitch.cpp \
    Scenes/mainscene.cpp \
    Scenes/settingsscene.cpp \
    main.cpp

HEADERS += \
    Components/minutessecondsbox.h \
    Components/roundprogressbar.h \
    Components/soundscontrolpanel.h \
    Components/spinsliderbox.h \
    Components/toggleswitch.h \
    Scenes/mainscene.h \
    Scenes/settingsscene.h

FORMS += \
    Components/minutessecondsbox.ui \
    Components/soundscontrolpanel.ui \
    Components/spinsliderbox.ui \
    Scenes/mainscene.ui \
    Scenes/settingsscene.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
