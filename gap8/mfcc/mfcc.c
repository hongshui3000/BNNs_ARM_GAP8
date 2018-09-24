
/* ==================================================================================
 * Includes
 * ================================================================================*/
#include "mfcc.h"
#include "../config.h"

static short int* fbank_first;
static short int* fbank_last;
static short int* frame;
static short int* dct_matrix;
static  int** Mel_fbank;
static short int* mel_energies;
static short int* Hammingwindow;
static short int* TwiddlesL1;
static short int* SwapL1;
static v2s*  Buffer;
/*dma identifier*/
static rt_dma_copy_t		dma_cop1;
static rt_dma_copy_t		dma_cop2;
static rt_dma_copy_t		dma_cop3;
static rt_dma_copy_t		dma_cop4;
static rt_dma_copy_t		dma_cop5;
static rt_dma_copy_t		dma_cop6;

extern void MFCC_init(){
	frame = rt_alloc(ALLOC_LOCATION,WIN_LENGTH*sizeof(short int));
	Buffer = rt_alloc(ALLOC_LOCATION, (WIN_LENGTH+4)*sizeof(v2s));
	//	size_filter_bank = rt_alloc(ALLOC_LOCATION,NUM_FBANK_BINS*sizeof(int));
	mel_energies = rt_alloc(ALLOC_LOCATION,NUM_FBANK_BINS*sizeof(short int));
	fbank_first =  rt_alloc(ALLOC_LOCATION,NUM_FBANK_BINS*sizeof(short int));
	fbank_last =   rt_alloc(ALLOC_LOCATION,NUM_FBANK_BINS*sizeof(short int));
	dct_matrix =   rt_alloc(ALLOC_LOCATION, NUM_MFCC_FEATURES*NUM_FBANK_BINS*sizeof(short int));
	Hammingwindow = rt_alloc(ALLOC_LOCATION, WIN_LENGTH*sizeof(short int));
	SwapL1 =       rt_alloc(ALLOC_LOCATION, WIN_LENGTH*sizeof(short int));
	Mel_fbank =    rt_alloc(ALLOC_LOCATION, NUM_FBANK_BINS*sizeof( int)); 
	TwiddlesL1 =   rt_alloc(ALLOC_LOCATION, WIN_LENGTH*sizeof(short int));

	{
		rt_dma_memcpy((uint32_t) HammingLUT, (uint32_t) Hammingwindow, WIN_LENGTH * sizeof(short int), RT_DMA_DIR_EXT2LOC, 0, &dma_cop1);
		rt_dma_memcpy((uint32_t) TwiddlesLUT, (uint32_t) TwiddlesL1, WIN_LENGTH*sizeof(short int), RT_DMA_DIR_EXT2LOC, 0, &dma_cop2);
		rt_dma_memcpy((uint32_t) SwapLUT, (uint32_t) SwapL1, WIN_LENGTH*sizeof(short int), RT_DMA_DIR_EXT2LOC, 0, &dma_cop3);
		rt_dma_memcpy((uint32_t) fbank_filter_first, (uint32_t) fbank_first, NUM_FBANK_BINS*sizeof(short int), RT_DMA_DIR_EXT2LOC, 0, &dma_cop4);
		rt_dma_memcpy((uint32_t) fbank_filter_last, (uint32_t) fbank_last, NUM_FBANK_BINS*sizeof(short int), RT_DMA_DIR_EXT2LOC, 0, &dma_cop5);
		rt_dma_memcpy((uint32_t) Dct_table, (uint32_t) dct_matrix, NUM_MFCC_FEATURES*NUM_FBANK_BINS*sizeof(short int), RT_DMA_DIR_EXT2LOC, 0, &dma_cop6);

		for (int bin = 0; bin < NUM_FBANK_BINS; bin++) {
			Mel_fbank[bin] = ( int*) rt_alloc(ALLOC_LOCATION,(fbank_filter_last[bin] - fbank_filter_first[bin] + 1)*sizeof( int));//Q13
			int j = 0;
			for (int i = fbank_filter_first[bin]; i <= fbank_filter_last[bin]; ++i) {
				Mel_fbank[bin][j] = mel_fbank[bin][j]; //Q13, Q13 (?) >> Q26 (risk overflow)
				j = j + 1;
			}
		}
	}

}
#ifdef FLOAT_INPUT
static inline void read_fromL2(short int * __restrict__ frame, float * __restrict__ audio_data)
#else
static inline void read_fromL2(short int * __restrict__ frame, short int * __restrict__ audio_data)
#endif
{
	int i;
		#ifdef MULTICORES
		rt_omp_start();
		#endif	
#ifdef MULTICORES
#pragma omp parallel for schedule(static) shared(frame, audio_data) num_threads(CORE_NUM)
#endif
	for (i = 0; i < WIN_LENGTH; i++) {
#ifdef FLOAT_INPUT
		frame[i] = (short int) (audio_data[i]*(1<<15)); //works

#else
		frame[i] = (short int) audio_data[i];	//Input Q15 format
#endif

	}

}
static inline void WindowedFrame(short int *__restrict__ Frame, v2s *__restrict__ OutFrame, short int *__restrict__ Window)
{
	unsigned int i;
		#ifdef MULTICORES
		rt_omp_start();
		#endif

#ifdef MULTICORES
#pragma omp parallel for schedule(static) shared(OutFrame, Window,Frame) num_threads(CORE_NUM)
#endif
	for (i=0; i<WIN_LENGTH; i++) {
		/* Frame and Window are Q15, we want OutFrame with precision FFT2_SAMPLE_DYN */
#if WIN_LENGTH == 512
		OutFrame[i] = (v2s) {(gap8_mulsRN(Frame[i], Window[i], 15))>>2, 0}; //Q13
#else
		OutFrame[i] = (v2s) {(gap8_mulsRN(Frame[i], Window[i], 15))>>3, 0}; //Q12
#endif
	}
}

