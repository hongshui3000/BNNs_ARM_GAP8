/*
 * \file confIG.h
 * \brief 		define application configuration 
 * \date 18.01.2017
 * 
 * \details 
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

/* ==================================================================================
 * Includes
 * ================================================================================*/
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h> 	/* ToDo: final, can be removed */

//#include "hal/semihosting.h"

#include "utility/misc.h"
#include "utility/tensor.h"

#include "pulp.h"

/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/

/* debug */ 
//#define DEBUG

/* enrolled */
#define UNFOLDED 

/* fixpoint */
//#define FIXPOINT_16BIT 
#define FIXPOINT_32BIT 

/* parallel processing */
#define MULTICORES

/*Allocated Location*/
#define ALLOC_LOCATION     RT_ALLOC_CL_DATA
//#define ALLOC_LOCATION    RT_ALLOC_L2_CL_DATA 

/* dma transfer l2 to l1 */
#define DMA_TRANS			/*< dma not working with 2bytes values */

/* Performance */
#define PERF

/* led toggle */
//#define LED

/******************************* 
* multicores (parallelisation)
*******************************/
#ifdef MULTICORES
#include <omp.h>
#endif



/**************************** 
* fix point 16 bit
*****************************/
#ifdef FIXPOINT_16BIT
#define FP_SCALE 		256
#define FP_SCALE_BITs 	8
#endif


/**************************** 
* fix point 16 bit
*****************************/
#ifdef FIXPOINT_32BIT
#define FP_SCALE 		1048576
#define FP_SCALE_BITs 	20
#endif


/******************************* 
* DMA used 
*******************************/
#ifdef DMA_TRANS 
/* empty */
#endif


#ifdef LED
/* empty */
#endif

#endif /* define CONFIG_H_ */
