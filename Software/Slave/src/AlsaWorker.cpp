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
#include <jack/jack.h>

#define MTU 1500 //TODO: is there a way to query this from JACK at runtime?
#define XOVERFREQ 2500 //TODO: what is the crossover frequency for the drivers/cabinet?
#define TAPS 100 //TODO: how mnay can we get away with?
#define PCMDEVICE "hw:0"

using namespace std;

extern QWaitCondition notify;

AlsaWorker::AlsaWorker(QtJack::Client& client, SlaveProcessor* processor)
{  
    m_client = &client;
	m_processor = processor;
    m_buffer = new QtJack::AudioBuffer;
    m_alsaBuffer = new int64_t[m_processor->bufferSize()];
    m_padBuffer = new int64_t[512];
    m_dac = new AlsaController(client, PCMDEVICE);

    double xoverFreq = XOVERFREQ / static_cast<double>(client.sampleRate()); //normalise to nyquist

    firWoof = new FIRFilter(LOWPASS, xoverFreq, TAPS);
    firTweet = new FIRFilter(HIGHPASS, xoverFreq, TAPS); 

    filterConfig_t midConfig;
    midConfig.type = PEAK;
    midConfig.fCut = 1000;
    midConfig.dbGain = 0;
    midConfig.q = 1 / sqrt(2); 
    m_midEQ = new IIRFilter(midConfig, client.sampleRate());

    filterConfig_t bassConfig;
    bassConfig.type = LOWSHELVE;
    bassConfig.fCut = 1000;
    bassConfig.dbGain = 0;
    bassConfig.q = 1 / (sqrt(2));
    m_bassEQ = new IIRFilter(bassConfig, client.sampleRate());

    filterConfig_t trebleConfig;
    trebleConfig.type = HIGHSHELVE;
    trebleConfig.fCut = 1000;
    trebleConfig.dbGain = 0;
    trebleConfig.q = 1 / (sqrt(2)); 
    m_trebleEQ = new IIRFilter(trebleConfig, client.sampleRate());

    filterConfig_t notchConfig;
    notchConfig.type = BANDPASS;
    notchConfig.fCut = 1500; //tweeter resonant frequency
    notchConfig.dbGain = -12;
    notchConfig.q = 1 / (sqrt(2)); 
    m_notchCorrection = new IIRFilter(notchConfig, client.sampleRate());

    m_bufferSize = m_processor->bufferSize();
    for (int i =0; i < (sizeof(m_padBuffer) / sizeof(int64_t)) ; i++)
    {
        m_padBuffer[i] = 0;
    }
}

int AlsaWorker::DriftCorrection()
{
    int32_t deltaK; //raw sample offset between ALSA and JACK
    int32_t nJ; //number of samples elapsed since JACK started the current cycle
    int32_t nA; //number of samples ALSA has written this cycle (16*4096 - samples_ready_to_write)
    int rushTolerance = 0; //number of samples to let ALSA run ahead of JACK before we pull it back
    int dragTolerance = 0; //number of samples to let ALSA lag behind JACK before we push it forward (pad)
    jack_client_t* rawClient; //handle to the raw C API client 

    rawClient = m_client->GetRawClient(); //JB: I have added this method to QtJack::Client to get the handle

    nJ = round(jack_frames_since_cycle_start(rawClient));
    nA = 16*m_bufferSize - m_dac->FramesReady();

    std::cout << "ALSA frames written since cycle start: " << nA << std::endl;
    std::cout << "frames since cycle start: " << nJ << std::endl;

    deltaK =  m_bufferSize - (nA + nJ); //scale up to 32-bit by multiplying number of frames by 4
    std::cout << "Sample offset: " << deltaK << std::endl;

    if (deltaK < rushTolerance)
    {
        /* ALSA is running ahead so force a rewind */
        m_dac->Rewind(-1*deltaK);
    }
    if(deltaK > dragTolerance)
    {
        /* ALSA is lagging behind so pad a temporary buffer and write */
        while (deltaK > 0) 
        {
	        uint64_t to_write = (deltaK > 512) ? 512 : deltaK;
            m_dac->WriteInterleaved(m_padBuffer, static_cast<int>(to_write));
	        deltaK -= to_write;
	    }
    }






    //std::cout << "Buffer size: " << m_bufferSize << std::endl;
} 

