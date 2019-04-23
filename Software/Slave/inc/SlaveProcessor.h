/**
 * @file SlaveProcessor.h
 * @author Jamie Brown
 * @brief 
 * @version 0.1
 * @date 2019-03-26
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef SLAVEPROCESSOR_H
#define SLAVEPROCESSOR_H

#include "AlsaController.h"

#include <QMutex>
#include <Client>
#include <Processor>
#include <RingBuffer>
#include <boost/circular_buffer.hpp>


class SlaveProcessor : public QtJack::Processor 
{
public:
    SlaveProcessor(QtJack::Client& client);
    void process(int samples);
    //QtJack::AudioRingBuffer ringBuffer;
    boost::circular_buffer<double>* ringBuffer;

    int bufferSize() const;
    int sampleRate() const;
    int bitDepth() const;

    /* filter test points */
    QtJack::AudioPort CrossoverLP_out;
    QtJack::AudioPort CrossoverHP_out;
    QtJack::AudioPort EqBass_out;
    QtJack::AudioPort EqMid_out;
    QtJack::AudioPort EqTreble_out;
    QtJack::AudioPort TweeterNotch_out;
    QtJack::AudioPort WooferShelf_out;
private:
    

    int64_t* m_alsaBuffer;
    AlsaController* m_dac;
    int m_bufferSize;
    int m_sampleRate;
    double m_inputSample;

    QtJack::AudioPort in;

    
    
    bool m_lostPacket;
    bool readOkay;

    QMutex m_mutex;
    
};

#endif //SLAVEPROCESSOR_H