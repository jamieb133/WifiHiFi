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

#include "FIRFilter.h"
#include "IIRFilter.h"
#include "AlsaController.h"
#include "SlaveProcessor.h"

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QUdpSocket>

#include <RingBuffer>
#include <AudioBuffer>
#include <Fir1.h>

class AlsaWorker : public QObject
{
    Q_OBJECT

public:
    explicit AlsaWorker(QtJack::Client& client, SlaveProcessor* processor);

    ThreeBand_t EQSettings();
    void AdjustMid(filterConfig_t* params);
    void AdjustBass(filterConfig_t* params);
    void AdjustTreble(filterConfig_t* params);
    void EnableEq();
    void DisableEq();

public slots:
    void Attenuate(float factor);

private slots:
//Q_SLOTS:
    void Work();

private:
    /**
     * @brief Calculates the required buffer resizing factor
     * 
     * @return int Number of samples to allocate for new outbound buffer
     */
    int DriftCorrection();

    Fir1* fir;
    FIRFilter* firWoof;
    FIRFilter* firTweet;

    IIRFilter* m_midEQ;
    IIRFilter* m_trebleEQ;
    IIRFilter* m_bassEQ;
    IIRFilter* m_notchCorrection;

    AlsaController* m_dac;
    QtJack::AudioBuffer* m_buffer;
    int64_t* m_alsaBuffer;
    int64_t* m_padBuffer;
    SlaveProcessor *m_processor;
    QMutex m_mutex;
    QtJack::AudioPort in;

    QtJack::AudioRingBuffer m_FirLowPassBuffer;
    QtJack::AudioRingBuffer m_FirHighPassBuffer;

    QtJack::Client* m_client;

    int m_bufferSize;
    

    

    /**
     * @brief attenuation factor 
     * 
     */
    float m_atten = 1.0;

    bool m_eqEnabled = true;

  
};


#endif //ALSAWORKER_H