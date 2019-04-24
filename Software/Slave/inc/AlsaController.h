/**
 * @file AlsaController.h
 * @author Jamie Brown
 * @brief 
 * @version 0.1
 * @date 2019-03-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef ALSACONTROLLER_H 
#define ALSACONTROLLER_H

#include <Client>
#include <alsa/asoundlib.h>
#include <string>

/**
 * @brief 
 * 
 */
class AlsaController
{
    public:
        AlsaController(QtJack::Client& client, const char* pcmDevice);
        bool WriteInterleaved(int64_t* buffer, int size);
        uint32_t FramesReady();
        bool Rewind(uint32_t samples);

    private:
        int RecoverXRuns(int status);
        snd_pcm_hw_params_t* m_hwParams;
        snd_pcm_t* m_playbackHandle;
        unsigned int m_periodSize;
        unsigned int m_sampleRate;

};

#endif //ALSACONTROLLER_H
