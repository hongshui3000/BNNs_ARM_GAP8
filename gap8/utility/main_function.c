

/**
 * \file misc.c
 * \brief   miscellaneous functions
 * \date 18.01.2017
 * 
 * \details 
 */ 

/**
 * \defgroup Utility <DESCRIPTION>
 * [\addtogroup] Utility
 * @{
 */

/* ==================================================================================
 * Includes
 * ================================================================================*/
#include "main_function.h"
/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/
#ifdef MFCC_PREPROCESSING
#ifdef FLOAT_INPUT
	#ifndef I2S_DEMO
//#include "../A_section_of_waveF.dat"	//float format [-1,1]
//#include "A_section_of_waveF1.dat"	//float format [-1,1] //class1
//#include "A_section_of_waveF3.dat"	//float format [-1,1]
//#include "A_section_of_waveF16.dat"	//float format [-1,1]
	#include "../A_section_of_waveF30.dat"	//float format [-1,1]
//#include "A_section_of_waveF33.dat"	//float format [-1,1] //271M
//#include "Class3_F160.dat"	//float format [-1,1]
//#include "Class3_F164.dat"	//float format [-1,1]
//#include "Class3_F175.dat"	//float format [-1,1]
	#endif
#else
	#ifndef I2S_DEMO
//#include "A_section_of_wave2.dat" //Q15 format wave sample
//#include "A_section_of_wave3.dat" //Q15 format wave sample
//#include "A_section_of_wave25.dat" //Q15 format wave sample
//#include "A_section_of_wave30.dat" //Q15 format wave sample
//#include "A_section_of_wave33.dat" //Q15 format wave sample
//	#include "../applaus2.dat"
//	#include "../applaus3.dat"
//	#include "../applaus4.dat"
//	#include "../applaus4_original.dat"

//#include "../viol_raw_record.dat"

//	#include "../violin_record.dat"
//	#include "../violin_record2.dat" //remove DC, amplitude still small
//	#include "../violin_record3.dat" //remove DC, amplitude increase a bit
	#include "../violin_record4.dat" //remove DC, amplitude increase a bit, remove beginning ripple
//	#include "../guitar_record.dat"
//	#include "../violin_original.dat"
//	#include "../Class3_160.dat" //Q15 format wave sample
//#include "Class3_164.dat" //Q15 format wave sample
//#include "../Class3_175.dat" //Q15 format wave sample
//#include "DC_wave.dat"
//#include "DC_wave2.dat"
//#include "DC_wave3.dat"
	#endif
#endif
#else
#ifdef FIXPOINT_16BIT
//#include "input_fp16.dat"
//#include "input_int400.dat"
#include "1_1_f16.dat" //spectrogram
//#include "2_301_f16.dat"
#elif defined(FIXPOINT_32BIT)
#ifdef INT_INPUT
#include "../1_1_int.dat" //spectrogram
#else
//#include "input_fp32.dat"
//#include "input_fp32_400.dat"
#include "../1_1_f32.dat" //spectrogram
#endif
#else
//#include "input.dat"
//#include "input_size_400.dat"
#include "1_1_F.dat"
#endif
#endif
#ifdef MFCC_PREPROCESSING
#ifdef FLOAT_INPUT
float *InSignal = wav_sample;
#else
#ifdef I2S_DEMO
//unsigned short int *InSignal = buff.buff[0];
short int *InSignal;
#else
short int *InSignal = wav_sample;
#endif
#endif
#endif
/**
 * \brief Struct with function pointers for all fram functions
 */

/* ==================================================================================
 * Local/Global variables definitions
 * ================================================================================*/
/* ==================================================================================
 * Local function declarations
 * ================================================================================*/
/* ==================================================================================
 * Local function implementations
 * ================================================================================*/
/* ==================================================================================
 * Global function implementations
 * ================================================================================*/
