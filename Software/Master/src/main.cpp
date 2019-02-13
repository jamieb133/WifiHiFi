#include <iostream>
#include <QtCore>
#include <QCoreApplication>
#include "TimeSyncServer.h"
#include "TimeSyncClient.h"
//#include "config.h"

using namespace std;

int main(int argc, char **argv)
{
    //cout << "hello" << endl;
    QCoreApplication app(argc, argv);

    //TimeSyncServer server;
    TimeSyncClient client;

    //server.StartNetworkSync();

    return app.exec();
}