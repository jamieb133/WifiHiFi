/**
 * @file types.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef TYPES_H
#define TYPES_H

namespace WifiHifi
{
    /**
     * @brief 
     * 
     */
    typedef enum {
        LOWPASS,
        BANDPASS,
        HIGHPASS,
        PEAK,
        LOWSHELVE,
        HIGHSHELVE,
        ALLPASS
    } filter_t;

    /**
     * @brief 
     * 
     */
    typedef struct filterConfig {
        filter_t type;  //filter type
        double fCut;    //cutoff frequency
        double dbGain;    //gain in db
        double q;       //quality factor
        double atten;
    }filterConfig_t;

    /**
     * @brief 
     * 
     */
    typedef struct ThreeBand {
        filterConfig_t bass;
        filterConfig_t mid;
        filterConfig_t treble;
    } ThreeBand_t;

    /**
     * @brief 
     * 
     */
    typedef struct IIRCoeffs {
        double a0;
        double a1;
        double a2;
        double b0;
        double b1;
        double b2;
    } IIRCoeffs_t;

    typedef struct SwitchFlag {
        bool volUp;
        bool volDown;
        bool power;
    } SwitchFlag_t;

};
#endif //TYPES_H