#define N_ClASSES       28
#define SUBSEC_SIZE     40
const char *class_name[28]={ 
"acoustic_guitar",
"airplane       ",
"applause       ",
"bird           ",
"car            ",
"cat            ",
"child          ",
"church_bell    ",
"crowd          ",
"dog barking    ",
"engine         ",
"fireworks      ",
"footstep       ",
"glass_breaking ",
"hammar         ",
"helicopter     ",
"knock          ",
"laughter       ",
"mouse_click    ",
"ocean_surf     ",
"rustle         ",
"scream         ",
"speech         ",
"squeak         ",
"tone           ",
"violin         ",
"water_tap      ",
"whistle        "
 };

void firstfunction(void *arg){
	/*-------------------------------------
	 * 	performence start
	 *--------------------------------------*/
//	printf("(%d, %d) Entered cluster with arg %x\n", rt_cluster_id(), rt_core_id(), (int)arg);
    #ifdef MULTICORES
    rt_omp_start();
    #endif

	uint32_t i,j,k;
	TENSOR* input;
	uint32_t pos, mfccL, mfccR, invalidL, invalidR;
	float* predict;
	predict = rt_alloc(ALLOC_LOCATION,N_ClASSES*sizeof(float));
#ifdef MULTICORES
#pragma omp parallel for schedule(static) num_threads(CORE_NUM)
#endif
	for(i = 0; i < N_ClASSES; i++)
	{
		predict[i] = 0.0;
		//printf("(%d, %d) predict PE with arg %\n", rt_cluster_id(), rt_core_id());
	}
#ifdef DEBUG
	//initialize model
	printf("model_init \n");
#endif

	model_init(N_ClASSES);



#ifdef PERF

//	rt_perf_t *perf;
//	perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
//	rt_perf_init(perf);
//	rt_perf_conf(perf, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
//	rt_perf_reset(perf);
//	rt_perf_start(perf);
        
#endif

		int iter = 1;
		for(pos = 0; pos < IMG_WIDTH; pos += SUBSEC_SIZE)
		{
#ifdef DEBUG
			printf("%d .               Iteration of input\n",iter);
#endif
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
			/* ==================================================================================
			 * MFCC Preprocessing here.
			 * ================================================================================*/

#ifdef MFCC_PREPROCESSING
			{

#ifndef PERF
				rt_perf_t *perf2;
				perf2 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
				rt_perf_init(perf2);
				rt_perf_conf(perf2, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));

				rt_perf_reset(perf2);
				rt_perf_start(perf2);
#endif          
				int winStep;
				int width;
				//printf("Start MFCC\n");
				short int* img_1d= rt_alloc(ALLOC_LOCATION, NUM_MFCC_FEATURES * sizeof(short int)); //64
				winStep = WIN_LENGTH - WIN_OVERLAP;             //512 - 472 = 40
				MFCC_init();
				#ifdef I2S_DEMO
				InSignal = (short int*)arg; //i2s input buffer
				#endif
				for(j=mfccL; j<mfccR; j++)
				{
					//Read input from L2
					mfcc_compute(InSignal + j*winStep, img_1d);  //output = Q16 (overflow for fixpoint 16)
#ifdef MULTICORES
#pragma omp parallel shared(img_1d,input,j,mfccL) num_threads(CORE_NUM)
#endif
					{
#ifdef MULTICORES
#pragma omp for schedule(static)
#endif
						for(k=0;k<NUM_MFCC_FEATURES; k++)
						{
							//short int img_1d_temp = img_1d[k];
							input->memFixpoint[k*input->Y+j-mfccL]= img_1d[k]; //Q16 (overflow for fixpoint 16)  int 
							//printf("%d\n",img_1d[k]);
						}                                                       //Q13 (overflow for fixpoint 13) short int
					}
				}
				//printf("MFCC successfully! \n");
				//int imin= 2147483647, imax= -2147483648;
				//int imin= 32767, imax= -32768;
				//int imin= -22517, imax= 15902;
				//int imin= -28359, imax= 20031;   //for Q16
				int imin= -6063, imax= 4286;   //for Q13
				int irange;
				irange = imax - imin;
				//int imin= -14173, imax = 10018; //for Q13
				//int imin= -21928, imax = 15498; //for Q13 for FFT256

#ifndef PERF
				rt_perf_t *perf2;
				perf2 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
				rt_perf_init(perf2);
				rt_perf_conf(perf2, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));

				rt_perf_reset(perf2);
				rt_perf_start(perf2);
#endif          
#ifdef MULTICORES
#pragma omp parallel shared(input, mfccL, mfccR,imax,irange) num_threads(CORE_NUM)
#endif
				{
#ifdef MULTICORES
#pragma omp for schedule(static) private(j)
#endif
					for(i = 0;i < NUM_MFCC_FEATURES; i++)
					{
						for(j = 0;j < mfccR- mfccL; j++)
						{
							//int index_temp = i*input->Y + j;
							//short int input_temp = FP2FIX((1.0-(float)(input->memFixpoint[i*input->Y + j - mfccL]-imin)/(float) irange)*255.0, 20);
#ifdef INT_INPUT
							input->memFixpoint[i*input->Y + j ] = ((imax - input->memFixpoint[i*input->Y + j ])*255)/irange;
							//printf("input[i] : %d\n",input->memFixpoint[i*input->Y + j ]);
#else
							input->memFixpoint[i*input->Y + j ] = FP2FIX(((float)(imax - input->memFixpoint[i*input->Y + j ])/(float) irange)*255.0, 20);
#endif
						}
					}
				}
				//j = 0;
				//	for(i = 0;i < NUM_MFCC_FEATURES; i++)
				//	{
				//			printf("input [%d] : %d\n", i, input->memFixpoint[i*input->Y + j ]);
				//	}

#ifndef PERF
				rt_perf_stop(perf2);
				rt_perf_save(perf2);
				int instr2 = rt_perf_get(perf2, RT_PERF_INSTR);
				int cycles2 = rt_perf_get(perf2, RT_PERF_CYCLES);
				//int active_cycles = rt_perf_get(perf, RT_PERF_ACTIVE_CYCLES);
				//unsigned int time_us = rt_time_get_us();
				printf("MFCC instr : %d\n", instr2);
				printf("MFCC cycles : %d\n", cycles2);
#endif
				//rt_free(ALLOC_LOCATION, Frame, WIN_LENGTH * sizeof(short int));
				//rt_user_alloc_dump(rt_alloc_l1(0));
				rt_free(ALLOC_LOCATION, img_1d, NUM_MFCC_FEATURES * sizeof(short int));
				free_MFCC_parameter();      
			}
