/**
 * @file AlsaController.cpp
 * @author Jamie Brown
 * @brief 
 * @version 0.1
 * @date 2019-03-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "AlsaController.h"

#include <alsa/asoundlib.h>
#include <iostream>

using namespace std;

/**
 * @brief Contructor: initialises hardware device paramaters for PCM audio output
 *
 * 
 * @param client A reference to the JACK client using this device
 */
AlsaController::AlsaController(QtJack::Client& client)
{
    int status; 
    m_periodSize = client.bufferSize();
    m_sampleRate = client.sampleRate();

    if ((status = snd_pcm_open (&m_playbackHandle, "hw:0", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        cout << "ALSA: cannot open audio device " << snd_strerror (status) << endl;
        exit (1);
    }
        
    if ((status = snd_pcm_hw_params_malloc (&m_hwParams)) < 0) {
        cout << "ALSA: cannot allocate hardware parameter structure " << snd_strerror (status) << endl;
        exit (1);
    }
                
    if ((status = snd_pcm_hw_params_any (m_playbackHandle, m_hwParams)) < 0) {
        cout << "ALSA: cannot initialize hardware parameter structure " << snd_strerror (status) << endl;
        exit (1);
    }

    if ((status = snd_pcm_hw_params_set_access (m_playbackHandle, m_hwParams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        cout << "ALSA: cannot set access type " << snd_strerror (status) << endl;
        exit (1);
    }

    if ((status = snd_pcm_hw_params_set_format (m_playbackHandle, m_hwParams, SND_PCM_FORMAT_S32_LE)) < 0) {
        cout << "ALSA: cannot set sample format " << snd_strerror (status) << endl;
        exit (1);
    }

    if ((status = snd_pcm_hw_params_set_rate_near (m_playbackHandle, m_hwParams, &m_sampleRate, 0)) < 0) {
        cout << "ALSA: cannot set sample rate " << snd_strerror (status) << endl;
        exit (1);
    }

    if ((status = snd_pcm_hw_params_set_channels (m_playbackHandle, m_hwParams, 2)) < 0) {
        cout << "ALSA: cannot set channel count " << snd_strerror (status) << endl;
        exit (1);
    }

    if ((status = snd_pcm_hw_params (m_playbackHandle, m_hwParams)) < 0) {
        cout << "ALSA: cannot set parameters " << snd_strerror (status) << endl;
        exit (1);
    }

    snd_pcm_hw_params_free (m_hwParams);

    if ((status = snd_pcm_prepare (m_playbackHandle)) < 0) {
        cout << "ALSA: cannot prepare audio interface for use " << snd_strerror (status) << endl;
        exit (1);
    }
}

/**
 * @brief Writes the interleaved contents of a buffer to the PCM device
 * 
 * @param buffer 
 * @return true 
 * @return false 
 */
bool AlsaController::WriteInterleaved(int64_t* buffer)
{
    int status;
    while(1)
    {
        if ( (status = snd_pcm_writei(m_playbackHandle, buffer, m_periodSize)) < 0)
        {
            /* initial write failed */
            if ( (status = RecoverXRuns(status)) < 0)
            {
                cout << "ALSA: failed to write to sound card, consider changing the buffer period" << endl;
                return false;
            }
        }
        else
        {
            /* write was successful */
            cout << "ALSA: done" << endl;
            return true;
        }
        
    }
}

/**
 * @brief Resets the device when a buffer underrun or overrun occurs 
 * 
 */
int AlsaController::RecoverXRuns(int status)
{
    /* copied this function from the alsa_out example client (https://github.com/jackaudio/jack2/blob/develop/example-clients/alsa_out.c) */
    if (status == -EPIPE) 
    {	/* under-run */
		status = snd_pcm_prepare(m_playbackHandle);
		if (status < 0)
			cout << "ALSA: can't recovery from underrun, prepare failed: " << snd_strerror(status) << endl;
		return 0;
	} 
    else if (status == -ESTRPIPE) 
    {
        /* wait until the suspend flag is released */
		while ((status = snd_pcm_resume(m_playbackHandle)) == -EAGAIN) { usleep(100); }	
		if (status < 0) 
        {
			status = snd_pcm_prepare(m_playbackHandle);
			if (status < 0)
            {
                cout << "ALSA: can't recover from suspend, prepare failed: " << snd_strerror(status) << endl;
            }
		}
		return 0;
	}
	return status;
}