#include <QCoreApplication>
#include <iostream>

#include "SlaveProcessor.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QtJack::Client client;
    client.connectToServer("WiSpeak");

    SlaveProcessor processor(client);
    client.setMainProcessor(&processor);
    client.activate();

    return a.exec();
}
