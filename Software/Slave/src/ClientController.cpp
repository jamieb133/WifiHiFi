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
#include <wiringPiI2C.h>
#include <math.h>

#define MCAST_ADDR "239.1.1.1"
#define DISCOVERY_PORT 1233
#define CONTROL_PORT 1234

ClientController::ClientController(HardwareController* hw, AlsaWorker* alsa)
{
    m_alsa = alsa;
    m_hw = hw;

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

    /* logarithmic volume */
    m_dbVols = new int[100];
    for (int count = 1; count <= 100; count++)
    {
        m_dbVols[count-1] = 30*log10(static_cast<float>(count)); //scale from 0 to 60 in linear gain
        std::cout << m_dbVols[count-1] << std::endl;
    }
}

void ClientController::Start()
{

}

void ClientController::readyRead()
{
    //qDebug() << "CLIENT CONTROLLER: received datagram";

    QByteArray buffer;
    buffer.resize(m_socket->pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;
    m_socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
    bool ok;
    filterConfig_t settings;

    switch(buffer.operator[](0))
        {
            case 'M':
            {
                buffer.remove(0, 1);
                //qDebug() << "MID REQUEST: " << sender.toString() << endl;
                //qDebug() << "Value: " << buffer.operator[](0) << endl;

                //buffer.remove(0, 1);
                float midRequest = buffer.toFloat(&ok);
                settings = m_alsa->EQSettings().mid;
                /* scale gain from -10 to 0 dB */
                settings.dbGain = (midRequest / 10.0) - 10.0;
                std::cout << "Mid request: " << midRequest << std::endl; 
                m_alsa->AdjustMid(&settings);
                break;
            }
            case 'B':
            {
                //qDebug() << "BASS REQUEST: " << sender.toString() << endl;
                //qDebug() << "Value: " << buffer.data() << endl;

                buffer.remove(0, 1);
                float bassRequest = buffer.toFloat(&ok);
                settings = m_alsa->EQSettings().bass;
                /* scale gain from -10 to 0 dB */
                settings.dbGain = (bassRequest / 10.0) - 10.0;
                if(settings.dbGain > 0.0) settings.dbGain = 0.0;
                if(settings.dbGain < -10.0) settings.dbGain = -10.0;
            
                m_alsa->AdjustBass(&settings);
                break;
            }
            case 'T':
            {
                //qDebug() << "TREBLE REQUEST: " << sender.toString() << endl;
                //qDebug() << "Value: " << buffer.data() << endl;

                buffer.remove(0, 1);
                float trebleRequest = buffer.toFloat(&ok);
                settings = m_alsa->EQSettings().treble;
                /* scale gain from -10 to 0 dB */
                settings.dbGain = (trebleRequest / 10.0) - 10.0;

                m_alsa->AdjustTreble(&settings);
                break;
            }
            case 'V':
            {
                //qDebug() << "VOLUME REQUEST: " << sender.toString() << endl;
                //qDebug() << "Value: " << buffer.data() << endl;

                buffer.remove(0, 1);
                //float vol = (buffer.toFloat(&ok) / 100.0) * 45.0;
                //vol = (vol / 100.0) * 30.0;
                HardwareVolume(buffer.toInt(&ok));
                
                //m_alsa->Attenuate(vol);

                break;
            }
            case 'E':
            {
                eqOn = !eqOn;
                //qDebug() << "EQ REQUEST: " << sender.toString();
                //qDebug() << "Value: " << eqOn;

                if (eqOn)
                {
                    m_alsa->DisableEq();
                }
                else
                {
                    m_alsa->EnableEq();
                }
                break;
            }
                
        }
}

void ClientController::HardwareVolume(int vol)
{
    int fd;
    if (vol <= 0) vol = 1;
    if (vol >= 100)  vol = 100;
    assert( (vol >= 0) && (vol <= 100));
    
    fd = wiringPiI2CSetup(0x4B);

    if (int err = wiringPiI2CWrite(fd, m_dbVols[vol]) < 0)
    //if (int err = wiringPiI2CWrite(fd, 0) < 0)
    {
        std::cout << "CLIENT CONTROLLER: failed to write hardware volume" << std::endl;
    }
    

    std::cout << "Volume request DB" << m_dbVols[vol] << std::endl;
    std::cout << "Volume request linear" << vol << std::endl;


}