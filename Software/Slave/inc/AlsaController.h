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

#include <alsa/asoundlib.h>

/**
 * @brief 
 * 
 */
class AlsaController
{
    public:
        AlsaController(QtJack::Client& client);
        bool WriteInterleaved(int64_t* buffer);

    private:
        int RecoverXRuns(int status);
        snd_pcm_hw_params_t* m_hwParams;
        snd_pcm_t* m_playbackHandle;

}