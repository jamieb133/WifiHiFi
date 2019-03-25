/**
 * @file AlsaWorker.cpp
 * @author Jamie Brown
 * @brief 
 * @version 0.1
 * @date 2019-02-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "AlsaWorker.h"

#include <QtCore>
#include <iostream>
#include <unistd.h>

using namespace std;

extern QWaitCondition notify;

AlsaWorker::AlsaWorker(QtJack::Client& client, SlaveProcessor* processor)
{  
	m_processor = processor;
    m_buffer = new QtJack::AudioBuffer;
    m_alsaBuffer = new int64_t[m_processor->bufferSize()];
    m_dac = new AlsaController(client);
}

void AlsaWorker::Work()
{
    cout << "WORKER: started" << endl;
    bool readOkay;
    int64_t currentSample;
    double inputSample;

    /* wait until receiver thread is ready */
    m_mutex.lock();
    notify.wait(&m_mutex);
    m_mutex.unlock();

    while(true)
    {     
        m_mutex.lock();
        notify.wait(&m_mutex);
        cout << "buffer size: " << m_processor->bufferSize() << endl;
        //cout << "audio buffer size: " << m_buffer.size() << endl;
        

        
        
        //if (inputSample != 0)  m_lostPacket = false;
        
        for (int pos = 0; pos < m_processor->bufferSize(); pos++)
        {
            /* copy from ring buffer into local ALSA buffer */
            inputSample = m_processor->ringBuffer->front();
            m_processor->ringBuffer->pop_front(); //shift buffer
            currentSample = static_cast<int64_t>(inputSample*0x10000000);
            m_alsaBuffer[pos] = currentSample;
        }

        /* output through alsa */
        if ( !m_dac->WriteInterleaved(m_alsaBuffer) )
        {
            cout << "ALSA WORKER: failed to write to device" << endl;
            exit(1);
        }


        cout << "ALSA WORKER: input sample "<< inputSample << endl;
        //cout << "ALSA WORKER: output sample "<< currentSample << endl;
        


        m_mutex.unlock();
        qApp->processEvents();
    }

    
}
