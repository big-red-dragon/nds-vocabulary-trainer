#-------------------------------------------------
#
# Project created by QtCreator 2012-11-04T19:39:52
#
#-------------------------------------------------

QT       += core gui
CONFIG += console c++11
CONFIG -= app_bundle

TARGET = nds-trainer-tool
TEMPLATE = app


SOURCES += main.cpp\
    NDSFont.cpp \
    TaskRunner.cpp \
    tasks/ButtonImgConverter.cpp \
    tasks/BackgroundImgConverter.cpp \
    TyFile.cpp \
    tasks/VocabularyConverter.cpp \
    TextDB.cpp \
    TyDataStream.cpp

HEADERS  += \
    NDSFont.h \
    Task.h \
    TaskRunner.h \
    tasks/ButtonImgConverter.h \
    tasks/BackgroundImgConverter.h \
    TyFile.h \
    tasks/VocabularyConverter.h \
    TextDB.h \
    TyDataStream.h
