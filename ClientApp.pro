QMAKE_CXXFLAGS += -Wno-implicit-function-declaration
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += core widgets charts network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    formauth.cpp \
    formreg.cpp \
    formresetpassword.cpp \
    formsolve.cpp \
    formtaskchoice.cpp \
    functionsforclient.cpp \
    main.cpp \
    mainwindow.cpp \
    mathfunction.cpp \
    tcpclient.cpp

HEADERS += \
    formauth.h \
    formreg.h \
    formresetpassword.h \
    formsolve.h \
    formtaskchoice.h \
    functionsforclient.h \
    mainwindow.h \
    mathfunction.h \
    tcpclient.h

FORMS += \
    formauth.ui \
    formreg.ui \
    formresetpassword.ui \
    formsolve.ui \
    formtaskchoice.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
