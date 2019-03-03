#setup
TEMPLATE        = app
CONFIG          += console
CONFIG          -= app_bundle
QT              -= gui

#includes
HEADERS         = ./inc/*.h
INCLUDEPATH     += ./inc/
INCLUDEPATH     += ../qtjack

#inputs
SOURCES         = ./src/*.cpp

LIBS += -L/usr/lib/arm-linux-gnueabihf -ljack
LIBS += -L../qtjack -lqtjack
LIBS += -L$LIBSRC -lfir

#outputs
TARGET          = slave
DESTDIR         = bin 
OBJECTS_DIR     = bin/src
MOC_DIR         = moc