extern void free_MFCC_parameter(){
	rt_free(ALLOC_LOCATION, frame, WIN_LENGTH*sizeof(short int));
	rt_free(ALLOC_LOCATION, Buffer, (WIN_LENGTH+4)*sizeof(v2s));
	rt_free(ALLOC_LOCATION, dct_matrix, NUM_MFCC_FEATURES*NUM_FBANK_BINS*sizeof(short int));
	rt_free(ALLOC_LOCATION, Hammingwindow, WIN_LENGTH*sizeof(short int));
	rt_free(ALLOC_LOCATION, TwiddlesL1, WIN_LENGTH*sizeof(short int));
	rt_free(ALLOC_LOCATION, SwapL1, WIN_LENGTH*sizeof(short int));
	rt_free(ALLOC_LOCATION, fbank_first, NUM_FBANK_BINS*sizeof(short int));
	rt_free(ALLOC_LOCATION, fbank_last, NUM_FBANK_BINS*sizeof(short int));
	rt_free(ALLOC_LOCATION, mel_energies, NUM_FBANK_BINS*sizeof(short int));
	for (int i = 0; i < NUM_FBANK_BINS; i++)
	{
		rt_free(ALLOC_LOCATION, Mel_fbank[i], (fbank_filter_last[i] - fbank_filter_first[i] + 1)*sizeof( int));
	}
	rt_free(ALLOC_LOCATION, Mel_fbank, NUM_FBANK_BINS*sizeof( int));

}

//Q13 for FFT input parallel version

