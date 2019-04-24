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
   m_coeffs = new IIRCoeffs_t;
   m_filterBank = new IIRCoeffs_t[100];
   m_bankStart = new IIRCoeffs_t;
   m_bankStart = m_filterBank;

    /* just in case... */
    m_coeffs->a0 = 1.0; //atten factor so prevent div by zero
    m_coeffs->a1 = 0.0;
    m_coeffs->a2 = 0.0;
    m_coeffs->b0 = 0.0;
    m_coeffs->b1 = 0.0;
    m_coeffs->b2 = 0.0;
    m_outputs[0] = 0.0;
    m_outputs[1] = 0.0;
    m_inputs[0] = 0.0;
    m_inputs[1] = 0.0;

    m_sampleRate = sampleRate;
    m_params = new filterConfig_t;
    *m_params = params;

    CreateFilterBank(params);
    Update(params.dbGain);
    PrintCoefficients();
   
}

IIRFilter::IIRFilter(IIRCoeffs_t coeffs, int sampleRate)
{
    /*
    m_a0 = coeffs.a0;
    m_a1 = coeffs.a1;
    m_a2 = coeffs.a2;
    m_b0 = coeffs.b0;
    m_b1 = coeffs.b1;
    m_b2 = coeffs.b2;
    */
   *m_coeffs = coeffs;
}

IIRFilter::~IIRFilter()
{
    
}

void IIRFilter::PrintCoefficients()
{
    std::cout << m_coeffs->a0 << std::endl;
    std::cout << m_coeffs->a1 << std::endl;
    std::cout << m_coeffs->a2 << std::endl;
    std::cout << m_coeffs->b0 << std::endl;
    std::cout << m_coeffs->b1 << std::endl;
    std::cout << m_coeffs->b2 << std::endl;
}

double IIRFilter::filter(double sample)
{
    double accum;
    
    /* calculate feed foward */
    accum = (m_coeffs->b0 * sample) + (m_coeffs->b1 * m_inputs[0]) + (m_coeffs->b2 * m_inputs[1]);
    /* calculate feedback */
    accum += -( (m_coeffs->a1 * m_outputs[0]) + (m_coeffs->a2 * m_outputs[1]) );
    /* Bristow-Johnson extra coefficient (a0) */
    accum = accum / m_coeffs->a0;
    

    /* shift buffers */
    m_inputs[1] = m_inputs[0];
    m_inputs[0] = sample;
    m_outputs[1] = m_outputs[0];
    m_outputs[0] = accum;

    return accum;
}

