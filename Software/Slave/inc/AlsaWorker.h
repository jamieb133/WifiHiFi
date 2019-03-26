/**
 * @file AlsaWorker.h
 * @author Jamie Brown
 * @brief 
 * @version 0.1
 * @date 2019-03-26
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef ALSAWORKER_H
#define ALSAWORKER_H

#include "CrossoverFilter.h"
#include "AlsaController.h"
#include "SlaveProcessor.h"

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <RingBuffer>
#include <AudioBuffer>
#include <Fir1.h>

class AlsaWorker : public QObject
{
    Q_OBJECT

public:
    explicit AlsaWorker(QtJack::Client& client, SlaveProcessor* processor);
    
private slots:
//Q_SLOTS:
    void Work();

private:
    Fir1* fir;
    CrossoverFilter* firWoof;
    CrossoverFilter* firTweet;

    AlsaController* m_dac;
    QtJack::AudioBuffer* m_buffer;
    int64_t* m_alsaBuffer;
    SlaveProcessor *m_processor;
    QMutex m_mutex;
    QtJack::AudioPort in;
  
};


#endif //ALSAWORKER_H