static inline void Radix2FFT_DIF_Par1(short int *__restrict__ Data, short int *__restrict__ Twiddles, int N_FFT)
{
	v2s *CoeffV = (v2s *) Twiddles;
	v2s *DataV  = (v2s *) Data;
	v2s  W, Tmp;
	unsigned int iCnt1, iCnt2, iCnt3,
		     iQ,    iL,    iM,
		     iA,    iB;
	unsigned int CoreId;
	unsigned int First, Last, Chunk;
#pragma omp parallel default(shared) private(iCnt1, iCnt2, iCnt3, iQ, iL, iM, iA, iB, First, Last, Chunk, CoreId, W, Tmp)// num_threads(8)
	{
		//CoreId = gap8_coreid();//omp_get_thread_num()
		//CoreId = omp_get_thread_num();
		CoreId = omp_get_thread_num();
		//    printf("core id %d \n", CoreId);
		iL = 1;
		iM = N_FFT2 / 2;
		for (iCnt1 = 0; iCnt1 < 6; iCnt1++) {
			//Chunk = (iM/gap8_ncore()); //omp_get_num_threads
			//printf("num_threads =  %d \n", omp_get_num_threads());
			Chunk = iM/omp_get_num_threads();
			First =  CoreId * Chunk; 
			Last  =  First  + Chunk;
			iQ    =  First  * iL;
			//iQ = 0;
			// for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {   //iCnt1 =   0,   1,  2,  3,  4, 5,
			for (iCnt2 = First; iCnt2 < Last; iCnt2++) {  // iM = 256, 128, 64, 32, 16, 8,...
				W = CoeffV[iQ];
				//v2s A = DataV[iCnt2];
				//v2s B = DataV[iCnt2+iM];
				iA = iCnt2;
				for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {// iL = 1, 2, 4, 8, 16, 32
					//v2s Tmp2;
					iB = iA + iM;
					Tmp       = (DataV[iA] - DataV[iB]);
					DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
					//DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {1, 1};
					//Tmp       = (A - B);
					//DataV[iA] = (A + B) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
					//A = DataV[iA + 2*iM]; B = DataV[iA + 3*iM];
					DataV[iB] = (v2s) gap8_cplxmulsdiv2(Tmp, W);
					iA = iA + 2 * iM;
				}
				iQ = iQ + iL;
			}
#pragma omp barrier
			iL <<= 1;
			iM >>= 1;
		}
		//}
		//#pragma omp parallel shared(DataV, CoeffV) private(iCnt1, iCnt2, iCnt3, iQ, iL, iM, iA, iB, First, Last, Chunk, CoreId, W, Tmp)
		//{

		//	CoreId = gap8_coreid();
		//iL = (1<<6);
		//iM = N_FFT2 / (1<<7);
		for (iCnt1 = 6; iCnt1 < 8; iCnt1++) {    
			iQ = 0;
			for (iCnt2 = 0; iCnt2 < iM; iCnt2++) { // iM = 4, 2
				W = CoeffV[iQ];
				//Chunk = (iL/gap8_ncore()); 
				Chunk = iL/omp_get_num_threads();
				First =  CoreId * Chunk; 
				Last  =  First  + Chunk;
				iA    =  iCnt2  + First * 2 * iM;
				//iA = iCnt2 + CoreId*64;
				for (iCnt3 = First; iCnt3 < Last; iCnt3++) {// iL = 64, 128
					//v2s Tmp2;
					iB = iA + iM;
					Tmp       = (v2s) (DataV[iA] - DataV[iB]);
					DataV[iA] = ((DataV[iA] + DataV[iB]) >> (v2s){FFT2_SCALEDOWN, FFT2_SCALEDOWN});
					DataV[iB] = (v2s) gap8_cplxmulsdiv2(Tmp, W);
					iA = iA + 2 * iM;
				}
				iQ = iQ + iL;
#pragma omp barrier
			}
			iL = iL << 1;
			iM = iM >> 1;
		}
		Chunk = (N_FFT2 >> 1)/omp_get_num_threads();
		First =  CoreId * Chunk; 
		Last = Min(First + Chunk, (N_FFT2 >> 1));
		iA = First * 2;
		for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
			iB = iA + 1;
			Tmp = DataV[iA] - DataV[iB];
			DataV[iA] = (DataV[iA] + DataV[iB]);
			DataV[iB] = Tmp;
			iA = iA + 2;
		}
	}			
	}


	//Q13 for FFT input

	static inline void Radix2FFT_DIF_args(short int *__restrict__ Data, short int *__restrict__ Twiddles, int N_FFT)

	{
		int iLog2N  = gap8_fl1(N_FFT2);
		unsigned int iCnt1, iCnt2, iCnt3,
			     iQ,    iL,    iM,
			     iA,    iB;
		v2s *CoeffV = (v2s *) Twiddles;
		v2s *DataV  = (v2s *) Data;
		iL = 1;
		iM = N_FFT2 / 2;

		for (iCnt1 = 0; iCnt1 < (iLog2N-1); iCnt1++) {
			iQ = 0;
			for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
				v2s W = CoeffV[iQ];
				iA = iCnt2;
				for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
					v2s Tmp;
					iB = iA + iM;
					Tmp = DataV[iA] - DataV[iB];
					DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
					DataV[iB] = (v2s) gap8_cplxmulsdiv2(Tmp, W);
					iA = iA + 2 * iM;
				}
				iQ += iL;
			}
			iL <<= 1;
			iM >>= 1;
		}
		iA = 0;
		/* Last Layer: W = (1, 0) */
		for (iCnt3 = 0; iCnt3 < (N_FFT2>>1); iCnt3++) {
			v2s Tmp;
			iB = iA + 1;
			Tmp = DataV[iA] - DataV[iB];
			DataV[iA] = (DataV[iA] + DataV[iB]);
			DataV[iB] = Tmp;
			iA = iA + 2;
		}
	}

