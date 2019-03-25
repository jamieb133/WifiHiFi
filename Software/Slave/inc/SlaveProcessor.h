#ifndef SLAVEPROCESSOR_H
#define SLAVEPROCESSOR_H

#include "CrossoverFilter.h"
#include "AlsaController.h"

#include <QMutex>
#include <Client>
#include <Processor>
#include <RingBuffer>
#include <boost/circular_buffer.hpp>
#include <Fir1.h>

class SlaveProcessor : public QtJack::Processor 
{
public:
    SlaveProcessor(QtJack::Client& client);
    void process(int samples);
    //QtJack::AudioRingBuffer ringBuffer;
    boost::circular_buffer<double>* ringBuffer;

    bool lostPacket() const;
    int bufferSize() const;
    int sampleRate() const;
private:
    Fir1* fir;
    CrossoverFilter* firWoof;
    CrossoverFilter* firTweet;

    int64_t* m_alsaBuffer;
    AlsaController* m_dac;
    int m_bufferSize;
    int m_sampleRate;

    QtJack::AudioPort in;

    bool m_lostPacket;
    bool readOkay;

    QMutex m_mutex;
    
};

#endif //SLAVEPROCESSOR_H