#else

#ifdef MULTICORES
#pragma omp parallel
			{
#pragma omp for private(j)
#endif
				for(i = 0; i < MFCC_BINS; i++){
					for(j = mfccL; j < mfccR; j++)
					{
#ifdef INT_INPUT
						input->memFixpoint[i*input->Y + j - mfccL] = (int32_t) mfcc[i][j];
#else
						input->memFixpoint[i*input->Y + j - mfccL] = mfcc[i][j];
#endif
					}
				}
#ifdef MULTICORES
			}
#endif
#endif

#else
#ifdef DEBUG
			printf("Floating point input \n");
#endif
			// Read input 
			input = tensor_allocate(TENSOR_FLOAT, MFCC_BINS, mfccR-mfccL, 1);
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
			}
#endif
#endif

#ifdef DEBUG
			printf("forward start \n");
#endif
			/* ==================================================================================
			 * BNNs Inference here.
			 * ================================================================================*/
			model_forward(input, invalidL, invalidR, predict);
		//for(i = 0; i < N_ClASSES; i++)
		//{
		//	myPrintf(predict[i], i);
		//}
#ifdef DEBUG
			printf("forward done\n");
#endif

			iter++;
		}
		/*-------------------------------------
		 *  performence stop
		 *--------------------------------------*/
		//  AveragePool 
		float sum = 0;