static inline void Radix2FFT_DIF_args2(short int *__restrict__ Data, short int *__restrict__ Twiddles, int N_FFT)
{
		v2s *CoeffV = (v2s *) TwiddlesL1;
		v2s *DataV  = (v2s *) Buffer;
		v2s  W, Tmp;
		unsigned int iCnt1, iCnt2, iCnt3,
			     iQ,    iL,    iM,    iA_temp,
			     iA,    iB,    num_thread;
		unsigned int CoreId;
		unsigned int First, Last, Chunk;
{

rt_omp_start();
#pragma omp parallel shared(CoeffV,DataV) private(iCnt1, iCnt2, iCnt3, iQ, iL, iM, iA, iB, iA_temp, num_thread, First, Last, Chunk, CoreId, W, Tmp) num_threads(CORE_NUM)
	{
	iL = 1;
	iM = N_FFT / 2;
	//CoreId = omp_get_thread_num();
	//num_thread = omp_get_num_threads();
	CoreId = rt_core_id();
	num_thread = rt_nb_pe();
	//CoreId = gap8_coreid();
	//num_thread = gap8_ncore();
	//printf("num_thread = %d\n", num_thread);
	//rt_user_alloc_dump(rt_alloc_l1(0));
	for (iCnt1 = 0; iCnt1 < 6; iCnt1++) {
		iQ    =  CoreId * (1<<iCnt1);
		iA = CoreId;											//iCnt1 =   0,   1,  2,  3,  4, 5,...
		for (iCnt2 = 0; iCnt2 < iM; iCnt2 = iCnt2 + num_thread) { // iM = 256, 128, 64, 32, 16, 8,...
			W = CoeffV[iQ];
			iA_temp = iA;
			for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {				// iL = 1, 2, 4, 8
				iB = iA + iM;
				Tmp       = (DataV[iA] - DataV[iB]);
				DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
				DataV[iB] = (v2s) gap8_cplxmulsdiv2(Tmp, W);				
				//printf("iB = %d\n", iB);
				iA = iA + 2 * iM;
				//iA = iA + (iM << 1);
			}
			iA = iA_temp + num_thread;
			iQ = iQ + num_thread * iL;
		}
		#ifdef MULTICORES
		#pragma omp barrier
		#endif
		iL <<= 1;
		iM >>= 1;
	}
	//#ifdef MULTICORES
	//#pragma omp barrier
	//#endif
	for (iCnt1 = 6; iCnt1 < 8; iCnt1++) {
		iQ = 0;
		for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
			W = CoeffV[iQ];
			Chunk = iL/rt_nb_pe();// iL = 16, 32, 64, 128
			//Chunk = iL/omp_get_num_threads();// iL = 16, 32, 64, 128
			//Chunk = iL/gap8_ncore();// iL = 16, 32, 64, 128
			First =  CoreId * Chunk; 
			Last  =  First  + Chunk;
			iA    =  iCnt2  + First * 2 * iM;
			for (iCnt3 = First; iCnt3 < Last; iCnt3++) { //Core0: iA 0~63, Core1: iA 64~128 ....
				//v2s Tmp2;			   //iCnt1 =    4,   5,  6,  7					
				iB = iA + iM;			 // iM =   16,   8,  4,  2
				Tmp       = (v2s) (DataV[iA] - DataV[iB]);
				DataV[iA] = ((DataV[iA] + DataV[iB]) >> (v2s){FFT2_SCALEDOWN, FFT2_SCALEDOWN});
				//printf("iB2 = %d\n", iB);
				DataV[iB] = (v2s) gap8_cplxmulsdiv2(Tmp, W);
				iA = iA + 2 * iM;
				//iA = iA + (iM << 1);
			}
			iQ = iQ + iL;
			#ifdef MULTICORES
			#pragma omp barrier
			#endif
		}
		iL = iL << 1;
		iM = iM >> 1;
		//#pragma omp barrier
	}
	//Chunk = (N_FFT2 >> 1)/omp_get_num_threads();
	Chunk = (N_FFT2 >> 1)/rt_nb_pe();
	//Chunk = (N_FFT2 >> 1)/gap8_ncore();
	First =  CoreId * Chunk; 
	Last = Min(First + Chunk, (N_FFT2 >> 1));
	//Last = First + Chunk;
	iA = First * 2;
	for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
		iB = iA + 1;
		Tmp = DataV[iA] - DataV[iB];
		DataV[iA] = (DataV[iA] + DataV[iB]);
		DataV[iB] = Tmp;
		iA = iA + 2;
	}
	}
}


}

