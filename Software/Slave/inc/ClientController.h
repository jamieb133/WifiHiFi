/**
 * @file ClientController.h
 * @author Jamie Brown
 * @brief 
 * @version 0.1
 * @date 2019-03-26
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include "AlsaWorker.h"

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QUdpSocket>
#include <QPointer>

class ClientController : public QObject
{
    Q_OBJECT

public:
    explicit ClientController(AlsaWorker* alsa);

public slots:
    /**
     * @brief 
     * 
     */
    void readyRead();

private slots:
    /**
     * @brief 
     * 
     */
    void Start();

signals:
    /**
     * @brief 
     * 
     * @param factor 
     */
    void VolRequest(float factor);

    /**
     * @brief 
     * 
     * @param fCut 
     */
    void BassRequest(int fCut);

    /**
     * @brief 
     * 
     * @param fCut 
     */
    void MidRequest(int fCut);

    /**
     * @brief 
     * 
     * @param fCut 
     */
    void TrebleRequest(int fCut);

private:
    /**
     * @brief 
     * 
     */
    QPointer<QUdpSocket> m_socket;

    /**
     * @brief 
     * 
     */
    AlsaWorker* m_alsa;

    bool eqOn = false;

    void HardwareVolume(int vol);

    int* m_dbVols;

  
};


#endif //CLIENTCONTROLLER_H