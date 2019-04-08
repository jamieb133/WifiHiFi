/**
 * @file ClientController.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "ClientController.h"

#define MCAST_ADDR "239.1.1.1"
#define DISCOVERY_PORT 1233
#define CONTROL_PORT 1234

ClientController::ClientController(AlsaWorker* alsa)
{
    m_alsa = alsa;

    /* setup udp socket */
    m_socket = new QUdpSocket(this);
    if(!m_socket->bind(QHostAddress::AnyIPv4, CONTROL_PORT, QUdpSocket::ShareAddress))
    {
        qDebug() << "ERROR: could not bind udp socket";
    }
    else
    {

        if(!m_socket->joinMulticastGroup(QHostAddress(MCAST_ADDR)))
        {
            qDebug() << "ERROR: could not join multicast group";
        }
    }
    
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void ClientController::Start()
{

}

void ClientController::readyRead()
{
    qDebug() << "CLIENT CONTROLLER: received datagram";

    QByteArray buffer;
    buffer.resize(m_socket->pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;
    m_socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

    switch(buffer.operator[](0))
        {
            case 'M':
                buffer.remove(0, 1);
                qDebug() << "MID REQUEST: " << sender.toString() << endl;
                qDebug() << "Value: " << buffer.operator[](0) << endl;
                break;
            case 'B':
                qDebug() << "BASS REQUEST: " << sender.toString() << endl;
                qDebug() << "Value: " << buffer.data() << endl;
                break;
            case 'T':
                qDebug() << "TREBLE REQUEST: " << sender.toString() << endl;
                qDebug() << "Value: " << buffer.data() << endl;
                break;
            case 'V':
                qDebug() << "VOLUME REQUEST: " << sender.toString() << endl;
                qDebug() << "Value: " << buffer.data() << endl;

                buffer.remove(0, 1);
                bool ok;
                float vol = buffer.toFloat(&ok) / 100.0;
                //qDebug() << vol;
                m_alsa->Attenuate(vol);
                break;
        }
}