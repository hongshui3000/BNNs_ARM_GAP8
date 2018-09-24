

/**
 * \file FirstLayer.h
 * \brief 		neural conovolution model (Alex-Net)
 * \date 18.01.2017
 * 
 * \details 
 * 
*/

#ifndef MODEL_H_
#define MODEL_H_

/**
 * \defgroup Model  
 * [\addtogroup] Model
 * @{
 */

/* ==================================================================================
 * Includes
 * ================================================================================*/
#include "../layers/Layer.h"
#include "../config.h"
/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/
/* ==================================================================================
 * Global variables declarations
 * ================================================================================*/
/* ==================================================================================
 * Global function prototypes
 * ================================================================================*/

/**
 * \brief			initialize model
 * @param[in]	classes		..prediction classes 
 */
extern void model_init(uint32_t classes);

/**
 * \brief			forward input image through model
 * @param[in]	input		..input image
 * @param[in]	invalidL	..overlapping left (through zero padding, not used to get prediction)
 * @param[in]	invalidR	..overlapping right (through zero padding, not used to get prediction)
 * @param[out]	predict		..prediction 
 */
extern void model_forward(TENSOR* input, uint32_t invalidL, uint32_t invalidR, double* predict);


#endif /* define MODEL_H_ */