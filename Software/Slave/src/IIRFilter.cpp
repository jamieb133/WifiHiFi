/**
 * @file IIRFilter.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-09
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "types.h"
#include "IIRFilter.h"
#include <math.h>
#include <iostream>

//#define M_PI 3.14159

using namespace WifiHifi;

IIRFilter::IIRFilter(filterConfig_t params, int sampleRate)
{
    /*
    m_outputs = new double[2];
    m_inputs = new double[2];
    */

    /* just in case... */
    m_a0 = 0;
    m_a1 = 0;
    m_a2 = 0;
    m_b0 = 0;
    m_b1 = 0;
    m_b2 = 0;
    m_outputs[0] = 0.0;
    m_outputs[1] = 0.0;
    m_inputs[0] = 0.0;
    m_inputs[1] = 0.0;

    m_sampleRate = sampleRate;
    m_params = new filterConfig_t;
    *m_params = params;
    Update(params);

   
}

IIRFilter::IIRFilter(IIRCoeffs_t coeffs, int sampleRate)
{
    m_a0 = coeffs.a0;
    m_a1 = coeffs.a1;
    m_a2 = coeffs.a2;
    m_b0 = coeffs.b0;
    m_b1 = coeffs.b1;
    m_b2 = coeffs.b2;
}

IIRFilter::~IIRFilter()
{
    
}

double IIRFilter::filter(double sample)
{
    double accum;
    
    /* calculate feed foward */
    accum = (m_b0 * sample) + (m_b1 * m_inputs[0]) + (m_b2 * m_inputs[1]);
    /* calculate feedback */
    accum += -( (m_a1 * m_outputs[0]) + (m_a2 * m_outputs[1]) );
    /* Bristow-Johnson extra coefficient (a0) */
    accum = accum / m_a0;
    //accum = accum * m_params->atten;
    

    /* shift buffers */
    m_inputs[1] = m_inputs[0];
    m_inputs[0] = sample;
    m_outputs[1] = m_outputs[0];
    m_outputs[0] = accum;

    return accum;
}

void IIRFilter::Update(filterConfig_t params)
{
    std::cout << "Generating biquad coeefs for type " << params.type << std::endl;
    *m_params = params;
    switch(m_params->type)
    {
        case LOWPASS:       CalcLowpass(m_params); break;
        case BANDPASS:      CalcNotch(m_params); break;
        case HIGHPASS:      CalcHighpass(m_params); break;
        case PEAK:          CalcPeak(m_params); break;
        case LOWSHELVE:     CalcLowShelve(m_params); break;
        case HIGHSHELVE:    CalcHighShelve(m_params); break;
        default:            throw "ERROR: not a valid filter type";
    }
}

void IIRFilter::CalcLowpass(filterConfig_t* params)
{
    //std::cout << "calc" << std::endl;
    double A, omega, alpha;

    A = pow(10, params->dbGain/(40.0*params->fCut));
    omega = (2 * M_PI * params->fCut) / m_sampleRate;
    alpha = sin(omega) / (2.0 * params->q);

    /* adjust coefficients */
    m_a0 = (1 + alpha) ;
    m_a1 = -2 * cos(omega);
    m_a2 = 1 - alpha;
    m_b0 = (1 - cos(omega)) / 2;
    m_b1 = 1 - cos(omega);
    m_b2 = m_b0;


}

void IIRFilter::CalcNotch(filterConfig_t* params)
{
    double A, omega, alpha;

    A = pow(10, params->dbGain/(40.0*params->fCut));
    omega = (2 * M_PI * params->fCut) / m_sampleRate;
    alpha = sin(omega) / (2.0 * params->q);

    /* adjust coefficients */
    m_a0 = (1 + alpha);
    m_a1 = -2 * cos(omega);
    m_a2 = 1 - alpha;
    m_b0 = params->q * alpha;
    m_b1 = 0;
    m_b2 = -params->q * alpha;
}

