/**
 * @file CrossoverFilter.h
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

class CrossoverFilter {

    public:
        /**
         * @brief Construct a new Fir Filter object
         * 
         * @param type 
         * @param fCut 
         * @param taps 
         */
        CrossoverFilter(filter_t type, double fCut, int taps);

        /**
         * @brief Destroy the Crossover Filter object
         * 
         */
        ~CrossoverFilter();

        /**
         * @brief 
         * 
         * @param sample 
         * @return double 
         */
        double filter(double sample);
        
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
         */
        Fir1 *m_fir;

        /**
         * @brief 
         * 
         */
        double *m_coeffs;
};

