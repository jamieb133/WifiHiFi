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


#define BIGBUFFSIZE 8192
#define BUFFSIZE 2048

using namespace std;

const double exampleLowpassImpulse[] = {
	-0.00105964, -0.0011159 , -0.00122684, -0.00136109, -0.00146365,
	-0.00145803, -0.00125073, -0.00073776,  0.00018711,  0.00162305,
        0.00365256,  0.0063323 ,  0.00968525,  0.01369476,  0.01830123,
        0.02340178,  0.02885309,  0.03447726,  0.04007059,  0.04541448,
        0.05028793,  0.05448067,  0.05780599,  0.0601124 ,  0.06129316,
        0.06129316,  0.0601124 ,  0.05780599,  0.05448067,  0.05028793,
        0.04541448,  0.04007059,  0.03447726,  0.02885309,  0.02340178,
        0.01830123,  0.01369476,  0.00968525,  0.0063323 ,  0.00365256,
        0.00162305,  0.00018711, -0.00073776, -0.00125073, -0.00145803,
	-0.00146365, -0.00136109, -0.00122684, -0.0011159 , -0.00105964
};

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
    startupFlag = true;

}

void SlaveProcessor::process(int samples)
{
    cout << currentSample << endl;
    for (int pos = 0 ; pos < samples; pos++)
    {
        /* filter the current sample and copy to the relevant output buffer */
        inputSample = in.buffer(samples).read(pos, &readOkay);

        currentSample = firWoof->filter(inputSample);
        wooferOut.buffer(samples).write(pos, currentSample);

        currentSample = firTweet->filter(inputSample);
        tweeterOut.buffer(samples).write(pos, currentSample);

    }
    

}