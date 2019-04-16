/**
 * @file ParametricFilter.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-09
 * 
 * @copyright Copyright (c) 2019
 * 
 */

class ParametricFilter: public IIRFilter 
{
public:
    /**
     * @brief Construct a new Parametric Filter object
     * 
     */
    ParametricFilter();

    /**
     * @brief Destroy the Parametric Filter object
     * 
     */
    ~ParametricFilter();

    /**
     * @brief 
     * 
     * @param freq 
     */
    void UpdateCutoff(int freq);

private:
    
};