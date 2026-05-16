QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET   = RoomManagementSystem
TEMPLATE = app

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    RoomManager.cpp

HEADERS += \
    MainWindow.h \
    RoomManager.h