static inline void SwapSamples_args(v2s *__restrict__ Data, short int *__restrict__ SwapTable, int Ni)
{

	int i;
		#ifdef MULTICORES
		rt_omp_start();
		#endif
#ifdef MULTICORES
#pragma omp parallel for schedule(static) shared(SwapTable, Data, Ni) num_threads(CORE_NUM)
#endif
	for (i = 0; i < Ni; i++) {
		v2s S = Data[i];
		//printf("Data[i] : %d\n",Data[i]);
		int SwapIndex = SwapTable[i];
		if (i < SwapIndex) {
			Data[i] = Data[SwapIndex]; 
			Data[SwapIndex] = S;
		}
		//printf("%d\n",i);
	}

}

static inline void FilterBank(v2s *__restrict__  Buffer, short int *__restrict__  mel_energies)
{
int32_t i, j, bin;
unsigned int* FramePower = ( unsigned int *) Buffer; 
short int sqrt_data;
#ifdef MULTICORES
rt_omp_start();
#endif
#ifdef MULTICORES
#pragma omp parallel for schedule(dynamic) shared(FramePower,fbank_first, fbank_last, mel_energies) private(bin,i,j,sqrt_data) num_threads(CORE_NUM_SPEC)
#endif
		for (bin = 0; bin < NUM_FBANK_BINS; bin++) {
			j = 0;
			short int mel_energy = 0;
			short int first_index = fbank_first[bin];
			short int last_index = fbank_last[bin];
			for (i = first_index; i <= last_index; ++i) {

#ifndef PERF

				rt_perf_t *perf1;
				perf1 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
				rt_perf_init(perf1);
				rt_perf_conf(perf1, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));

				rt_perf_reset(perf1);
				rt_perf_start(perf1);
#endif
				sqrt_data = (short int) isqrt((unsigned int) FramePower[i]); //Radix2 : Q26 >> Q13 , Radix4: Q24>> Q12

#ifndef PERF
				rt_perf_stop(perf1);
				rt_perf_save(perf1);
				int instr = rt_perf_get(perf1, RT_PERF_INSTR);
				int cycles = rt_perf_get(perf1, RT_PERF_CYCLES);
				//int active_cycles = rt_perf_get(perf, RT_PERF_ACTIVE_CYCLES);
				//unsigned int time_us = rt_time_get_us();
				//printf("SQRT instr  : %d\n", instr);
				printf("SQRT cycles : %d\n", cycles);
#endif

#if WIN_LENGTH == 512
				mel_energy += gap8_mulsRN(sqrt_data, Mel_fbank[bin][j++], 13); //Radix2: Q13, Q13 (?) >> Q26>>13 = Q13 (risk overflow)
#else
				mel_energy += gap8_mulsRN(sqrt_data, Mel_fbank[bin][j++], 12); //Radix4: Q12, Q13 (?) >> Q25>>12 = Q13 (risk overflow)
#endif
			}
			mel_energies[bin] = mel_energy;
			//avoid log of zero
			if (mel_energy <= 0)
			{
				mel_energies[bin] = 1;  //Q12, 13resolution
			}

		}

}


static inline void EstimatePower_int(v2s *__restrict__ FrameIn,  int N)
{
	int i;
	int* Power_int = ( int*) FrameIn;
		#ifdef MULTICORES
		rt_omp_start();
		#endif
#ifdef HIGH_PASS
#ifdef MULTICORES
#pragma omp parallel for schedule(static) shared(Power_int,FrameIn,N) num_threads(CORE_NUM)
#endif
	for (i=0; i<4; i++) {
		Power_int[i] = i*i; // gap8_mulsRN(P, INVSQRT2, FFT2_SAMPLE_DYN);
	}
#endif
#ifdef MULTICORES
#pragma omp parallel for schedule(static) shared(Power_int,FrameIn,N) num_threads(CORE_NUM)
#endif
	#ifdef HIGH_PASS
	for (i=4; i<N; i++) {
		int P = gap8_dotp2(FrameIn[i]*2, FrameIn[i]*2);
		Power_int[i] = P; // gap8_mulsRN(P, INVSQRT2, FFT2_SAMPLE_DYN);
	}
	#else
	for (i=0; i<N; i++) {
		int P = gap8_dotp2(FrameIn[i], FrameIn[i]);
		Power_int[i] = P; // gap8_mulsRN(P, INVSQRT2, FFT2_SAMPLE_DYN);
	}
    #endif
}

