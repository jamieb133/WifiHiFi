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
    void ProcessSyncRequest(int RxTimestamp);
    void GenerateDelayRequest();
    void ProcessDelayResponse();
    void ReadTime(int *ts);

    int m_timestamp;
    int m_timeoffset;
    QTimer* m_timer;
    QUdpSocket* socket;
};

#endif //TIMESYNCCLIENT_H