void IIRFilter::CreateFilterBank(filterConfig_t params)
{
    std::cout << "IIRFilter: generating biquad filter bank for type " << params.type << std::endl;
    *m_params = params;
    switch(m_params->type)
    {
        case LOWPASS:       
        {
            //for (int i = 0; i < (sizeof(m_filterBank)/sizeof(IIRCoeffs_t)); i++)
            for (int i = 0; i < 100; i++)
            {
                m_params->dbGain = static_cast<double>(-0.1 * i);
                CalcLowpass(m_params, m_filterBank); 
                m_filterBank++;
            }
            m_filterBank = m_bankStart;
            break;
        }
        case BANDPASS:      
        {
            //for (int i = 0; i < (sizeof(m_filterBank)/sizeof(IIRCoeffs_t)); i++)
            for (int i = 0; i < 100; i++)
            {
                m_params->dbGain = static_cast<double>(-0.1 * i);
                CalcNotch(m_params, m_filterBank); 
                m_filterBank++;
            }
            m_filterBank = m_bankStart;
            break;
        }
        case HIGHPASS:      
        {
            //for (int i = 0; i < (sizeof(m_filterBank)/sizeof(IIRCoeffs_t)); i++)
            for (int i = 0; i < 100; i++)
            {
                m_params->dbGain = static_cast<double>(-0.1 * i);
                CalcHighpass(m_params, m_filterBank); 
                m_filterBank++;
            }
            m_filterBank = m_bankStart;
            break;
        }
        
        case PEAK:          
        {
            //for (int i = 0; i < (sizeof(m_filterBank)/sizeof(IIRCoeffs_t)); i++)
            for (int i = 0; i < 100; i++)
            {
                m_params->dbGain = static_cast<double>(-0.1 * i);
                std::cout << "m_params->dbGain " << m_params->dbGain << std::endl;
                CalcPeak(m_params, m_filterBank); 
                std::cout << "a0 " << m_filterBank->a0 << std::endl;
                m_filterBank++;
            }
            m_filterBank = m_bankStart;
            break;
        }
        case LOWSHELVE:    
        {
            //for (int i = 0; i < (sizeof(m_filterBank)/sizeof(IIRCoeffs_t)); i++)
            for (int i = 0; i < 100; i++)
            {
                m_params->dbGain = static_cast<double>(-0.1 * i);
                CalcLowShelve(m_params, m_filterBank); 
                m_filterBank++;
            }
            m_filterBank = m_bankStart;
            break;
        }
        case HIGHSHELVE:   
        {
            //for (int i = 0; i < (sizeof(m_filterBank)/sizeof(IIRCoeffs_t)); i++)
            for (int i = 0; i < 100; i++)
            {
                m_params->dbGain = static_cast<double>(-0.1 * i);
                CalcHighShelve(m_params, m_filterBank); 
                m_filterBank++;
            }
            m_filterBank = m_bankStart;
            break;
        }
        default: throw "ERROR: not a valid filter type";
    }
}

void IIRFilter::Update(double gain)
{
    int index = static_cast<int>(-10 * gain);
    m_coeffs = &m_filterBank[index];
    //std::cout << "Requested gain: " << gain << std::endl;
    //std::cout << "Index: " << index << std::endl;
    //std::cout << "atten: " << m_coeffs->a0 << std::endl;
}

void IIRFilter::CalcLowpass(filterConfig_t* params, IIRCoeffs_t* coeffs)
{
    //std::cout << "calc" << std::endl;
    double A, omega, alpha;

    A = pow(10, params->dbGain/(40.0*params->fCut));
    omega = (2 * M_PI * params->fCut) / m_sampleRate;
    alpha = sin(omega) / (2.0 * params->q);

    /* adjust coefficients */
    coeffs->a0 = (1 + alpha) ;
    coeffs->a1 = -2 * cos(omega);
    coeffs->a2 = 1 - alpha;
    coeffs->b0 = (1 - cos(omega)) / 2;
    coeffs->b1 = 1 - cos(omega);
    coeffs->b2 = coeffs->b0;


}

void IIRFilter::CalcNotch(filterConfig_t* params, IIRCoeffs_t* coeffs)
{
    
    double A, omega, alpha;

    A = pow(10, params->dbGain/(40.0*params->fCut));
    omega = (2 * M_PI * params->fCut) / m_sampleRate;
    alpha = sin(omega) / (2.0 * params->q);

    
    coeffs->a0 = (1 + alpha);
    coeffs->a1 = -2 * cos(omega);
    coeffs->a2 = 1 - alpha;
    coeffs->b0 = params->q * alpha;
    coeffs->b1 = 0;
    coeffs->b2 = -params->q * alpha;
    
    
}

void IIRFilter::CalcHighpass(filterConfig_t* params, IIRCoeffs_t* coeffs)
{
    double A, omega, alpha;

    A = pow(10, params->dbGain/(40.0*params->fCut));
    omega = (2 * M_PI * params->fCut) / m_sampleRate;
    alpha = sin(omega) / (2.0 * params->q);

    /* adjust coefficients */
    //m_a0 = 1 + (alpha + A);
    coeffs->a0 = (1 + alpha);
    coeffs->a1 = -2 * cos(omega);
    coeffs->a2 = 1 - alpha;
    coeffs->b0 = (1 + cos(omega)) / 2;
    coeffs->b1 = -(1 + cos(omega));
    coeffs->b2 = coeffs->b0;

}

