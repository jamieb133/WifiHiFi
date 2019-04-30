/**
 * @file HardwareController.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef HARDWARECONTROLLER_H
#define HARDWARECONTROLLER_H

#include "types.h"

#include <QtCore>
#include <QObject>
#include <server.h>



class HardwareController : public QObject
{
    Q_OBJECT
    public:
        HardwareController();
        ~HardwareController();
        static void VolumeUpISR();
        static void VolumeDownISR();
        static void PowerOffISR();
        static void SetVolume(int vol);

    //private:
        int* m_dbVols;
        int m_vol;
        int m_deltaVol;
    private slots:
        void Work();
        void PowerTimeout();

    private:
        bool m_upFlag;
        bool m_downFlag;
        bool m_powerFlag;
        QTimer* m_timer;
        QtJack::Server* m_server;

    
};

#endif //HARDWARECONTROLLER_H