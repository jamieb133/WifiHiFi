/**
 * @file FIRFilter.cpp
 * @author Jamie Brown
 * @brief 
 * @version 0.1
 * @date 2019-03-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "FIRFilter.h"

#include <iostream>
#include <math.h>

#define PI 3.14159

extern const double exampleLowpassImpulse[];

using namespace std;
using namespace WifiHifi;

FIRFilter::FIRFilter(filter_t type, double fCut, int taps)
{
    switch(type)
    {
        case LOWPASS: 
            if (LowpassCoeffs(taps, fCut)) { m_fir = new Fir1(m_coeffs, taps); }
            else { m_fir = NULL; }
            break;
        case HIGHPASS:
            if ( HighpassCoeffs(taps, fCut) ) { m_fir = new Fir1(m_coeffs, taps); }
            //if ( HighpassCoeffs(taps, fCut) ) { m_fir = new Fir1(highpassImpulse, 99); }
            else { m_fir = NULL; }
            break;
        case ALLPASS:
            if ( AllpassCoeffs(taps) ) { m_fir = new Fir1(m_coeffs, taps); }
            //if ( HighpassCoeffs(taps, fCut) ) { m_fir = new Fir1(highpassImpulse, 99); }
            else { m_fir = NULL; }
            break;
        default:
            std::cout << "ERROR: bad filter parameters" << std::endl;
    }
    
    if (m_fir == NULL)
    {
        std::cout << "failed to create crossover filter" << std::endl;
    }

    /* cleanup */
    delete m_coeffs;
}

FIRFilter::~FIRFilter()
{

}

double FIRFilter::filter(double sample)
{
    if (m_fir != NULL)
    {
        return m_fir->filter(sample);
    }
    else
    {
        cout << "ERROR: could not filter sample" << endl;
        return 0;
    }
}

bool FIRFilter::HighpassCoeffs(int taps, double fCut)
{
    m_coeffs = new double[taps];
    double* hamWin = new double[taps];

    for(int count = 0 ; count < taps ; count++)
    {
        /* calculate hamming window */
        //hamWin[count] = 0.54 - ( 0.46 * cos( ( 2 * PI * count ) / taps ) );
        hamWin[count] = 0.42 + ( 0.5 * cos( ( 2 * PI * count ) / taps ) ) + (0.08 * cos( ( 4 * PI * count ) / taps ) ); //this is actually a blackman window 

        /* prevent division by zero */
        if (count == 0)
        {
            /* coefficient for n = 0 */
            m_coeffs[0] = 1 - (2 * fCut) ;
        }
        else
        {
            /* calculate coefficients */
            m_coeffs[count] = -1*sin( 2 * PI * fCut * count ) / ( PI * count );
            //m_coeffs[count] = ( 2 * fCut) * ( sin( 2 * PI * fCut * count ) / ( PI * count ) );
        }
                                                 
        /* apply window and attenuate */    
        m_coeffs[count] *=  hamWin[count];                                             
    }
    cout << "Generated a high-pass FIR filter" << endl;
    return true;
}

bool FIRFilter::LowpassCoeffs(int taps, double fCut)
{
    m_coeffs = new double[taps]; 
    double* hamWin = new double[taps]; //hamming window (raised cosine) function

    for(int count = 0 ; count < taps ; count++)
    {
        /* calculate hamming window */
        //hamWin[count] = 0.54 - ( 0.46 * cos( ( 2 * PI * count ) / taps ) );
        hamWin[count] = 0.42 + ( 0.5 * cos( ( 2 * PI * count ) / taps ) ) + (0.08 * cos( ( 4 * PI * count ) / taps ) ); //this is actually a blackman window 

        /* prevent division by zero */
        if (count == 0)
        {
            /* coefficient for n = 0 */
            m_coeffs[0] = 2 * fCut ;
        }
        else
        {
            /* calculate coefficients */
            m_coeffs[count] = sin( 2 * PI * fCut * count ) / ( PI * count );
            //m_coeffs[count] = ( 2 * fCut) * ( sin( 2 * PI * fCut * count ) / ( PI * count ) );
        }
                                       
        /* apply window and attenuate */    
        m_coeffs[count] *=  hamWin[count];                                         
    }
    
    cout << "Generated a low-pass FIR filter" << endl;
    delete hamWin;
    return true;
}

bool FIRFilter::AllpassCoeffs(int taps)
{
    m_coeffs = new double[taps]; 
    double* hamWin = new double[taps]; //hamming window (raised cosine) function

    for(int count = 0 ; count < taps ; count++)
    {
        /* calculate hamming window */
        hamWin[count] = 0.54 - ( 0.46 * cos( ( 2 * PI * count ) / taps ) );   
        /* allpass so just apply window function */    
        m_coeffs[count] =  hamWin[count];  


        //m_coeffs[count] = 1;                                       
    }
    
    cout << "Generated an all-pass FIR filter" << endl;
    delete hamWin;
    return true;
}

void FIRFilter::reset()
{
    m_fir->reset();
}