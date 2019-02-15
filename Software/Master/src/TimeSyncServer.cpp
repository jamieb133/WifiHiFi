#include "TimeSyncServer.h"
#include "config.h"
#include "types.h"
#include <unistd.h>


TimeSyncServer::TimeSyncServer()
{
    //clockType = clockTypeReq;
    socket = new QUdpSocket(this);
    bool isBound = socket->bind(QHostAddress::AnyIPv4, 1234, QUdpSocket::ShareAddress);
    qDebug() << "Socket bound?" << isBound << endl;

    connect(socket, SIGNAL(readyRead()), this, SLOT(UDPCallback()));

    
}

void TimeSyncServer::run()
{
    clock_getres(CLOCK_PROCESS_CPUTIME_ID, &m_timestamp);
    qDebug() << "Clock resolution: " << m_timestamp.tv_sec << "s" << m_timestamp.tv_nsec << "ns" << endl;
    //qDebug() << sizeof(m_timestamp.tv_sec) << sizeof(m_timestamp.tv_nsec) << endl;

    qDebug() << "Current time: " << m_timestamp.tv_sec << "s" << m_timestamp.tv_nsec << "ns" << endl;

    m_timer = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(GenerateSyncRequest()), Qt::DirectConnection);
    m_timer->start(1000);

    exec();
}

void TimeSyncServer::GenerateSyncRequest()
{
    /* get current time of process timer */
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &m_timestamp);

    char preamble = TIMESYNC_PREAMBLE;
    char syncReq = SYNC;
    
    /* populate outbound time-sync packet */
    QByteArray SyncPacket;
    SyncPacket.append(preamble);
    SyncPacket.append(syncReq);
    PackTimestamp(&m_timestamp, &SyncPacket);

    /* send */
    socket->writeDatagram(SyncPacket, QHostAddress("192.168.0.255"), 1234);
    qDebug() << "SENT SYNC-REQUEST: " << SyncPacket << endl;
    qDebug() << "Timestamp = " << m_timestamp.tv_nsec;

    //quint8 heya = static_cast<quint8>(SyncPacket[8]);
    //qDebug() << heya;
}

void TimeSyncServer::GenerateDelayResponse()
{
    char preamble = TIMESYNC_PREAMBLE;
    char delayResp = DELAY_RESP;

    /* populate outbound delay-response packet */
    QByteArray DelayRespPacket;
    DelayRespPacket.append(preamble);
    DelayRespPacket.append(delayResp);
    PackTimestamp(&m_timestamp, &DelayRespPacket);


    /* send */
    socket->writeDatagram(DelayRespPacket, QHostAddress("192.168.0.255"), 1234);
    qDebug() << "SENT DELAY-RESPONSE: " << DelayRespPacket <<endl;

}

void TimeSyncServer::UDPCallback()
{
    /* get current time of hardware timer */
    //clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &m_timestamp);
    //qDebug() << "Current time: " << m_timestamp.tv_sec << "s" << m_timestamp.tv_nsec << "ns" << endl;

    QByteArray Buffer;
    Buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;
    socket->readDatagram(Buffer.data(), Buffer.size(), &sender, &senderPort);

    quint8 preamble = static_cast<quint8>(Buffer[0]);
    quint8 msgType = static_cast<quint8>(Buffer[1]);
    
    if (preamble == TIMESYNC_PREAMBLE)
    {
        switch (msgType)
        {
            case SYNC :         //qDebug() << "Received sync message but running as master" << endl;
                                break;
            case DELAY_REQ:     //qDebug() << "Received delay-request, will process" << endl;
                                //GenerateDelayResponse();
                                break;
            case DELAY_RESP:    //qDebug() << "Received delay-response, but running as master" << endl;
                                break;
            default:            qDebug() << "ERROR: unrecognised time-sync message type" << endl;
                                break;
        }  
    }
    
}

void TimeSyncServer::PackTimestamp(struct timespec* ts, QByteArray* packet)
{
    quint8 timestampByte;
    quint32 tsVal;

    /* split seconds value into 4 bytes and append to packet */
    tsVal = (ts->tv_sec & 0xFF000000) >> 24;
    timestampByte = tsVal;
    packet->append(timestampByte);
    
    tsVal = (ts->tv_sec & 0x00FF0000) >> 16;
    timestampByte = tsVal;
    packet->append(timestampByte);
    tsVal = (ts->tv_sec & 0x0000FF00) >> 8;
    timestampByte = tsVal;
    packet->append(timestampByte);
    tsVal = ts->tv_sec & 0x000000FF;
    timestampByte = tsVal;
    packet->append(timestampByte);
    
    tsVal = (ts->tv_nsec & 0xFF000000) >> 24;
    timestampByte = tsVal;
    
    packet->append(timestampByte);
    //qDebug() << timestampByte;
    tsVal = (ts->tv_nsec & 0x00FF0000) >> 16;
    timestampByte = tsVal;
    packet->append(timestampByte);
    //qDebug() << timestampByte;
    tsVal = (ts->tv_nsec & 0x0000FF00) >> 8;
    timestampByte = tsVal;
    qDebug() << tsVal;
    packet->append(timestampByte);
    //qDebug() << timestampByte;
    tsVal = ts->tv_nsec & 0x000000FF;
    timestampByte = tsVal;
    packet->append(timestampByte);
    //qDebug() << timestampByte;
}

