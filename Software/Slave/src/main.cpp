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
#include <QThread>
#include <iostream>

#include "SlaveProcessor.h"
#include "LocalThread.h"
#include "AlsaWorker.h"
#include "ClientController.h"

using namespace std;

QWaitCondition notify;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QtJack::Client client;
    client.connectToServer("WifiHifi");

    SlaveProcessor processor(client);
    client.setMainProcessor(&processor);

    LocalThread localT;
    AlsaWorker alsaW(client, &processor);
    alsaW.moveToThread(&localT);
    QObject::connect(&localT, SIGNAL(started()), &alsaW, SLOT(Work()));

    QThread controllerThread;
    ClientController controller(&alsaW);
    controller.moveToThread(&controllerThread);
    QObject::connect(&controllerThread, SIGNAL(started()), &controller, SLOT(Start()));

    /* start client and local thread */
    client.activate();
    localT.start();
    controllerThread.start();

    return a.exec();
}
