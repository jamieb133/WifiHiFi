#ifndef TIMESYNCSERVER_H
#define TIMESYNCSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QtCore>
#include "types.h"

class TimeSyncServer : public QObject
{
    Q_OBJECT

public:
    explicit TimeSyncServer(QObject* parent = 0);
    void StartNetworkSync();
    
signals:
    
public slots:
    void UDPCallback();
    void GenerateSyncRequest();

private:
    void GenerateDelayResponse();
    void ReadTime(int *ts);

    int m_timestamp;
    QTimer* m_timer;
    QUdpSocket* socket;
    QHostAddress* destIP;
    SyncServer_t clockType;
};

#endif //TIMESYNCSERVER_H