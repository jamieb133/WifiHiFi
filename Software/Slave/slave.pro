#setup
TEMPLATE        = app
CONFIG          += console
CONFIG          -= app_bundle
QT              -= gui
QT              += network

#includes
HEADERS         = ./inc/*.h
INCLUDEPATH     += ./inc/
INCLUDEPATH     += ../lib/qtjack

#inputs
SOURCES         = ./src/*.cpp

LIBS += -L/usr/lib/arm-linux-gnueabihf -lasound

LIBS += -L/usr/lib/arm-linux-gnueabihf -ljack
LIBS += -L../lib/qtjack -lqtjack
LIBS += -L$LIBSRC -lfir

PRE_TARGETDEPS += ../lib/qtjack/libqtjack.a

#outputs
TARGET          = slave
DESTDIR         = bin 
OBJECTS_DIR     = bin/src
MOC_DIR         = moc