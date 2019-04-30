/**
 * @file HardwareController.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "HardwareController.h"

#include <iostream>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <math.h>
#include <unistd.h>
#include <QTimer>
//#include <assert>

#define VOLUMEUP 6
#define VOLUMEDOWN 10
#define POWEROFF 11

HardwareController::HardwareController()
{
    //m_server = server;

    SetVolume(0);
    m_deltaVol = 0;
    if(wiringPiSetup() < 0) 
    {
        std::cout << "ERROR INITIALISING WIRING PI" << std::endl;
        exit(1);
    }
    
    if(wiringPiISR(VOLUMEUP, INT_EDGE_RISING, &VolumeUpISR) < 0) 
    { 
        std::cout << "ERROR SETTING UP VOLUME UP ISR" << std::endl;
        exit(1);
    }
    if(wiringPiISR(VOLUMEDOWN, INT_EDGE_RISING, &VolumeDownISR) < 0) 
    { 
        std::cout << "ERROR SETTING UP VOLUME DOWN ISR" << std::endl;
        exit(1);
    }
    if(wiringPiISR(POWEROFF, INT_EDGE_RISING, &PowerOffISR) < 0) 
    { 
        std::cout << "ERROR SETTING UP POWER OFF ISR" << std::endl;
        exit(1);
    }

    /* set button pins an inputs */
    pinMode(VOLUMEDOWN, INPUT);
    pinMode(VOLUMEUP, INPUT);
    pinMode(POWEROFF, INPUT);

    /* disable the internal pull ups */
    //pullUpDnControl(VOLUMEDOWN, PUD_DOWN);
    //pullUpDnControl(VOLUMEUP, PUD_DOWN);
    //pullUpDnControl(POWEROFF, PUD_DOWN);

    /* logarithmic volume */
    m_dbVols = new int[100];
    for (int count = 1; count <= 100; count++)
    {
        m_dbVols[count-1] = 30*log10(static_cast<float>(count)); //scale from 0 to 60 in linear gain
        std::cout << m_dbVols[count-1] << std::endl;
    }

    m_upFlag = true;
    m_downFlag = true;
    m_powerFlag = true;

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(PowerTimeout()));

}

HardwareController::~HardwareController()
{
    

}

void HardwareController::Work()
{
    std::cout << "HARDWARE CONTROLLER STARTED" << std::endl;
    while(1)
    {
        //std::cout << "HARDWARE CONTROLLER STARTED" << std::endl;
        if (digitalRead(VOLUMEDOWN) == 0 && m_downFlag )
        {
            m_downFlag = false;
            std::cout << "VOLUME DOWN" << std::endl;
            usleep(100000);
        } 
        else if (digitalRead(VOLUMEDOWN) == 1)
        {
            m_downFlag = true;
        }


        if ( (digitalRead(VOLUMEUP) == 0) && m_upFlag)
        {
            m_upFlag = false;
            std::cout << "VOLUME UP" << std::endl;
            usleep(100000);
        } 
        else if (digitalRead(VOLUMEUP) == 1)
        {
            m_upFlag = true;
        }
        

        if ( (digitalRead(POWEROFF) == 0) && m_powerFlag)
        {
            m_powerFlag = false;
            std::cout << "POWER REQUEST" << std::endl;
            m_timer->start(3000);
        } 
        else if (digitalRead(POWEROFF) == 1)
        {
            m_powerFlag = true;
        }
        qApp->processEvents();
    }
    
}

void HardwareController::PowerTimeout()
{
    std::cout << "POWER TIMEOUT" << std::endl;
    if ( (digitalRead(POWEROFF) == 0) && !m_powerFlag)
    {
        std::cout << "POWER OFF!!!" << std::endl;
        //m_server->stop();
        exit(10);
    }
    m_timer->stop();
       
}

void HardwareController::VolumeUpISR()
{
    //if (m_vol < 98) m_deltaVol += 2;
    //std::cout << "VOLUME UP: " << m_deltaVol << std::endl;
}

void HardwareController::VolumeDownISR()
{
    //if (m_vol > 1) m_deltaVol -= 2;
    //std::cout << "VOLUME DOWN: " << m_deltaVol << std::endl;
}

void HardwareController::PowerOffISR()
{
   // std::cout << "POWER OFF" << std::endl;
    
}

void HardwareController::SetVolume(int vol)
{
    /*
    int fd;
    vol += m_deltaVol;
    m_vol = vol;
    if (vol <= 0) vol = 1;
    if (vol >= 100)  vol = 100;
    //assert( (vol >= 0) && (vol <= 100));
    
    fd = wiringPiI2CSetup(0x4B);

    if (int err = wiringPiI2CWrite(fd, m_dbVols[vol]) < 0)
    //if (int err = wiringPiI2CWrite(fd, 0) < 0)
    {
        std::cout << "HARDWARE CONTROLLER: failed to write hardware volume" << std::endl;
    }
    

    std::cout << "Volume request DB" << m_dbVols[vol] << std::endl;
    std::cout << "Volume request linear" << vol << std::endl;
    */


}

