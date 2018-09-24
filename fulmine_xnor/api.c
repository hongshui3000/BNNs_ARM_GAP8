/**
 * \file api.c
 * \brief
 * \date 18.01.2017
 * 
 * \details 
 */

/**
 * \defgroup Application  <DESCRIPTION>
 * [\addtogroup] Application
 * @{
 */

/* ==================================================================================
 * Includes
 * ================================================================================*/

#include <math.h>
#include <stdio.h>

#include "config.h"
#include "platform/fulmine.h"

#include "model/model.h"
#ifdef FIXPOINT_16BIT
#include "input_fp16.dat"
#elif defined(FIXPOINT_32BIT)
#include "input_fp32.dat"
#else
#include "input.dat"
// #include "input_size_400.dat"
#endif
// #include <hwPerf.h>

/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/
/* ==================================================================================
 * Local/Global variables definitions
 * ================================================================================*/
#define N_ClASSES 		28
#define SUBSEC_SIZE		80
/* ==================================================================================
 * Local function declarations
 * ================================================================================*/
/* ==================================================================================
 * Local function implementations
 * ================================================================================*/
/* ==================================================================================
 * Main function implementations
 * ================================================================================*/
int main(int argc, char *argv[])
{
	/* set fulmine frequency to 100Mhz*/
	fll_set_cluster_frequency(400);

	// plp_trace_stop();

	// uint64_t res;
	// uint64_t* f = (uint64_t*) malloc(8);
	// uint64_t* x = (uint64_t*) malloc(8);

	// plp_trace_start();

	// res = *f * *x;

	// plp_trace_stop();

	// printf(" %d done\n", (int) res);
	// while(1);
	// while(1);
/*-------------------------------------
* 	performence start
*--------------------------------------*/
// #ifdef PERF
 // #pragma omp parallel
 // { 
	// 		hw_perf_t perf;
 //        hw_perf_init(&perf);
 //        hw_perf_step(&perf);
 // }
// #endif


#ifdef LED
	led_init();
#endif	

#ifdef DMA_TRANS	
	// printf("DMA activ \n");
#endif

/*-------------------------------------
* 	performence start
*--------------------------------------*/	

	uint32_t i,j,k;
	TENSOR* input = 0;
    uint32_t pos, mfccL, mfccR, invalidL, invalidR;
    double predict[N_ClASSES];

    #ifdef MULTICORES
    #pragma omp parallel for schedule(static)
    #endif
    for(i = 0; i < N_ClASSES; i++)
    {
		predict[i] = 0;
    }

	/*initialize model*/
	model_init(N_ClASSES);

 //    #ifdef LED
	// led_toggle();
	// #endif
	/* calculation loop */
	// printf("start \n" );
	pos = 80;
  //   for(pos = 0; pos < IMG_WIDTH; pos += SUBSEC_SIZE)
  //   {
		//  /*Pad input from the left and *right */  
		// if(pos == 0)
		// {
		//     mfccL = 0;
		//     invalidL = 0;
		// } 
		// else 
		// {
	    	mfccL = pos-12;
	    	invalidL = 3;
		// }
	
		// if(pos+SUBSEC_SIZE >= IMG_WIDTH)
		// {
	 //    	mfccR = IMG_WIDTH;
	 //    	invalidR = 0;
		// } 
		// else 
		// {
	    	mfccR = pos+SUBSEC_SIZE+8;
	    	invalidR = 2;
		// }
// printf("alloc \n");
#if defined(FIXPOINT_16BIT) || defined(FIXPOINT_32BIT)
		
		input = tensor_allocate(TENSOR_FIXPOINT, MFCC_BINS, mfccR-mfccL, 1);
		// printf("intput alloc \n");
		#ifdef MULTICORES
    	#pragma omp parallel for schedule(static) collapse(2)
    	#endif
		for(i = 0; i < MFCC_BINS; i++){
	    for(j = mfccL; j < mfccR; j++)
	    {
			input->memFixpoint[i*input->Y + j - mfccL] = mfcc[i][j];
	    }}
	    // printf("for done \n");
#else
	    // printf("wrong\n");
		/* Read input */
		input = tensor_allocate(TENSOR_FLOAT, MFCC_BINS, mfccR-mfccL, 1);
		/*TODO improve: load with dma faster?*/
		#ifdef MULTICORES
    	#pragma omp parallel for schedule(static) collapse(2)
    	#endif
		for(i = 0; i < MFCC_BINS; i++){
	    for(j = mfccL; j < mfccR; j++)
	    {
			input->memFloat[i*input->Y + j - mfccL] = mfcc[i][j];
	    }}
#endif

		/* Forward pass */
	// 	 #pragma omp parallel
 // { 

	// 	hw_perf_start(&perf);
		model_forward(input, invalidL, invalidR, predict);
	// }
	    // printf("forward start \n");
		// printf("forward done\n");
	// 	 #pragma omp parallel
 // { 
    #ifdef LED
	led_toggle();
	#endif
	// 	hw_perf_stop(&perf);
	// }
    // }

    /* AveragePool */
    #ifdef MULTICORES
    #pragma omp parallel for schedule(static)
    #endif
    for(i = 0; i < N_ClASSES; i++)
    {
		predict[i] = predict[i]/(16*100);
    }
    
    /* LogSoftMax */
    float sum = 0;

    #ifdef MULTICORES
    #pragma omp parallel for schedule(static)
    #endif
    for(i = 0; i < N_ClASSES; i++)
    {
		sum += expf(predict[i]);
    }

    sum = 1/sum;

    #ifdef MULTICORES
    #pragma omp parallel for schedule(static)
    #endif
    for(i = 0; i < N_ClASSES; i++)
    {
		predict[i] = logf(sum*expf(predict[i]));
    }

/*-------------------------------------
* 	performence end
*--------------------------------------*/
#ifdef PERF 
perf_print();
#endif

/*-------------------------------------
* 	performence end
*--------------------------------------*/	

    #ifdef LED
	led_toggle();
	#endif

    /* Print result */
    for(i = 0; i < N_ClASSES; i++)
    {
		myPrintf(predict[i]);
    }

 //    #ifdef LED
	// led_toggle();
	// #endif

    // #pragma omp critical

    // {

    //     // hw_perf_stop(&perf[id]);
    // hw_perf_commit(&perf);

    // printf("[CORE %d] HW performance counters results\n", plp_coreId());
  
    // for (i=0; i<hw_perf_nb_events(&perf); i++)
    // {
    //     printf("%20s: %d\n", hw_perf_get_name(&perf, i), hw_perf_get_value(&perf, i));
  
    // }  
    // }
    
	printf(" run successfull \n");

    return 0;    
}
