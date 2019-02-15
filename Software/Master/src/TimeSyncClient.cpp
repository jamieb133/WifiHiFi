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


void TimeSyncClient::ProcessSyncRequest(quint32 RxSecs, quint32 RxNSecs)
{

    
    GenerateDelayRequest();
}

void TimeSyncClient::GenerateDelayRequest()
{
    QByteArray DelayPacket;
    DelayPacket.append(TIMESYNC_PREAMBLE);
    DelayPacket.append(DELAY_REQ);
    DelayPacket.append(m_timestamp);
    socket->writeDatagram(DelayPacket, QHostAddress("192.168.0.255"), 1234);
    qDebug() << "SENT DELAY REQUEST" << endl;
}

void TimeSyncClient::ProcessDelayResponse(quint32 RxSecs, quint32 RxNSecs)
{
    
}

void TimeSyncClient::UDPCallback()
{
    /* get current timestamp */
    //ReadTime(&m_timestamp);

    QByteArray Buffer;
    Buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;
    socket->readDatagram(Buffer.data(), Buffer.size(), &sender, &senderPort);

    qDebug() << Buffer << endl;

    quint8 preamble = static_cast<quint8>(Buffer[0]);
    quint8 msgType = static_cast<quint8>(Buffer[1]);


    if (preamble == TIMESYNC_PREAMBLE)
    {
        switch (msgType)
        {
            case SYNC :         qDebug() << "Received sync message, processing" << endl;
                                RxSecs = 0;
                                RxNSecs = UnpackTimestamp(Buffer, sizeof(Buffer));
                                qDebug() << "Master timestamp = " << RxNSecs;
                                ProcessSyncRequest(RxSecs, RxNSecs);
                                break;
            case DELAY_REQ:     //qDebug() << "Received delay-request, not processing" << endl;
                                break;
            case DELAY_RESP:    qDebug() << "Received delay-response, processing" << endl;
                                RxSecs = 0;
                                RxNSecs = UnpackTimestamp(Buffer, sizeof(Buffer));

                                break;
            default:            qDebug() << "ERROR: unrecognised time-sync message type" << endl;
                                break;
        }  
    }

}

quint32 TimeSyncClient::UnpackTimestamp(QByteArray packet, int size)
{
    int index = size + 1;

    quint8 timestampByte;
    quint32 ts = 0;
    quint32 val;
    timestampByte = static_cast<quint8>(packet[6]);
    val = timestampByte;
    val = val << 24;
    ts |= val; 
    timestampByte = static_cast<quint8>(packet[7]);
    val = timestampByte << 16;
    ts |= val;
    timestampByte = static_cast<quint8>(packet[8]);
    val = timestampByte << 8;
    ts |= val;
    timestampByte = static_cast<quint8>(packet[9]);
    val = timestampByte ;
    ts |= val;
    
    return ts;
    
}
