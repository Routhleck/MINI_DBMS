VERSION = 1.2
QMAKE_TARGET_DESCRIPTION = "Qt app that displays SQL table; for educational purposes"
QMAKE_TARGET_COPYRIGHT   = "Andrzej Wojtowicz, Adam Mickiewicz University in Poznan"
QMAKE_TARGET_PRODUCT     = "Qt SQL Example"

QT      += core gui sql widgets

TARGET   = qt-sql-example
TEMPLATE = app


SOURCES += src/main.cpp\
           src/mainwindow.cpp \
           src/db_controller.cpp

HEADERS += src/mainwindow.h \
           src/db_controller.h

#QMAKE_USE +=mysql

FORMS   += src/mainwindow.ui

RESOURCES += res/icons.qrc

win32:RC_ICONS += res/database.ico

win32:LIBS += -L$$quote(C:/Program Files/MySQL/MySQL Server 5.5/lib -llibmysql)

INCLUDEPATH += $$quote(C:/Program Files/MySQL/MySQL Server 5.54/include)

DEPENDPATH += $$quote(C:/Program Files/MySQL/MySQL Server 5.5/include)
