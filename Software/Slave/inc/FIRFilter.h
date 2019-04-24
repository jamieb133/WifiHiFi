/**
 * @file FIRFilter.h
 * @author Jamie Brown
 * @brief 
 * @version 0.1
 * @date 2019-03-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "types.h"

#include <Fir1.h>

using namespace WifiHifi;

class FIRFilter {

    public:
        /**
         * @brief Construct a new Fir Filter object
         * 
         * @param type 
         * @param fCut 
         * @param taps 
         */
        FIRFilter(filter_t type, double fCut, int taps);

        /**
         * @brief Destroy the Crossover Filter object
         * 
         */
        ~FIRFilter();

        /**
         * @brief 
         * 
         * @param sample 
         * @return double 
         */
        double filter(double sample);

        /**
         * @brief remove all samples from the buffer
         * 
         * @return double 
         */
        void reset();
        
    private:
        /**
         * @brief 
         * 
         * @param taps 
         * @param fCut 
         * @return true 
         * @return false 
         */
        bool LowpassCoeffs(int taps, double fCut);

        /**
         * @brief 
         * 
         * @param taps 
         * @param fCut 
         * @return true 
         * @return false 
         */
        bool HighpassCoeffs(int taps, double fCut);

        /**
         * @brief 
         * 
         * @param taps 
         * @return true 
         * @return false 
         */
        bool AllpassCoeffs(int taps);
        
        /**
         * @brief 
         * 
         */
        Fir1 *m_fir;

        /**
         * @brief 
         * 
         */
        double *m_coeffs;
};

