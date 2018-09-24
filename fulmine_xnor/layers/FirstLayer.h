

/**
 * \file FirstLayer.h
 * \brief Additional firmware test-funcitonality for SIL-specific tests
 * \date 18.01.2017
 * 
 * \details 
 * 			1. Convolution (float)
 *			2. Batch Normalization 
 *			3. ReLU
 *			4. Batch Normalization
 *			5. Binarize 			
 * 
*/

#ifndef FIRSTLAYER_H_
#define FIRSTLAYER_H_

/**
 * \defgroup Layer
 * [\addtogroup] Layer
 * @{
 */

/* ==================================================================================
 * Includes
 * ================================================================================*/
// #include <stdint.h>
// #include "../utility/tensor.h"
#include "../config.h"
#include "LayerConfig.h"
/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/
#ifdef FIXPOINT_16BIT
typedef struct FirstLayer_Features
{
    int16_t *weight, *bias;
    int16_t *mean1, *std1, *gamma1, *beta1;
    int16_t *mean2, *std2, *gamma2, *beta2;
}FIRST_FEATURES;
#elif defined(FIXPOINT_32BIT)
typedef struct FirstLayer_Features
{
    int32_t *weight, *bias;
    int32_t *mean1, *std1, *gamma1, *beta1;
    int32_t *mean2, *std2, *gamma2, *beta2;
}FIRST_FEATURES;
#else /* floating point */
typedef struct FirstLayer_Features
{
    float *weight, *bias;
    float *mean1, *std1, *gamma1, *beta1;
    float *mean2, *std2, *gamma2, *beta2;
}FIRST_FEATURES;
#endif
/* ==================================================================================
 * Global variables declarations
 * ================================================================================*/
/* ==================================================================================
 * Global function prototypes
 * ================================================================================*/


/**
 * \brief			set layer configuration 
 * @param[in]	confi	..layer configuration
 */
extern void firstLayer_setConfiguration(LAYER_CONFIG* config);
/**
 * \brief			convolute 32 filters with input map 
 * @param[in]	input		..input map
 * @param[in]	features	..weights and parameters
 * @param[out]	output		..output feature map
 */
extern void firstLayer_convolution_32filters(TENSOR* __restrict__ input, FIRST_FEATURES* __restrict__ features, TENSOR* __restrict__ output);


#endif /* define FIRSTLAYER_H_ */