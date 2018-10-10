

/**
 * \file LastLayer.h
 * \brief 			
 * \date 18.01.2017
 * 
 * \details 
 * 			1. Binary Convolution 
 *			2. Scale 
 *			3. ReLU
 *			4. Batch Normalization
 *			5. Convolution (float)
 *			6. Accumulation  			
 * 
*/

#ifndef LASTLAYER_H_
#define LASTLAYER_H_

/**
 * \defgroup Layer
 * [\addtogroup] Layer
 * @{
 */

/* ==================================================================================
 * Includes
 * ================================================================================*/
#include "../config.h"
#include "LayerConfig.h"
/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/
#ifdef FIXPOINT_16BIT
typedef struct LastLayer
{
    uint32_t* 	weight1;
    int16_t*	weight2;
    int16_t*    bias1, *alpha1, *bias2;
    int16_t* 	mean, *std, *gamma, *beta;    
    uint32_t 	nFilters1, nFilters2;
}LAST_FEATURES;
#elif defined(FIXPOINT_32BIT)
typedef struct LastLayer
{
    uint32_t*   weight1;
    int32_t*    weight2;
//    int32_t*    weight2_1;
//    int32_t*    weight2_2;
//    int32_t*    weight2_3;
//    int32_t*    weight2_4;
    int32_t*    bias1, *alpha1, *bias2;
    int32_t*    mean, *std, *gamma, *beta;    
    uint32_t    nFilters1, nFilters2;
}LAST_FEATURES;
#else
typedef struct LastLayer
{
    uint32_t* 	weight1;
    float*		weight2;
    float*      bias1, *alpha1, *bias2;
    float* 		mean, *std, *gamma, *beta;    
    uint32_t 	nFilters1, nFilters2;
}LAST_FEATURES;
#endif

/* ==================================================================================
 * Global variables declarations
 * ================================================================================*/
/* ==================================================================================
 * Global function prototypes
 * ================================================================================*/
/**
 * \brief			set layer configuration 
 * @param[in]				.. is doing nothing right now
 */
extern void lastLayer_setConfiguration(void);
/**
 * \brief			convolute 32 filters with input map 
 * @param[in]	input		..feature map
 * @param[in]	l			..weights and parameters, filter sizes
 * @param[in]	invalidL	..overlapping left (through zero padding, not used to get prediction)
 * @param[in]	invalidR	..overlapping right (through zero padding, not used to get prediction)
 * @param[out]	predict		..prediction 
 */
extern void lastLayer_convolution(TENSOR* __restrict__ input, LAST_FEATURES* __restrict__ l, uint32_t invalidL, uint32_t invalidR, float* __restrict__ predict);

#endif /* define LASTLAYER_H_ */
