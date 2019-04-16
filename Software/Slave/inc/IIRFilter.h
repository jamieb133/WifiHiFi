/**
 * @file IIRFilter.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-09
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef IIRFILTER_H
#define IIRFILTER_H

#include "types.h"

using namespace WifiHifi;

class IIRFilter 
{
public:
    /**
     * @brief Construct a new IIRFilter object
     * 
     * @param params 
     * @param sampleRate 
     */
    IIRFilter(filterConfig_t params, int sampleRate);

    /**
     * @brief Construct a new IIRFilter object
     * 
     * @param coeffs 
     * @param sampleRate 
     */
    IIRFilter(IIRCoeffs_t coeffs, int sampleRate);

    /**
     * @brief Destroy the IIRFilter object
     * 
     */
    ~IIRFilter();

    /**
     * @brief 
     * 
     * @param sample 
     * @return double 
     */
    double filter(double sample);

    /**
     * @brief 
     * 
     * @param freq 
     */
    void Update(filterConfig_t params);

    /**
     * @brief 
     * 
     * @return filterConfig_t 
     */
    filterConfig_t getParams();

protected:
    /**
     * @brief 
     * 
     */
    void CalcLowpass(filterConfig_t* params);

    /**
     * @brief 
     * 
     */
    void CalcNotch(filterConfig_t* params);

    /**
     * @brief 
     * 
     */
    void CalcHighpass(filterConfig_t* params);

    /**
     * @brief 
     * 
     * @param params 
     */
    void CalcLowShelve(filterConfig_t* params);

    /**
     * @brief 
     * 
     * @param params 
     */
    void CalcHighShelve(filterConfig_t* params);

    /**
     * @brief 
     * 
     * @param params 
     */
    void CalcPeak(filterConfig_t* params);

    /**
     * @brief biquad coefficients
     * 
     */
    double m_a0, m_a1, m_a2, m_b0, m_b1, m_b2;

    /**
     * @brief sample rate
     * 
     */
    int m_sampleRate;

    /**
     * @brief 
     * 
     */
    filterConfig_t* m_params;

    /**
     * @brief 
     * 
     */
    double m_outputs[2]; 
    double m_inputs[2];
};

#endif //IIRFILTER_H