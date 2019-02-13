#setup
TEMPLATE        = app
CONFIG          += qt core
QT              += widgets testlib network

#includes
HEADERS         = ./inc/*.h
INCLUDEPATH     += ./inc/

#inputs
SOURCES         = ./src/*.cpp

#outputs
TARGET          = Master
DESTDIR         = bin 
OBJECTS_DIR     = bin/src
MOC_DIR         = moc