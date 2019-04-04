#setup
TEMPLATE    = app 
TARGET      = WifiHifiControl
QT          += widgets qml quick quickwidgets multimedia multimediawidgets testlib 

#includes
INCLUDEPATH += inc/
HEADERS     += ./inc/*.h

#libraries

#inputs

DESTDIR         = bin 
OBJECTS_DIR     = ./bin/.obj 

MOC_DIR         = ./src/moc

SOURCES     += src/*.cpp
