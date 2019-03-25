
#ifndef ALSAWORKER_H
#define ALSAWORKER_H

#include "AlsaController.h"
#include "SlaveProcessor.h"

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <RingBuffer>
#include <AudioBuffer>

class AlsaWorker : public QObject
{
    Q_OBJECT

public:
    explicit AlsaWorker(QtJack::Client& client, SlaveProcessor* processor);
    
private slots:
//Q_SLOTS:
    void Work();

private:
    AlsaController* m_dac;
    QtJack::AudioBuffer* m_buffer;
    int64_t* m_alsaBuffer;
    SlaveProcessor *m_processor;
    QMutex m_mutex;
    QtJack::AudioPort in;
  
};


#endif //ALSAWORKER_H