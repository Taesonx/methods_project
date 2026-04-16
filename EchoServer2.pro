QT -= gui
QT += network sql

CONFIG += c++11 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mytcpserver.cpp \
    request_handler.cpp \
    databasemanager.cpp

HEADERS += \
    mytcpserver.h \
    request_handler.h \
    databasemanager.h \
    function29.h
