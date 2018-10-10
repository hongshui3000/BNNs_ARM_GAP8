

/**
 * \file BinaryLayer.h
 * \brief 	Binary convolution layer
 * \date 18.01.2017
 * 
 * \details 
 * 			1. Binary Convolution
 *			2. Scale 
 *			3. ReLU
 *			4. Batch Normalization
 *			5. Binarize 			
 * 
*/

#ifndef BINARYLAYER_H_
#define BINARYLAYER_H_

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
typedef struct BinaryLayer
{
    uint32_t* 	weight;
    int32_t* 	signGamma, *threshhold, *relu, *compare; 
    uint32_t 	nFilters;
    int32_t		kX, kY;
    int32_t 	padX, padY;
    int32_t 	dX, dY;
}BINARYLAYER;


typedef struct Bin_features
{
	uint32_t* weight; 
	int32_t*  signGamma, *threshhold, *relu, *compare; 
}BIN_FEATURES;

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
extern void binaryLayer_setConfiguration(LAYER_CONFIG* config);
/**
 * \brief			convolute 32 filters with input map 
 * @param[in]	input		..feature map
 * @param[in]	features	..weights and parameters
 * @param[out]	output		..feature map
 */
extern void binaryLayer_convolution_32filters(TENSOR* __restrict__ input, BIN_FEATURES* __restrict__ features, TENSOR* __restrict__ output);

#endif /* define BINARYLAYER_H_ */
