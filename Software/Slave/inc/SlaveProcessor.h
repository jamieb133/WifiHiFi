#ifndef SLAVEPROCESSOR_H
#define SLAVEPROCESSOR_H

#include "CrossoverFilter.h"

#include <Client>
#include <Processor>
#include <RingBuffer>
#include <Fir1.h>

class SlaveProcessor : public QtJack::Processor 
{
public:
    SlaveProcessor(QtJack::Client& client);
    void process(int samples);
private:
    Fir1* fir;
    CrossoverFilter* firWoof;
    CrossoverFilter* firTweet;
    double currentSample, inputSample;

    bool readOkay;
    int buffNum;
    float* m_shiftBuffer;
    float* m_bigBuff;
    bool startupFlag;
    QtJack::AudioPort in;
    QtJack::AudioPort tweeterOut;
    QtJack::AudioPort wooferOut;
    QtJack::AudioRingBuffer ringBuffer;

};

#endif //SLAVEPROCESSOR_H