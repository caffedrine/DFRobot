#-------------------------------------------------
#
# Project created by Alex C. 2017-03-18T16:39:25
#
#-------------------------------------------------

QT       += core gui serialport network qml quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DFRobot_Server
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES +=  main.cpp\
            mainwindow.cpp \
            tcpserver.cpp \
            ../libs/datastructure.cpp \
            ../libs/Communication/GUI.cpp \
            ../libs/Communication/Joystick.cpp \
            ../libs/Communication/Serial.cpp \
            ../libs/Communication/I2C.cpp \
            #../libs/Communication/TCPClient.cpp \
            #../libs/Communication/TCPServer.cpp \
            serialconn.cpp

HEADERS  += mainwindow.h \
            tcpserver.h \
            ../libs/datastructure.h \
            ../libs/Communication/ICommunication.h \
            ../libs/Communication/GUI.h \
            ../libs/Communication/Joystick.h \
            ../libs/Communication/Serial.h \
            ../libs/Communication/I2C.h \
            #../libs/Communication/TCPClient.h \
            #../libs/Communication/TCPServer.h \
            serialconn.h

FORMS    += mainwindow.ui

DISTFILES += \
    CircularGauge.qml
