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

#define BUFFSIZE 16384

using namespace std;

extern QWaitCondition notify;

SlaveProcessor::SlaveProcessor(QtJack::Client& client) : Processor(client)
{
    int taps = 100;

    /* scale down frequencies for filtering */
    double cutoff = 10.0;
    double xoverFreq = cutoff / static_cast<double>(client.sampleRate()); //Hz
    cout << "Crossover frequency: " << cutoff << "Hz " << "(" << xoverFreq << ")" << endl;
    cout << "Sample rate: " << client.sampleRate() << "Hz"<< endl;

    m_bufferSize = client.bufferSize();
    m_sampleRate = client.sampleRate();

    firWoof = new CrossoverFilter(LOWPASS, xoverFreq, taps);
    firTweet = new CrossoverFilter(HIGHPASS, xoverFreq, taps);

    in = client.registerAudioInPort("in");

    ringBuffer = new boost::circular_buffer<double>(BUFFSIZE);

    //m_dac = new AlsaController(client);
    m_alsaBuffer = new int64_t[client.bufferSize()];

}

void SlaveProcessor::process(int samples)
{
    //m_mutex.lock();
    //cout << "SLAVE PROCESS" << endl;

    double inputSample;
    int64_t currentSample;
    m_lostPacket = true;
    
    for (int pos = 0 ; pos < samples; pos++)
    {
        /* copy sample from input buffer to ring buffer */
        inputSample = in.buffer(samples).read(pos, &readOkay);
        ringBuffer->push_back(inputSample);
    }
    cout << "PROCESSOR: " << inputSample << endl;
    

    /* notify the ALSA thread that new buffer is ready */
    notify.wakeAll();
    //m_mutex.unlock();

}

bool SlaveProcessor::lostPacket() const
{
    //m_mutex.lock();
    return m_lostPacket;
    //m_mutex.unlock();
}

int SlaveProcessor::bufferSize() const
{
    return m_bufferSize;
}

int SlaveProcessor::sampleRate() const
{
    return m_sampleRate;
}