void AlsaWorker::Work()
{
    cout << "WORKER: buffer size is " << m_processor->bufferSize() << endl;
    cout << "WORKER: sample-rate is " << m_processor->sampleRate() << endl;

    bool readOkay;
    bool packetDropped = true;
    bool streamRecovered = false;
    int64_t currentSample; //interleaved sample
    int64_t leftSample32 = 0;
    int64_t rightSample32 = 0;
    double inputSample, bassSample, midSample, trebleSample, leftSample, rightSample;
    int posPacket = 0;
    int newBuffSize;

    int samplesPerPacket = MTU / (m_processor->bitDepth() / 8);
    if (samplesPerPacket > 256) { samplesPerPacket = 256; } else { samplesPerPacket = 512; }
    int packetsPerBuffer = m_processor->bufferSize() / samplesPerPacket;

    /* wait until receiver thread is ready */
    m_mutex.lock();
    notify.wait(&m_mutex);
    m_mutex.unlock();

    m_atten = 1;

    while(true)
    {     
        m_mutex.lock();

        /* TODO: only call if this thread catches up to jack thread */
        if(!notify.wait(&m_mutex, 93))
        {
            cout << "ALSA WORKER: thread sync error" << endl;
        }

        /* determine the required output buffer size to syncronise 
           ALSA with the JACK server */
        newBuffSize = DriftCorrection();
        
        //pos = 0; pos < resizedBufferLen: pos++
        for (int pos = 0; pos < m_processor->bufferSize(); pos++)
        {
            /* copy from ring buffer into local ALSA buffer */
            inputSample = m_processor->ringBuffer->front();
            m_processor->ringBuffer->pop_front(); //shift buffer

            /* apply 3-band EQ filters */
            
            if (m_eqEnabled)
            {
                inputSample = m_midEQ->filter(inputSample);
                inputSample = m_bassEQ->filter(inputSample);
                inputSample = m_trebleEQ->filter(inputSample);
            }
            

            /* unfiltered  */
            leftSample = inputSample * m_atten; //woofer
            rightSample = inputSample * m_atten; //tweeter
            //leftSample = inputSample; //woofer
            //rightSample = inputSample; //tweeter

            /* apply crossover filters */
            if (true)
            {
                leftSample = firWoof->filter(inputSample) * 2.5;
                rightSample = firTweet->filter(inputSample) * 2.0;

                /* output to test points */
                //m_FirLowPassBuffer.write(&leftSample, 1)
                //m_FirHighPassBuffer.write(&rightSample, 1)
               
            }
            
            /* interleave each channel into MSB and LSB */

            //leftSample = 0; //woofer
            //rightSample = 0; //tweeter

            /* apply correction filters */
            //rightSample = m_notchCorrection->filter(rightSample);

            leftSample32 = static_cast<int64_t>(leftSample*0x10000000);
            leftSample32 = leftSample32 & 0x00000000FFFFFFFF;
            rightSample32 = static_cast<int64_t>(rightSample*0x10000000);
            rightSample32 = rightSample32 << 32;
            rightSample32 = rightSample32 & 0xFFFFFFFF00000000; //shift right sample to 32 MSB
            currentSample = rightSample32 | leftSample32;                           
            m_alsaBuffer[pos] = currentSample;

            /* any non-zero samples in a potential packet indicate that it was delivered 
                not sure this is the best method... */
            if (inputSample != 0) packetDropped = false;

            /* have we reached the end of a UDP packet? */
            if (posPacket == samplesPerPacket)
            {
                if (packetDropped)
                {
                    /* TODO: interpolate */
                    
                    /* placeholder: repeat previous packet */
                    for (int cnt = 0; cnt < samplesPerPacket; cnt++)
                    {
                        //m_alsaBuffer[cnt + (pos - samplesPerPacket)] = m_alsaBuffer[cnt + (pos - (2 * samplesPerPacket))];
                    }
                    if ( streamRecovered ) cout << "ALSA WORKER: stream lost" << endl;
                    streamRecovered = false; 
                }
                else
                {
                    if ( !streamRecovered ) cout << "ALSA WORKER: stream recovered" << endl;
                    streamRecovered = true;
                }
        
                /* reset for next packet */
                packetDropped = true;
                posPacket = 0;
            }
            else
            {
                posPacket++;
            }

        }

        /* output through alsa */
        
        if ( !m_dac->WriteInterleaved(m_alsaBuffer, m_bufferSize) )
        {
            cout << "ALSA WORKER: failed to write to device" << endl;
            exit(1);
        }
        
        /* route filter outputs to test points */

        //cout << "ALSA WORKER: input sample " << inputSample << endl;
        //cout << "ALSA WORKER: output sample "<< inputSample << endl;
        
        m_mutex.unlock();
        qApp->processEvents();
    }

    
}

void AlsaWorker::Attenuate(float factor)
{
    m_atten = factor;
}

void AlsaWorker::AdjustMid(filterConfig_t* params)
{
    //std::cout << "New Gain " << params->dbGain << std::endl;
    m_midEQ->Update(params->dbGain);
}

void AlsaWorker::AdjustBass(filterConfig_t* params)
{
    m_bassEQ->Update(params->dbGain);
}

void AlsaWorker::AdjustTreble(filterConfig_t* params)
{
    m_trebleEQ->Update(params->dbGain);
}

void AlsaWorker::EnableEq()
{
    m_eqEnabled = true;
}

void AlsaWorker::DisableEq()
{
    m_eqEnabled = false;
}

ThreeBand_t AlsaWorker::EQSettings()
{
    ThreeBand_t eq;
    eq.bass = m_bassEQ->getParams();
    eq.mid = m_midEQ->getParams();
    eq.treble = m_trebleEQ->getParams();

    return eq;
}
