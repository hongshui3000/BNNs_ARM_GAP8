

/**
 * \file tensor.h
 * \brief 
 * \date 18.01.2017
 * 
 * \details 
 */ 


#ifndef TENSOR_T_
#define TENSOR_T_

/**
 * \defgroup Utility 	.. 
 * [\addtogroup] Utility
 * @{
 */

/* ==================================================================================
 * Includes
 * ================================================================================*/
#include "../config.h"
#include "../layers/LayerConfig.h"

/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/

/**
 * \brief  Struct typedef
 */
typedef enum
{
	TENSOR_FLOAT = 0,
	TENSOR_BINARY = 1,
	TENSOR_FIXPOINT = 2	
}TENSOR_TYPE;

/**
 * \brief  Struct typedef
 */
typedef struct
{
	uint32_t 	X, Y, Z;
	TENSOR_TYPE type;
	float* 		memFloat;
	uint32_t* 	memBin;
#ifdef FIXPOINT_16BIT
	int16_t*	memFixpoint;
#else 
	int32_t* 	memFixpoint;
#endif
}TENSOR;


/* ==================================================================================
 * Global variables declarations
 * ================================================================================*/
/* ==================================================================================
 * Global function prototypes
 * ================================================================================*/


/**
 * \brief			Thread to trigger the watchdog
 * @param[in]	u8_ex	..blabla
 * @param[in]	u16_ex	..bla
 * @param[in]	i32_ex	..bla
 * @param[out]			..blabkla
 */
extern TENSOR* tensor_allocate(TENSOR_TYPE type, uint32_t x, uint32_t y, uint32_t z);
extern TENSOR* tensor_alloc_featureMap(TENSOR* input, LAYER_CONFIG* config, uint32_t nFilters);
extern void tensor_free(TENSOR* tensor);



#endif /* define TENSOR_T_ */