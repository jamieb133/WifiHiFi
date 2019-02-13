#include "TimeSyncServer.h"
#include "config.h"
#include "types.h"

TimeSyncServer::TimeSyncServer(QObject* parent) : QObject(parent)
{
    //clockType = clockTypeReq;
    socket = new QUdpSocket(this);
    bool isBound = socket->bind(QHostAddress::AnyIPv4, 1234, QUdpSocket::ShareAddress);
    qDebug() << "Socket bound?" << isBound << endl;

    connect(socket, SIGNAL(readyRead()), this, SLOT(UDPCallback()));

    m_timestamp = 0;
}

void TimeSyncServer::StartNetworkSync()
{
    m_timer = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(SendSyncRequest()), Qt::DirectConnection);
    m_timer->start(1000);
}

void TimeSyncServer::GenerateSyncRequest()
{
    /* get current time of hardware timer */
    ReadTime(&m_timestamp);

    /* populate outbound time-sync packet */
    QByteArray SyncPacket;
    SyncPacket.append(TIMESYNC_PREAMBLE);
    SyncPacket.append(SYNC);
    SyncPacket.append(m_timestamp);

    /* send */
    socket->writeDatagram(SyncPacket, QHostAddress(TIMESYNC_MCAST), 1234);
    qDebug() << "SENT SYNC-REQUEST" <<endl;
}

void TimeSyncServer::GenerateDelayResponse()
{
    /* get current time of hardware timer */
    ReadTime(&m_timestamp);

    /* populate outbound delay-response packet */
    QByteArray DelayRespPacket;
    DelayRespPacket.append(TIMESYNC_PREAMBLE);
    DelayRespPacket.append(DELAY_RESP);
    DelayRespPacket.append(m_timestamp); 

    /* send */
    socket->writeDatagram(DelayRespPacket, QHostAddress(TIMESYNC_MCAST), 1234);
    qDebug() << "SENT DELAY-RESPONSE" <<endl;

}

void TimeSyncServer::UDPCallback()
{
    QByteArray Buffer;
    Buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;
    socket->readDatagram(Buffer.data(), Buffer.size(), &sender, &senderPort);

    int RxTimestamp;
    quint8 preamble = static_cast<quint8>(Buffer[0]);
    quint8 msgType = static_cast<quint8>(Buffer[1]);
    quint16 upperTimestamp = static_cast<quint8>(Buffer[2]);
    upperTimestamp = upperTimestamp << 8;
    quint16 lowerTimestamp = static_cast<quint8>(Buffer[3]);
    RxTimestamp = upperTimestamp | lowerTimestamp;

    if (preamble == TIMESYNC_PREAMBLE)
    {
        switch (msgType)
        {
            case SYNC :         qDebug() << "Received sync message but running as master" << endl;
                                break;
            case DELAY_REQ:     qDebug() << "Received delay-request, will process" << endl;
                                GenerateDelayResponse();
                                break;
            case DELAY_RESP:    qDebug() << "Received delay-response, but running as master" << endl;
                                break;
            default:            qDebug() << "ERROR: unrecognised time-sync message type" << endl;
                                break;
        }  
    }
    

}

void TimeSyncServer::ReadTime(int* ts)
{
    //*ts = (m_timer->interval) - (m_timer->remainingTime)
    *ts = 40;
}