void IIRFilter::CalcLowShelve(filterConfig_t* params, IIRCoeffs_t* coeffs)
{
    double A, omega, alpha, beta;

    A = pow(10.0, params->dbGain/40.0);
    omega = (2 * M_PI * params->fCut) / m_sampleRate;
    alpha = sin(omega) / (2.0 * params->q);
    beta = sqrt(2*A);

    coeffs->b0 = A * ((A + 1) - (A - 1) * cos(omega) + beta * sin(omega));
    coeffs->b1 = 2 * A * ((A - 1) - (A + 1) * cos(omega));
    coeffs->b2 = A * ((A + 1) - (A - 1) * cos(omega) - beta * sin(omega));
    coeffs->a0 = (A + 1) + (A - 1) * cos(omega) + beta * sin(omega);
    coeffs->a1 = -2 * ((A - 1) + (A + 1) * cos(omega));
    coeffs->a2 = (A + 1) + (A - 1) * cos(omega) - beta * sin(omega);

    /*
    std::cout << "Low shelve with coeffs: " 
        << "b0=" << m_b0 << ", b1=" << m_b1 << ", b2="  << m_b2 
        << ", a0=" << m_a0 << ", a1=" << m_a1 << ", a2=" << m_a2 
        << std::endl;
        */
}

void IIRFilter::CalcHighShelve(filterConfig_t* params, IIRCoeffs_t* coeffs)
{
    double A, omega, alpha, beta;

    A = pow(10.0, params->dbGain/40.0);
    omega = (2 * M_PI * params->fCut) / m_sampleRate;
    alpha = sin(omega) / (2.0 * params->q);
    beta = sqrt(2*A);

    coeffs->b0 = A * ((A + 1) + (A - 1) * cos(omega) + beta * sin(omega));
    coeffs->b1 = -2 * A * ((A - 1) + (A + 1) * cos(omega));
    coeffs->b2 = A * ((A + 1) + (A - 1) * cos(omega) - beta * sin(omega));
    coeffs->a0 = (A + 1) - (A - 1) * cos(omega) + beta * sin(omega);
    coeffs->a1 = 2 * ((A - 1) - (A + 1) * cos(omega));
    coeffs->a2 = (A + 1) - (A - 1) * cos(omega) - beta * sin(omega);

/*
    std::cout << "High shelve with coeffs: " 
        << "b0=" << m_b0 << ", b1=" << m_b1 << ", b2="  << m_b2 
        << ", a0=" << m_a0 << ", a1=" << m_a1 << ", a2=" << m_a2 
        << std::endl;
        */
}

void IIRFilter::CalcPeak(filterConfig_t* params, IIRCoeffs_t* coeffs)
{
    double A, omega, alpha;

    A = pow(10.0, params->dbGain/40.0);
    omega = (2 * M_PI * params->fCut) / m_sampleRate;
    alpha = sin(omega) / (2.0 * params->q);

    coeffs->b0 = 1 + (alpha * A);
    coeffs->b1 = -2 * cos(omega);
    coeffs->b2 = 1 - (alpha * A);
    coeffs->a0 = 1 + (alpha / A);
    coeffs->a1 = -2 * cos(omega);
    coeffs->a2 = 1 - (alpha / A);
/*
    std::cout << "Peak filter with coeffs: " 
        << "b0=" << m_b0 << ", b1=" << m_b1 << ", b2="  << m_b2 
        << ", a0=" << m_a0 << ", a1=" << m_a1 << ", a2=" << m_a2 
        << std::endl;
        */

}

filterConfig_t IIRFilter::getParams()
{
    //std::cout << "IIRFILTER: type = " << m_params->type << std::endl;
    return *m_params;
}