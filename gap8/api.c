/**
 * \file api.c
 * \brief
 * \date 16.06.2018
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
//#include <stdlib.h>
#include "config.h"
//#include "platform/fulmine.h"

#include "model/model.h"
#ifdef FIXPOINT_16BIT
//#include "input_fp16.dat"
#include "input_int400.dat"
#elif defined(FIXPOINT_32BIT)
//#include "input_fp32.dat"
#include "input_fp32_400.dat"
#else
//#include "input.dat"
#include "input_size_400.dat"
#endif
// #include <hwPerf.h>

/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/
/* ==================================================================================
 * Local/Global variables definitions
 * ================================================================================*/
#define N_ClASSES 		28
#define SUBSEC_SIZE		40
/* ==================================================================================
 * Main function implementations
 * ================================================================================*/
int main(int argc, char *argv[])
{
	/* set fulmine frequency to 100Mhz*/
	//fll_set_cluster_frequency(400);
	// plp_trace_stop();
	//uint64_t res;
	//uint64_t* f = (uint64_t*) rt_alloc(RT_ALLOC_CL_DATA,8);
	//uint64_t* x = (uint64_t*) rt_alloc(RT_ALLOC_CL_DATA,8);
	// plp_trace_start();
	// res = *f * *x;
	// plp_trace_stop();
	// printf(" %d done\n", (int) res);
	/*-------------------------------------
	 * 	performence start
	 *--------------------------------------*/
#ifdef PERF
	 
	rt_perf_t *perf;
	perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
	rt_perf_init(perf);
	rt_perf_conf(perf, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
	
#endif


#ifdef LED
	led_init();
#endif	

#ifdef DMA_TRANS	
	printf("DMA activ \n");
#else
	printf("MCU activ \n");
#endif

	rt_cluster_mount(1, 0, 0, NULL);
#ifdef DEBUG
	rt_user_alloc_dump(rt_alloc_l1(0));
#endif

	uint32_t i,j,k;
	TENSOR* input = 0;
	uint32_t pos, mfccL, mfccR, invalidL, invalidR;
	double predict[N_ClASSES];

#ifdef MULTICORES
#pragma omp parallel 
{
#pragma omp for schedule(static)
#endif
	for(i = 0; i < N_ClASSES; i++)
	{
		predict[i] = 0;
	}
#ifdef MULTICORES
}
#endif

#ifdef DEBUG
	rt_user_alloc_dump(rt_alloc_l1(0));
	//initialize model
	printf("model_init \n");
#endif
	model_init(N_ClASSES);
	// #ifdef LED
	// led_toggle();
	// #endif
#ifdef PERF
	rt_perf_start(perf);
#endif
	/*-------------------------------------
	 * 	performence start
	 *--------------------------------------*/
	int iter = 1;
	for(pos = 0; pos < IMG_WIDTH; pos += SUBSEC_SIZE)
	{
		printf("%d .               Iteration of input\n",iter);
		// *Pad input from the left and *right  
		if(pos == 0)
		{
			mfccL = 0;
			invalidL = 0;
		} 
		else 
		{
			mfccL = pos-12;
			invalidL = 3;
		}

		if(pos+SUBSEC_SIZE >= IMG_WIDTH)
		{
			mfccR = IMG_WIDTH;
			invalidR = 0;
		} 
		else 
		{
			mfccR = pos+SUBSEC_SIZE+8;
			invalidR = 2;
		}

#if defined(FIXPOINT_16BIT) || defined(FIXPOINT_32BIT)
#ifdef DEBUG
		printf("Allocate input to L1 : ");
#endif
		input = tensor_allocate(TENSOR_FIXPOINT, MFCC_BINS, mfccR-mfccL, 1);
#ifdef MULTICORES
#pragma omp parallel
		{
#pragma omp for private(j)
#endif
			for(i = 0; i < MFCC_BINS; i++){
				for(j = mfccL; j < mfccR; j++)
				{
					input->memFixpoint[i*input->Y + j - mfccL] = mfcc[i][j];
				}
			}
#ifdef MULTICORES
		}
#endif
#else
#ifdef DEBUG
		printf("Floating point input \n");
#endif
		// Read input 
		input = tensor_allocate(TENSOR_FLOAT, MFCC_BINS, mfccR-mfccL, 1);
		L1_usage += input->X*input->Y*input->Z*sizeof(float) + sizeof(TENSOR);
#ifdef DEBUG
		printf("L1 usage : %ld Byte \n",L1_usage);
#endif
		//TODO improve: load with dma faster?
#ifdef MULTICORES
#pragma omp parallel
		{
#pragma omp for private(j)
#endif
			for(i = 0; i < MFCC_BINS; i++){
				for(j = mfccL; j < mfccR; j++){
					input->memFloat[i*input->Y + j - mfccL] = mfcc[i][j];
				}
			}
#ifdef MULTICORES
#pragma omp barrier
		}
#endif
#endif
		printf("forward start \n");
		// #pragma omp parallel
		//{ 
		model_forward(input, invalidL, invalidR, predict);
		// }
		printf("forward done\n");
		//#pragma omp parallel
		// { 
		//#ifdef LED
		//led_toggle();
		//#endif
		// }
		iter++;
	}
	/*-------------------------------------
	 * 	performence stop
	 *--------------------------------------*/
	//  AveragePool 
	float sum = 0;
#ifdef MULTICORES
#pragma omp parallel
	{
#pragma omp for schedule(static)
#endif
		for(i = 0; i < N_ClASSES; i++)
		{
			predict[i] = predict[i]/1600;
		}
#ifdef MULTICORES
		// LogSoftMax 
#pragma omp for reduction(+:sum) //I changed here, instead of schedule(static)
#endif
		for(i = 0; i < N_ClASSES; i++)
		{
			sum += expf(predict[i]);
		}
#ifdef MULTICORES
#pragma omp single
		{
#endif
			sum = 1/sum;
#ifdef MULTICORES
		}
#pragma omp for schedule(static)	
#endif
		for(i = 0; i < N_ClASSES; i++)
		{
			predict[i] = logf(sum*expf(predict[i]));
		}
#ifdef MULTICORES
	}
#endif

#ifdef LED
	led_toggle();
#endif
#ifdef PERF
	rt_perf_stop(perf);
#endif
	// Print result 
	for(i = 0; i < N_ClASSES; i++)
	{
		myPrintf(predict[i], i);
	}

	// #ifdef LED
	// led_toggle();
	// #endif

#ifdef PERF
	rt_perf_save(perf);
	int instr = rt_perf_get(perf, RT_PERF_INSTR);
	int cycles = rt_perf_get(perf, RT_PERF_CYCLES);
	//int active_cycles = rt_perf_get(perf, RT_PERF_ACTIVE_CYCLES);
	//unsigned int time_us = rt_time_get_us();
	printf("instr : %d\n", instr);
	printf("cycles : %d\n", cycles);
	//printf("active : %d\n", active_cycles);
	//printf("time : %u us\n", time_us);
#endif
#ifdef DMA_TRANS	
	printf("DMA activ \n");
#else
	printf("MCU activ \n");
#endif
	printf("run successfull!!! \n");
	return 0;    
}
