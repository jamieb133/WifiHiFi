/**
 * @file LocalThread.cpp
 * @author Jamie Brown
 * @brief 
 * @version 0.1
 * @date 2019-02-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "LocalThread.h"

#include <QtCore>
#include <iostream>
#include <unistd.h>

using namespace std;

/**
 * @brief Construct a new Sensor Thread:: Sensor Thread object
 * 
 */
LocalThread::LocalThread()
{  
	
}

/**
 * @brief 
 * 
 */
void LocalThread::run()
{
    cout << "LOCAL THREAD: starting sensor thread..." << endl;

    /* kick off the thread */
    exec();
}