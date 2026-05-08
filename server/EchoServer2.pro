QT -= gui
QT += network
QT += network sql

CONFIG += c++11 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    databasemanager.cpp \
    main.cpp \
    mytcpserver.cpp
    databasemanager.cpp

HEADERS += \
    databasemanager.h \
    function29.h \
    mytcpserver.h
    databasemanager.h
    function29.h
