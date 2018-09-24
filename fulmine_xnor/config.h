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

#include "hal/semihosting.h"

#include "utility/misc.h"
#include "utility/tensor.h"

#include "pulp.h"

/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/

/* debug */ 
// #define DEBUG

/* enrolled */
#define UNFOLDED 

/* fixpoint */
// #define FIXPOINT_16BIT 
#define FIXPOINT_32BIT 

/* parallel processing */
#define MULTICORES

/* dma transfer l2 to l1 */
// #define DMA_TRANS			/*< dma not working with 2bytes values */

/* Performance */
// #define PERF
// #include <hwPerf.h>

/* led toggle */
#define LED

/******************************* 
* multicores (parallelisation)
*******************************/
#ifdef MULTICORES
#include <omp.h>
#endif

/**************************** 
* debug 
*****************************/
#ifdef DEBUG
#include <stdio.h> 
#define L1_BASE 	268435456 	/* l1 offset */
#define L1_SIZE		65536		/* size of l1 */

#define l1_cal_address(units, sizeOf) ({\
	void* retVal = calloc(units, sizeOf);\
	uint32_t size = units*sizeOf;\
	uint32_t start = ((uint32_t) retVal) - L1_BASE;\
	printf(" at %u  to %u  size %u ", start, start + size, size);\
	uint32_t mod = ((uint32_t) retVal) % 8;\
	if(mod == 0)\
		printf("  ok \n");\
	else\
		printf("   %d \n", (int)mod );\
	retVal; })

#define l1_mal_address(size) ({\
	void* retVal = malloc(size);\
	int32_t start = ((int32_t) retVal) - L1_BASE;\
	printf(" at %d  to %d  size %d ", start, start + size, size);\
	if( start + size > L1_SIZE )\
		printf("  <-------------" );\
	uint32_t mod = ((uint32_t) retVal) % 8;\
	if(mod == 0)\
		printf("  ok \n");\
	else\
		printf("   %d \n", (int)mod );\
	retVal; })

#define calloc(units, sizeOf) ({printf("calloc: "), l1_cal_address(units,sizeOf);})
#define malloc(size) ({printf("malloc: "), l1_mal_address(size);})

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


/******************************* 
* performence test 
*******************************/
#ifdef PERF 
/* empty */
#include <hwPerf.h>
hw_perf_t perf[4];
#endif

#ifdef LED
/* empty */
#endif

#endif /* define CONFIG_H_ */
