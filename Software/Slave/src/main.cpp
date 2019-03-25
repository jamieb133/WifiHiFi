/**
 * @file main.cpp
 * @author Jamie Brown
 * @brief 
 * @version 0.1
 * @date 2019-03-25
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <QCoreApplication>
#include <QWaitCondition>
#include <iostream>

#include "SlaveProcessor.h"
#include "LocalThread.h"
#include "AlsaWorker.h"

using namespace std;

QWaitCondition notify;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QtJack::Client client;
    client.connectToServer("WiSpeak");

    SlaveProcessor processor(client);
    client.setMainProcessor(&processor);

    LocalThread localT;
    AlsaWorker alsaW(client, &processor);
    alsaW.moveToThread(&localT);
    QObject::connect(&localT, SIGNAL(started()), &alsaW, SLOT(Work()));

    /* start client and local thread */
    client.activate();
    localT.start();

    return a.exec();
}
