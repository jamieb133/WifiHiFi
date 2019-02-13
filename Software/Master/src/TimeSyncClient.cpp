#include "TimeSyncClient.h"
#include "config.h"
#include "types.h"

TimeSyncClient::TimeSyncClient(QObject* parent) : QObject(parent)
{
    //clockType = clockTypeReq;
    socket = new QUdpSocket(this);
    bool isBound = socket->bind(QHostAddress::AnyIPv4, 1234, QUdpSocket::ShareAddress);
    qDebug() << "Socket bound?" << isBound << endl;

    connect(socket, SIGNAL(readyRead()), this, SLOT(UDPCallback()));

    m_timestamp = 0;
}


void TimeSyncClient::ProcessSyncRequest(int RxTimestamp)
{
    m_timeoffset = m_timestamp - RxTimestamp;
    qDebug() << "OFFSET = " << m_timeoffset << endl;
    GenerateDelayRequest();
}

void TimeSyncClient::GenerateDelayRequest()
{
    QByteArray DelayPacket;
    DelayPacket.append(TIMESYNC_PREAMBLE);
    DelayPacket.append(DELAY_REQ);
    DelayPacket.append(m_timestamp);
    socket->writeDatagram(DelayPacket, QHostAddress(TIMESYNC_MCAST), 1234);
    qDebug() << "SENT DELAY REQUEST" << endl;
}

void TimeSyncClient::UDPCallback()
{
    /* get current timestamp */
    ReadTime(&m_timestamp);

    QByteArray Buffer;
    Buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;
    socket->readDatagram(Buffer.data(), Buffer.size(), &sender, &senderPort);

    quint8 preamble = static_cast<quint8>(Buffer[0]);
    quint8 msgType = static_cast<quint8>(Buffer[1]);
    quint16 upperTimestamp = static_cast<quint8>(Buffer[2]);
    upperTimestamp = upperTimestamp << 8;
    quint16 lowerTimestamp = static_cast<quint8>(Buffer[3]);
    int RxTimestamp = upperTimestamp | lowerTimestamp;

    if (preamble == TIMESYNC_PREAMBLE)
    {
        switch (msgType)
        {
            case SYNC :         qDebug() << "Received sync message, processing" << endl;
                                ProcessSyncRequest(RxTimestamp);
                                break;
            case DELAY_REQ:     qDebug() << "Received delay-request, not processing" << endl;
                                break;
            case DELAY_RESP:    qDebug() << "Received delay-response, processing" << endl;
                                break;
            default:            qDebug() << "ERROR: unrecognised time-sync message type" << endl;
                                break;
        }  
    }

}

void TimeSyncClient::ReadTime(int* ts)
{
    //*ts = (m_timer->interval) - (m_timer->remainingTime)
    *ts = 40;
}
