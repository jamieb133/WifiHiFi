#ifndef TIMESYNCCLIENT_H
#define TIMESYNCCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QtCore>
#include "types.h"

class TimeSyncClient : public QObject
{
    Q_OBJECT

public:
    explicit TimeSyncClient(QObject* parent = 0);
    
signals:
    
public slots:
    void UDPCallback();

private:
    void ProcessSyncRequest(quint32 RxSecs, quint32 RxNSecs);
    void GenerateDelayRequest();
    void ProcessDelayResponse(quint32 RxSecs, quint32 RxNSecs);
    quint32 UnpackTimestamp(QByteArray packet, int size);

    int m_timestamp;
    int m_timeoffset;
    QTimer* m_timer;
    QUdpSocket* socket;
    quint32 RxSecs;
    quint32 RxNSecs;

    
};

#endif //TIMESYNCCLIENT_H