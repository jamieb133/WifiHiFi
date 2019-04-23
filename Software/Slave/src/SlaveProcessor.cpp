/**
 * @file SlaveProcessor.cpp
 * @author Jamie Brown
 * @brief 
 * @version 0.1
 * @date 2019-03-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <iostream>

#include "SlaveProcessor.h"

#include <QWaitCondition>

//#define BUFFSIZE 16384
#define BUFFSIZE 40960

using namespace std;

extern QWaitCondition notify;

SlaveProcessor::SlaveProcessor(QtJack::Client& client) : Processor(client)
{
    
    m_bufferSize = client.bufferSize();
    m_sampleRate = client.sampleRate();

    in = client.registerAudioInPort("in");
    ringBuffer = new boost::circular_buffer<double>(BUFFSIZE);

    /* register test points */
    /*
    CrossoverLP_out = client.registerAudioOutPort("WifiHifiCrossoverLP_test");
    CrossoverHP_out = client.registerAudioOutPort("WifiHifiCrossoverHP_test");
    EqBass_out = client.registerAudioOutPort("WifiHifiEqBass_test");
    EqMid_out = client.registerAudioOutPort("WifiHifiEqMid_test");
    EqTreble_out = client.registerAudioOutPort("WifiHifiEqTreble_test");
    TweeterNotch_out = client.registerAudioOutPort("WifiHifiTweeterNotch_test");
    WooferShelf_out = client.registerAudioOutPort("WifiHifiWooferShelf_test");
    */

}

void SlaveProcessor::process(int samples)
{
    int64_t currentSample;
    //cout << "write" << endl;
    for (int pos = 0 ; pos < samples; pos++)
    {
        /* copy sample from input buffer to ring buffer */
        m_inputSample = in.buffer(samples).read(pos, &readOkay);
        ringBuffer->push_back(m_inputSample);
    }
    //cout << "PROCESSOR: " << m_inputSample << endl;
    

    /* notify the ALSA thread that new buffer is ready */
    notify.wakeAll();

}

int SlaveProcessor::bufferSize() const
{
    return m_bufferSize;
}

int SlaveProcessor::sampleRate() const
{
    return m_sampleRate;
}

int SlaveProcessor::bitDepth() const
{
    return sizeof(m_inputSample);
}