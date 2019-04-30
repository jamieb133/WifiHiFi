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
#include <server.h>

#include "SlaveProcessor.h"
#include "LocalThread.h"
#include "AlsaWorker.h"
#include "ClientController.h"
#include "HardwareController.h"

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

    /* setup server */
    /*
    QtJack::Server* jackServer = new QtJack::Server();
    QtJack::DriverMap drivers = jackServer->availableDrivers();
    QtJack::Driver netDriver = drivers["net"];
    QtJack::ParameterMap parameters = netDriver.parameters();
    parameters["capture"].setValue(1);
    */
    
    QThread buttonThread;
    HardwareController hwController;
    hwController.moveToThread(&buttonThread);
    QObject::connect(&buttonThread, SIGNAL(started()), &hwController, SLOT(Work()));

    QThread controllerThread;
    ClientController controller(&hwController, &alsaW);
    controller.moveToThread(&controllerThread);
    QObject::connect(&controllerThread, SIGNAL(started()), &controller, SLOT(Start()));


    /* start client and local thread */
    //jackServer->start(netDriver);  //start the server
    client.activate();
    localT.start();
    controllerThread.start();
    buttonThread.start();

    return a.exec();
}
