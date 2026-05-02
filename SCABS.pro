QT += core gui widgets

CONFIG += c++17
TARGET = SCABS
TEMPLATE = app

SOURCES += scabs_gui.cpp
HEADERS += scabs_backend.h

# Windows: hide console window
win32: RC_ICONS =
win32: CONFIG += windows

# Ensure moc works
CONFIG += no_keywords