static void DCT(short int* __restrict__ mfcc_out){
		short int sum;
		int i, j;
		#ifdef MULTICORES
		rt_omp_start();
		#endif
#ifdef MULTICORES
#pragma omp parallel for schedule(static) shared(mfcc_out, dct_matrix, mel_energies) private(j,sum) num_threads(CORE_NUM)
#endif
			for (i = 0; i < NUM_MFCC_FEATURES; i++) {
				sum = 0;
				for (j = 0; j < NUM_FBANK_BINS; j++) {
					//k = i * NUM_FBANK_BINS + j;
#if WIN_LENGTH == 512
#ifndef PERF

					rt_perf_t *perf1;
					perf1 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
					rt_perf_init(perf1);
					rt_perf_conf(perf1, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));

					rt_perf_reset(perf1);
					rt_perf_start(perf1);
#endif
					sum  += gap8_mulsRN(dct_matrix[i * NUM_FBANK_BINS + j ], mel_energies[j], 13);//Radix2: Q13 * Q13 --> Q26>>13 == Q13,  30~70 cycles, while ARM only 1 cycles float multiplication
					//sum  += __builtin_pulp_mulsRN(dct_matrix[k ], mel_energies[j], 13, 1<<12);//Radix2: Q13 * Q13 --> Q26>>13 == Q13
					//sum  += (short int) (dct_matrix[i * NUM_FBANK_BINS + j]) * (short int) (mel_energies[j]) + (1<<(12))>>(13);
#ifndef PERF
					rt_perf_stop(perf1);
					rt_perf_save(perf1);
					int dctinstr = rt_perf_get(perf1, RT_PERF_INSTR);
					int dctcycles = rt_perf_get(perf1, RT_PERF_CYCLES);
					//int active_cycles = rt_perf_get(perf, RT_PERF_ACTIVE_CYCLES);
					//unsigned int time_us = rt_time_get_us();
					//printf("DCT instr  : %d\n", dctinstr);
					printf("DCT cycles : %d\n", dctcycles);
#endif

#else
					sum += gap8_mulsRN(dct_matrix[i * NUM_FBANK_BINS + j], mel_energies[j], 13);//Radix4: Q13 * Q12 --> Q25>>12 == Q13
#endif
				}
				mfcc_out[i] = (short int) sum; //Q13
			}

}