void IIRFilter::CalcHighpass(filterConfig_t* params)
{
    double A, omega, alpha;

    A = pow(10, params->dbGain/(40.0*params->fCut));
    omega = (2 * M_PI * params->fCut) / m_sampleRate;
    alpha = sin(omega) / (2.0 * params->q);

    /* adjust coefficients */
    //m_a0 = 1 + (alpha + A);
    m_a0 = (1 + alpha);
    m_a1 = -2 * cos(omega);
    m_a2 = 1 - alpha;
    m_b0 = (1 + cos(omega)) / 2;
    m_b1 = -(1 + cos(omega));
    m_b2 = m_b0;

}

void IIRFilter::CalcLowShelve(filterConfig_t* params)
{
    double A, omega, alpha, beta;

    A = pow(10.0, params->dbGain/40.0);
    omega = (2 * M_PI * params->fCut) / m_sampleRate;
    alpha = sin(omega) / (2.0 * params->q);
    beta = sqrt(2*A);

    m_b0 = A * ((A + 1) - (A - 1) * cos(omega) + beta * sin(omega));
    m_b1 = 2 * A * ((A - 1) - (A + 1) * cos(omega));
    m_b2 = A * ((A + 1) - (A - 1) * cos(omega) - beta * sin(omega));
    m_a0 = (A + 1) + (A - 1) * cos(omega) + beta * sin(omega);
    m_a1 = -2 * ((A - 1) + (A + 1) * cos(omega));
    m_a2 = (A + 1) + (A - 1) * cos(omega) - beta * sin(omega);

    std::cout << "Low shelve with coeffs: " 
        << "b0=" << m_b0 << ", b1=" << m_b1 << ", b2="  << m_b2 
        << ", a0=" << m_a0 << ", a1=" << m_a1 << ", a2=" << m_a2 
        << std::endl;
}

void IIRFilter::CalcHighShelve(filterConfig_t* params)
{
    double A, omega, alpha, beta;

    A = pow(10.0, params->dbGain/40.0);
    omega = (2 * M_PI * params->fCut) / m_sampleRate;
    alpha = sin(omega) / (2.0 * params->q);
    beta = sqrt(2*A);

    m_b0 = A * ((A + 1) + (A - 1) * cos(omega) + beta * sin(omega));
    m_b1 = -2 * A * ((A - 1) + (A + 1) * cos(omega));
    m_b2 = A * ((A + 1) + (A - 1) * cos(omega) - beta * sin(omega));
    m_a0 = (A + 1) - (A - 1) * cos(omega) + beta * sin(omega);
    m_a1 = 2 * ((A - 1) - (A + 1) * cos(omega));
    m_a2 = (A + 1) - (A - 1) * cos(omega) - beta * sin(omega);

    std::cout << "High shelve with coeffs: " 
        << "b0=" << m_b0 << ", b1=" << m_b1 << ", b2="  << m_b2 
        << ", a0=" << m_a0 << ", a1=" << m_a1 << ", a2=" << m_a2 
        << std::endl;
}

void IIRFilter::CalcPeak(filterConfig_t* params)
{
    double A, omega, alpha;

    A = pow(10.0, params->dbGain/40.0);
    omega = (2 * M_PI * params->fCut) / m_sampleRate;
    alpha = sin(omega) / (2.0 * params->q);

    m_b0 = 1 + (alpha * A);
    m_b1 = -2 * cos(omega);
    m_b2 = 1 - (alpha * A);
    m_a0 = 1 + (alpha / A);
    m_a1 = -2 * cos(omega);
    m_a2 = 1 - (alpha / A);

    std::cout << "Peak filter with coeffs: " 
        << "b0=" << m_b0 << ", b1=" << m_b1 << ", b2="  << m_b2 
        << ", a0=" << m_a0 << ", a1=" << m_a1 << ", a2=" << m_a2 
        << std::endl;

}

filterConfig_t IIRFilter::getParams()
{
    std::cout << "IIRFILTER: type = " << m_params->type << std::endl;
    return *m_params;
}