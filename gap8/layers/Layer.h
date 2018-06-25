

/**
 * \file Layer.h
 * \brief 	top level layer
 * \date 27.04.2017
 * 
*/

#ifndef LAYER_H_
#define LAYER_H_

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
// #include "LayerConfig.h"

#include "../layers/FirstLayer.h"
#include "../layers/BinaryLayer.h"
#include "../layers/LastLayer.h"

/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/
typedef enum Layer_Type
{
	fLayer = 0,
	bLayer = 1,
	lLayer = 2
}LAYER_TYPE;

typedef struct Layer
{
	uint32_t 		nFilters;
    LAYER_TYPE		type;
	LAYER_CONFIG*	config;
	void* 			features[2];
}LAYER;

/* ==================================================================================
 * Global variables declarations
 * ================================================================================*/
/* ==================================================================================
 * Global function prototypes
 * ================================================================================*/

#endif /* define LAYER_H_ */