#ifdef FLOAT_INPUT
extern void mfcc_compute(float* __restrict__ audio_data, short int* __restrict__ mfcc_out) {	//normalized input [-1,1]
#else
extern void mfcc_compute(short int* __restrict__ audio_data, short int* __restrict__ mfcc_out) { //Q15 format normalized input [-32768, 32767]
#endif
		int32_t i, j, bin;
		short int k;
		//printf("MFCC: core id %d %d\n", rt_cluster_id(), rt_core_id());

		//TensorFlow way of normalizing .wav data to (-1,1)
#ifndef PERF

		rt_perf_t *perf1;
		perf1 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
		rt_perf_init(perf1);
		rt_perf_conf(perf1, (1<<RT_PERF_CYCLES));

		rt_perf_reset(perf1);
		rt_perf_start(perf1);
#endif
		read_fromL2(frame, audio_data);
		rt_dma_wait(&dma_cop1);

#ifndef PERF
		rt_perf_stop(perf1);
		rt_perf_save(perf1);
		int wiminstr = rt_perf_get(perf1, RT_PERF_INSTR);
		int wimcycles = rt_perf_get(perf1, RT_PERF_CYCLES);
		printf("read from L2 cycles : %d\n", wimcycles);
#endif

#ifndef PERF

		rt_perf_t *perf1;
		perf1 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
		rt_perf_init(perf1);
		rt_perf_conf(perf1, (1<<RT_PERF_CYCLES));

		rt_perf_reset(perf1);
		rt_perf_start(perf1);
#endif

		WindowedFrame(frame, Buffer, Hammingwindow);	//Q13
		//Compute FFT

#ifndef PERF
		rt_perf_stop(perf1);
		rt_perf_save(perf1);
		int wiminstr = rt_perf_get(perf1, RT_PERF_INSTR);
		int wimcycles = rt_perf_get(perf1, RT_PERF_CYCLES);
		printf("win L2 cycles : %d\n", wimcycles);
#endif
		rt_dma_wait(&dma_cop2);//loading Twiddle Factor

#ifndef PERF

		rt_perf_t *perf2;
		perf2 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
		rt_perf_init(perf2);
		rt_perf_conf(perf2, (1<<RT_PERF_CYCLES) );

		rt_perf_reset(perf2);
		rt_perf_start(perf2);
#endif

#ifdef MULTICORES
	//	Radix2FFT_DIF_Par1((short int*)Buffer,TwiddlesL1,WIN_LENGTH); //input Q13 output  Q13, parallel version
		Radix2FFT_DIF_args2((short int*)Buffer,TwiddlesL1,WIN_LENGTH); //input Q13 output  Q13
#else
		Radix2FFT_DIF_args((short int*)Buffer,TwiddlesL1,WIN_LENGTH); //input Q13 output  Q13
#endif

#ifndef PERF
		rt_perf_stop(perf2);
		rt_perf_save(perf2);
		int fftinstr = rt_perf_get(perf2, RT_PERF_INSTR);
		int fftcycles = rt_perf_get(perf2, RT_PERF_CYCLES);
		printf("FFT Par cycles : %d\n", fftcycles);
#endif
		rt_dma_wait(&dma_cop3);//swapTable

		SwapSamples_args(Buffer,SwapL1,WIN_LENGTH); 	//Just reorder, precision same, Q13
		//Convert to power spectrum
		int32_t half_dim = WIN_LENGTH / 2 +1;

#ifndef PERF

		rt_perf_t *perf1;
		perf1 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
		rt_perf_init(perf1);
		rt_perf_conf(perf1, (1<<RT_PERF_CYCLES));

		rt_perf_reset(perf1);
		rt_perf_start(perf1);
#endif
		EstimatePower_int(Buffer, half_dim);// Radix2: Q13 -->Q26,  Radix4: Q12 --> Q24

#ifndef PERF
		rt_perf_stop(perf1);
		rt_perf_save(perf1);
		int pwinstr = rt_perf_get(perf1, RT_PERF_INSTR);
		int pwcycles = rt_perf_get(perf1, RT_PERF_CYCLES);
		printf("cycles : %d\n", pwcycles);
#endif
		//Apply mel filterbanks
		rt_dma_wait(&dma_cop4);
		rt_dma_wait(&dma_cop5);
#ifndef PERF

		rt_perf_t *perf3;
		perf3 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
		rt_perf_init(perf3);
		rt_perf_conf(perf3, (1<<RT_PERF_CYCLES));

		rt_perf_reset(perf3);
		rt_perf_start(perf3);
#endif

		FilterBank(Buffer, mel_energies);

#ifndef PERF
		rt_perf_stop(perf3);
		rt_perf_save(perf3);
		int filinstr = rt_perf_get(perf3, RT_PERF_INSTR);
		int filcycles = rt_perf_get(perf3, RT_PERF_CYCLES);
		printf("Filters cycles : %d\n", filcycles);
#endif

#ifndef PERF

		rt_perf_t *perf1;
		perf1 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
		rt_perf_init(perf1);
		rt_perf_conf(perf1, (1<<RT_PERF_CYCLES));

		rt_perf_reset(perf1);
		rt_perf_start(perf1);
#endif
		//Take DCT. Uses matrix mul.
		//int64_t sum;
		rt_dma_wait(&dma_cop6);
		//#pragma omp parallel shared(mfcc_out, dct_matrix, mel_energies) //private(j)
		
		DCT(mfcc_out);

#ifndef PERF
		rt_perf_stop(perf1);
		rt_perf_save(perf1);
		int dctinstr = rt_perf_get(perf1, RT_PERF_INSTR);
		int dctcycles = rt_perf_get(perf1, RT_PERF_CYCLES);
		printf("DCT cycles : %d\n", dctcycles);
#endif
	}
	short int isqrt(unsigned int a) {

		int g = 0x8000;
		int c = 0x8000;
		for (;;) {
			if (g*g > a) {
				g ^= c;
			}
			c >>= 1;
			if (c == 0) {
				return g;
			}
			g |= c;
		}
	}
