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

#define BUFFSIZE 4096

using namespace std;

SlaveProcessor::SlaveProcessor(QtJack::Client& client) : Processor(client)
{
    int taps = 100;

    /* scale down frequencies for filtering */
    double cutoff = 10.0;
    double xoverFreq = cutoff / static_cast<double>(client.sampleRate()); //Hz
    cout << "Crossover frequency: " << cutoff << "Hz " << "(" << xoverFreq << ")" << endl;
    cout << "Sample rate: " << client.sampleRate() << "Hz"<< endl;

    firWoof = new CrossoverFilter(LOWPASS, xoverFreq, taps);
    firTweet = new CrossoverFilter(HIGHPASS, xoverFreq, taps);

    in = client.registerAudioInPort("in");
    wooferOut = client.registerAudioOutPort("woof");
    tweeterOut = client.registerAudioOutPort("tweet");

    ringBuffer = QtJack::AudioRingBuffer(BUFFSIZE);

    m_dac = new AlsaController(client);
    m_alsaBuffer = new int64_t[client.bufferSize()];

}

void SlaveProcessor::process(int samples)
{
    for (int pos = 0 ; pos < samples; pos++)
    {
        /* filter the current sample and copy to the relevant output buffer */
        inputSample = in.buffer(samples).read(pos, &readOkay);

        //currentSample = firWoof->filter(inputSample);
        //wooferOut.buffer(samples).write(pos, currentSample);

        //currentSample = firTweet->filter(inputSample);
        //tweeterOut.buffer(samples).write(pos, currentSample);

        /* copy current sample to local ALSA buffer */
        /* TODO: move this to seperate thread that reads from big ring buffer */
        currentSample = static_cast<int64_t>(inputSample*0x10000000);
        //currentSample = currentSample << 32;

        m_alsaBuffer[pos] = currentSample;

    }
    cout << "INPUT: " << inputSample << endl;
    cout << "OUTPUT: "<< currentSample << endl;

    /* output through alsa */
    m_dac->WriteInterleaved(m_alsaBuffer);

}