QMAKE_CXXFLAGS += -Wno-implicit-function-declaration
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += core widgets charts

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    formauth.cpp \
    formreg.cpp \
    formsolve.cpp \
    formtaskchoice.cpp \
    functionsforclient.cpp \
    main.cpp \
    mainwindow.cpp \
    mathfunction.cpp

HEADERS += \
    formauth.h \
    formreg.h \
    formsolve.h \
    formtaskchoice.h \
    functionsforclient.h \
    mainwindow.h \
    mathfunction.h

FORMS += \
    formauth.ui \
    formreg.ui \
    formsolve.ui \
    formtaskchoice.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
