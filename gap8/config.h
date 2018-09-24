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

/*my popcount*/
#define MYPOPCOUNT

/* parallel processing */
#define MULTICORES
#ifdef MULTICORES
	#define CORE_NUM   8
	#define CORE_NUM_SPEC   4
#endif
/*Allocated Location*/
#define ALLOC_LOCATION     RT_ALLOC_CL_DATA
//#define ALLOC_LOCATION    RT_ALLOC_L2_CL_DATA 

/* dma transfer l2 to l1 */
#define DMA_TRANS			/*< dma not working with 2bytes values */

/*MFCC preprocessing*/
#define MFCC_PREPROCESSING
#define HIGH_PASS
#define INT_INPUT //integer input to BNNs
//#define SHIFT12 //select model shift12
//#define BOARD_MODE
//#define FLOAT_INPUT //float input (samples) to MFCC 
//#define I2S_DEMO
/* Performance */
#define PERF
//#define DMA_PERF

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
* fix point 32 bit
*****************************/
#ifdef FIXPOINT_32BIT
//#define FP_SCALE 		1048576 //Q20
//#define FP_SCALE 		4096 //Q12
//#define FP_SCALE_BITs 	20  //Q20
//#define FP_SCALE_BITs 	12  //Q12
	#ifdef INT_INPUT  
		#ifdef SHIFT12 // Q12
		#define FP_SCALE_BIT_INs 0
		#define FP_SCALE 	4096 //Q12
		#define FP_SCALE_BITs 	12  //Q12
		#else
		#define FP_SCALE_BIT_INs 0
		#define FP_SCALE 	1048576 //Q20
		#define FP_SCALE_BITs 	20  //Q20
		#endif
	#else  //Q20 input
		#ifdef SHIFT12 //Q12
		#define FP_SCALE_BIT_INs 20
		#define FP_SCALE  	4096 //Q20
		#define FP_SCALE_BITs 	12  //Q20
		#else
		#define FP_SCALE_BIT_INs 20
		#define FP_SCALE 	1048576 //Q20
		#define FP_SCALE_BITs 	20  //Q20
		#endif
	#endif
#endif


/******************************* 
* DMA used 
*******************************/
#ifdef DMA_TRANS 
/* empty */
#endif

/******************************* 
* MFCC used 
*******************************/
#ifdef MFCC_PREPROCESSING
#include "mfcc/mfcc.h"
#endif

#ifdef LED
/* empty */
#endif

#endif /* define CONFIG_H_ */