#ifdef MULTICORES
#pragma omp parallel num_threads(CORE_NUM)
		{
#pragma omp for schedule(static)
#endif
			for(i = 0; i < N_ClASSES; i++)
			{
				predict[i] = predict[i]/1600; //1600 = outputX x outputY
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
				//predict[i] = logf(sum*expf(predict[i]));
				predict[i] = logf(sum) + predict[i];
			}
#ifdef MULTICORES
		}
#endif

#ifdef LED
		led_toggle();
#endif
#ifdef PERF
//		rt_perf_stop(perf);
#endif
		// Print result 
//		for(i = 0; i < N_ClASSES; i++)
//		{
//			myPrintf(predict[i], i, class_name);
//		}
	float top_1, top_5[5],top_5_acc;
	int32_t index_5[5];
		for(i = 0; i < 5; i++){
			top_5[i] = -1000.2;
			index_5[i] = 0;
		}
		float maximum = -1000.2;
		short int index;
		for(i = 0; i < N_ClASSES; i++){
			if(predict[i]>maximum)
			{
				maximum = predict[i];
				index = i;
			}
			if(predict[i]>top_5[4])
			{
				if(predict[i]>top_5[3])
				{
					if(predict[i]>top_5[2])
					{
						if(predict[i]>top_5[1])
						{
							if(predict[i]>top_5[0])
							{
								top_5[4] = top_5[3];
								top_5[3] = top_5[2];
								top_5[2] = top_5[1];
								top_5[1] = top_5[0];
								top_5[0] = predict[i];
								index_5[4] = index_5[3];
								index_5[3] = index_5[2];
								index_5[2] = index_5[1];
								index_5[1] = index_5[0];
								index_5[0] = i;
							}
							else{
							top_5[4] = top_5[3];
							top_5[3] = top_5[2];
							top_5[2] = top_5[1];
							top_5[1] = predict[i];
							index_5[4] = index_5[3];
							index_5[3] = index_5[2];
							index_5[2] = index_5[1];
							index_5[1] = i;
							}
						}else{
						top_5[4] = top_5[3];
						top_5[3] = top_5[2];
						top_5[2] = predict[i];
						index_5[4] = index_5[3];
						index_5[3] = index_5[2];
						index_5[2] = i;
						}
					}else{
					top_5[4] = top_5[3];
					top_5[3] = predict[i];
					index_5[4] = index_5[3];
					index_5[3] = i;
					}
				}else{
				top_5[4] = predict[i];
				index_5[4] = i;
				}
			}
		}				
		for(i = 0; i < 5; i++){
			printf("top: %d : %s ", i+1,class_name[index_5[i]]);
			myPrintf(top_5[i], i, class_name);

		}
		printf("\n");
#ifdef PERF
//		rt_perf_save(perf);
//		int instr = rt_perf_get(perf, RT_PERF_INSTR);
//		int cycles = rt_perf_get(perf, RT_PERF_CYCLES);
		//int active_cycles = rt_perf_get(perf, RT_PERF_ACTIVE_CYCLES);
		//unsigned int time_us = rt_time_get_us();
		//printf("Tile size :%d \n",SUBSEC_SIZE);
//		printf("instr : %d\n", instr);
//		printf("cycles : %d\n", cycles);
		//printf("active : %d\n", active_cycles);
		//printf("time : %u us\n", time_us);
#endif

		rt_free(ALLOC_LOCATION, predict, N_ClASSES*sizeof(float));

//#ifdef MULTICORES
//#pragma omp parallel for schedule(static) //num_threads(8)
//#endif
//		for(i = 0; i < N_ClASSES; i++)
//		{
//			predict[i] = 0.0;
//		}
//
#ifdef LED
		led_toggle();
#endif
	model_free(N_ClASSES);

//	rt_user_alloc_dump(rt_alloc_l1(0));

#ifdef DMA_TRANS    
//		printf("DMA activ \n");
#else
		printf("MCU activ \n");
#endif
//		printf("run successfull!!! \n");

}
