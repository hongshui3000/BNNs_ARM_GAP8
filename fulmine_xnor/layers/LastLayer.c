/**
 * \file FirstLayer.c
 * \brief
 * \date 18.01.2017
 * 
 * \details 
 */ 

/**
 * \defgroup Layer <DESCRIPTION>
 * [\addtogroup] Layer
 * @{
 */

/* ==================================================================================
 * Includes
 * ================================================================================*/
#include "LastLayer.h"

/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/
/* ==================================================================================
 * Local/Global variables definitions
 * ================================================================================*/
/* ==================================================================================
 * Local function declarations
 * ================================================================================*/
#ifdef FIXPOINT_16BIT
static inline void lastLayer_convolution_16bitFixPoint(
	TENSOR* input,
	LAST_FEATURES* l, 
	uint32_t invalidL,
	uint32_t invalidR,
	double* predict
);
#elif defined(FIXPOINT_32BIT)
static inline void lastLayer_convolution_32bitFixPoint(
	TENSOR* input,
	LAST_FEATURES* l, 
	uint32_t invalidL,
	uint32_t invalidR,
	double* predict
);
#else
static inline void lastLayer_convolution_float(
	TENSOR* input,
	LAST_FEATURES* l, 
	uint32_t invalidL,
	uint32_t invalidR,
	double* predict
);
#endif

void enrolled_prediction(LAST_FEATURES* l,  float* inter, double* predict);

float enrolled_convolution(LAST_FEATURES* l,  float* inter, int k);

/* ==================================================================================
 * Local function implementations
 * ================================================================================*/
/* ==================================================================================
 * Global function implementations
 * ================================================================================*/
extern void lastLayer_setConfiguration(void)
{
	/*empty*/
}

extern void lastLayer_convolution
(
	TENSOR* input,
	LAST_FEATURES* l, 
	uint32_t invalidL,
	uint32_t invalidR,
	double* predict
)
{


#ifdef FIXPOINT_16BIT
	lastLayer_convolution_16bitFixPoint(input, l, invalidL, invalidR, predict);
#elif defined(FIXPOINT_32BIT)
	lastLayer_convolution_32bitFixPoint(input, l, invalidL, invalidR, predict);
#else
	lastLayer_convolution_float(input, l, invalidL, invalidR, predict);
#endif	

}


#ifdef FIXPOINT_16BIT
static inline void lastLayer_convolution_16bitFixPoint
(
	TENSOR* input,
	LAST_FEATURES* l, 
	uint32_t invalidL,
	uint32_t invalidR,
	double* predict
)
{

    #ifdef MULTICORES	
    #pragma omp parallel \
    	shared(input, predict) \
    	shared(l) \
    	shared(invalidR, invalidL)
    {
    #endif	
	    uint32_t i, j, k, kk;
	    int32_t sum1 = 0;
	    int32_t sum2 = 0;
	    int32_t inter[l->nFilters1];
	    float temp;

	#ifdef MULTICORES
    #pragma omp	for schedule(static) collapse(2)
	#endif
	for(i = 0; i < input->X; i++){
	for(j = invalidL; j < input->Y - invalidR; j++)
	{
		{
			for(k = 0; k < l->nFilters1; k++)
			{
				sum1 = 16 * input->Z;
				/* binary convolution */
				for(kk = 0; kk < input->Z; kk++)
				{
				    sum1 -= popcount(l->weight1[k * input->Z + kk] ^ 
							input->memBin[(i * input->Y + j)*input->Z + kk]);
				}
				sum1 *= 2;

				/* Store intermediary results */
				inter[k] = (0.0 > sum1 * l->alpha1[k] + l->bias1[k]) ? 0.0: (sum1 * l->alpha1[k] + l->bias1[k]);

				inter[k] = ((((inter[k] - l->mean[k]) * l->std[k] >> FP_SCALE_BITs) * l->gamma[k]) >> FP_SCALE_BITs) + l->beta[k];	
			}
		}

	    /* Calculate the full-point layer */
	    for(k = 0; k < l->nFilters2; k++)
	    {
	    	sum2 = 0;
			/* convolution */

			for(kk = 0; kk < l->nFilters1; kk++)
			{
		    	sum2 += (l->weight2[k * l->nFilters1 + kk] * inter[kk]) >> FP_SCALE_BITs;
			}

			temp = ((float)(sum2 + l->bias2[k])) / (float) FP_SCALE;
	    	
	    	#ifdef MULTICORES
	    	#pragma omp critical
	    	#endif	  
	    	{
		    	predict[k] += temp;	    		
	    	}  	
		} 
	}}
	#ifdef MULTICORES
	}
	#endif
}
#elif defined(FIXPOINT_32BIT)
static inline void lastLayer_convolution_32bitFixPoint
(
	TENSOR* input,
	LAST_FEATURES* l, 
	uint32_t invalidL,
	uint32_t invalidR,
	double* predict
)
{
    #ifdef MULTICORES	
    #pragma omp parallel \
    	shared(input, predict) \
    	shared(l) \
    	shared(invalidR, invalidL)
    {
    #endif	
	    uint32_t i, j, k, kk;
	    int32_t sum1 = 0;
	    int64_t sum2 = 0;
	    int32_t inter[l->nFilters1];
	    float temp;

	#ifdef MULTICORES
    #pragma omp	for schedule(static) collapse(2)
	#endif
	for(i = 0; i < input->X; i++){
	for(j = invalidL; j < input->Y - invalidR; j++)
	{

#ifdef UNFOLDED
		/* input z is always 4 */
		{
			for(k = 0; k < l->nFilters1; k++)
			{

				sum1 = 64;
				sum1 -= popcount(l->weight1[k * 4 + 0] ^ input->memBin[(i * input->Y + j)*4 + 0])
					 + popcount(l->weight1[k * 4 + 1] ^ input->memBin[(i * input->Y + j)*4 + 1])
					 + popcount(l->weight1[k * 4 + 2] ^ input->memBin[(i * input->Y + j)*4 + 2])
					 + popcount(l->weight1[k * 4 + 3] ^ input->memBin[(i * input->Y + j)*4 + 3]);
				sum1 *= 2;
				
				inter[k] = (0 > (sum1 * l->alpha1[k])  + l->bias1[k]) ? 0.0: ((sum1 * l->alpha1[k]) >> FP_SCALE_BITs)  + l->bias1[k];
				inter[k] = (((((inter[k] - l->mean[k]) * l->std[k]) >> FP_SCALE_BITs) * l->gamma[k]) >> FP_SCALE_BITs) + l->beta[k];	
			}		
		}

		// else
#endif
		{
			for(k = 0; k < l->nFilters1; k++)
			{
				sum1 = 16 * input->Z;
				/* binary convolution */
				for(kk = 0; kk < input->Z; kk++)
				{
				    sum1 -= popcount(l->weight1[k * input->Z + kk] ^ 
							input->memBin[(i * input->Y + j)*input->Z + kk]);
				}
				sum1 *= 2;

				/* Store intermediary results */
				inter[k] = (0 > (sum1 * (int64_t)l->alpha1[k]) + l->bias1[k]) ? 0: (sum1 * (int64_t)l->alpha1[k] + l->bias1[k]);

				inter[k] = (((((inter[k] - l->mean[k]) * (int64_t) l->std[k]) >> FP_SCALE_BITs) * (int64_t)l->gamma[k]) >> FP_SCALE_BITs) + l->beta[k];	
		
			}
		}

	    /* Calculate the full-point layer */
	    for(k = 0; k < l->nFilters2; k++)
	    {
	    	sum2 = 0;
			/* convolution */
			for(kk = 0; kk < l->nFilters1; kk++)
			{
		    	sum2 += ((int64_t)l->weight2[k * l->nFilters1 + kk] * (int64_t)inter[kk]) >> FP_SCALE_BITs;
			}

			temp = ((float)(sum2 + l->bias2[k])) / (float) FP_SCALE;
	    	
	    	#ifdef MULTICORES
	    	#pragma omp critical
	    	#endif	  
	    	{
		    	/* accumulate predition */
		    	predict[k] += temp;	    		
	    	}  	
		} 		
	}}
	#ifdef MULTICORES
	}
	#endif
}
#else
static inline void lastLayer_convolution_float
(
	TENSOR* input,
	LAST_FEATURES* l, 
	uint32_t invalidL,
	uint32_t invalidR,
	double* predict
)
{
    #ifdef MULTICORES	
    #pragma omp parallel \
    	shared(input, predict) \
    	shared(l) \
    	shared(invalidR, invalidL)
    {
    #endif	
	    uint32_t i, j, k, kk;
	    int32_t sum1 = 0;
	    float sum2 = 0;
	    float inter[l->nFilters1];

	#ifdef MULTICORES
    #pragma omp	for schedule(static) collapse(2)
	#endif
	for(i = 0; i < input->X; i++){
	for(j = invalidL; j < input->Y - invalidR; j++)
	{
		/* input z is always 4 */
#ifdef UNFOLDED
		{
			for(k = 0; k < l->nFilters1; k++)
			{

				sum1 = 64;
				sum1 -= popcount(l->weight1[k * 4 + 0] ^ input->memBin[(i * input->Y + j)*4 + 0])
					 + popcount(l->weight1[k * 4 + 1] ^ input->memBin[(i * input->Y + j)*4 + 1])
					 + popcount(l->weight1[k * 4 + 2] ^ input->memBin[(i * input->Y + j)*4 + 2])
					 + popcount(l->weight1[k * 4 + 3] ^ input->memBin[(i * input->Y + j)*4 + 3]);
				sum1 *= 2;
				
				inter[k] = (0.0 > sum1 * l->alpha1[k] + l->bias1[k]) ? 0.0: sum1 * l->alpha1[k] + l->bias1[k];
				inter[k] = (inter[k] - l->mean[k]) * l->std[k] * l->gamma[k] + l->beta[k];	
			}		
		}

#endif
		{
			for(k = 0; k < l->nFilters1; k++)
			{
				sum1 = 16 * input->Z;
				/* binary convolution */
				for(kk = 0; kk < input->Z; kk++)
				{
				    sum1 -= popcount(l->weight1[k * input->Z + kk] ^ 
							input->memBin[(i * input->Y + j)*input->Z + kk]);
				}

				sum1 *= 2;
				
				/* Store intermediary results */
				inter[k] = (0.0 > sum1 * l->alpha1[k] + l->bias1[k]) ? 0.0: sum1 * l->alpha1[k] + l->bias1[k];

				inter[k] = (inter[k] - l->mean[k]) * l->std[k] * l->gamma[k] + l->beta[k];

			}
		}

	    /* Calculate the full-point layer */		
	    for(k = 0; k < l->nFilters2; k++)
	    {
	    	sum2 = 0;
			/* convolution */
			for(kk = 0; kk < l->nFilters1; kk++)
			{
		    	sum2 += l->weight2[k * l->nFilters1 + kk] * inter[kk];
			}

			sum2 += l->bias2[k];
	    	
	    	#ifdef MULTICORES
	    	#pragma omp critical
	    	#endif	  
	    	{
		    	/* accumulate predition */
		    	predict[k] += sum2;	    		
	    	}  	
		} 		
	}}
	#ifdef MULTICORES
	}
	#endif
}
#endif




void enrolled_prediction(LAST_FEATURES* l,  float* inter, double* predict)
{
	float sum; 

	sum = enrolled_convolution(l, inter, 0);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[0] += sum + l->bias2[0];
	}
	sum = enrolled_convolution(l, inter, 1);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[1] += sum + l->bias2[1];
	}
	sum = enrolled_convolution(l, inter, 2);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[2] += sum + l->bias2[2];
	}
	sum = enrolled_convolution(l, inter, 3);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[3] += sum + l->bias2[3];
	}
	sum = enrolled_convolution(l, inter, 4);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[4] += sum + l->bias2[4];
	}
	sum = enrolled_convolution(l, inter, 5);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[5] += sum + l->bias2[5];
	}
	sum = enrolled_convolution(l, inter, 6);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[6] += sum + l->bias2[6];
	}
	sum = enrolled_convolution(l, inter, 7);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[7] += sum + l->bias2[7];
	}
	sum = enrolled_convolution(l, inter, 8);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[8] += sum + l->bias2[8];
	}
	sum = enrolled_convolution(l, inter, 9);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[9] += sum + l->bias2[9];
	}
	sum = enrolled_convolution(l, inter, 10);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[10] += sum + l->bias2[10];
	}
	sum = enrolled_convolution(l, inter, 11);	
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[11] += sum + l->bias2[11];
	}
	sum = enrolled_convolution(l, inter, 12);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[12] += sum + l->bias2[12];
	}
	sum = enrolled_convolution(l, inter, 13);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[13] += sum + l->bias2[13];
	}
	sum = enrolled_convolution(l, inter, 14);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[14] += sum + l->bias2[14];
	}
	sum = enrolled_convolution(l, inter, 15);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[15] += sum + l->bias2[15];
	}
	sum = enrolled_convolution(l, inter, 16);	
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[16] += sum + l->bias2[16];
	}
	sum = enrolled_convolution(l, inter, 17);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[17] += sum + l->bias2[17];
	}
	sum = enrolled_convolution(l, inter, 18);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[18] += sum + l->bias2[18];
	}
	sum = enrolled_convolution(l, inter, 19);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[19] += sum + l->bias2[19];
	}
	sum = enrolled_convolution(l, inter, 20);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[20] += sum + l->bias2[20];
	}
	sum = enrolled_convolution(l, inter, 21);	
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[21] += sum + l->bias2[21];
	}
	sum = enrolled_convolution(l, inter, 22);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[22] += sum + l->bias2[22];
	}
	sum = enrolled_convolution(l, inter, 23);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[23] += sum + l->bias2[23];
	}
	sum = enrolled_convolution(l, inter, 24);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[24] += sum + l->bias2[24];
	}
	sum = enrolled_convolution(l, inter, 25);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[25] += sum + l->bias2[25];
	}
	sum = enrolled_convolution(l, inter, 26);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[26] += sum + l->bias2[26];
	}
	sum = enrolled_convolution(l, inter, 27);
	#ifdef MULTICORES
	#pragma omp critical
	#endif	  
	{
		predict[27] += sum + l->bias2[27];
	}				
}

float enrolled_convolution(LAST_FEATURES* l,  float* inter, int k)
{
	float retVal;

	retVal = l->weight2[k * l->nFilters1 + 0] * inter[0]
		   + l->weight2[k * l->nFilters1 + 1] * inter[1]
		   + l->weight2[k * l->nFilters1 + 2] * inter[2]
		   + l->weight2[k * l->nFilters1 + 3] * inter[3]
		   + l->weight2[k * l->nFilters1 + 4] * inter[4]
		   + l->weight2[k * l->nFilters1 + 5] * inter[5]
		   + l->weight2[k * l->nFilters1 + 6] * inter[6]
		   + l->weight2[k * l->nFilters1 + 7] * inter[7]
		   + l->weight2[k * l->nFilters1 + 8] * inter[8]
		   + l->weight2[k * l->nFilters1 + 9] * inter[9]
		   + l->weight2[k * l->nFilters1 + 10] * inter[10]
		   + l->weight2[k * l->nFilters1 + 11] * inter[11]
		   + l->weight2[k * l->nFilters1 + 12] * inter[12]
		   + l->weight2[k * l->nFilters1 + 12] * inter[12]
		   + l->weight2[k * l->nFilters1 + 13] * inter[13]
		   + l->weight2[k * l->nFilters1 + 14] * inter[14]
		   + l->weight2[k * l->nFilters1 + 15] * inter[15]
		   + l->weight2[k * l->nFilters1 + 16] * inter[16]
		   + l->weight2[k * l->nFilters1 + 17] * inter[17]
		   + l->weight2[k * l->nFilters1 + 18] * inter[18]
		   + l->weight2[k * l->nFilters1 + 19] * inter[19]
		   + l->weight2[k * l->nFilters1 + 20] * inter[20]
		   + l->weight2[k * l->nFilters1 + 21] * inter[21]
		   + l->weight2[k * l->nFilters1 + 22] * inter[22]
		   + l->weight2[k * l->nFilters1 + 23] * inter[23]
		   + l->weight2[k * l->nFilters1 + 24] * inter[24]
		   + l->weight2[k * l->nFilters1 + 25] * inter[25]
		   + l->weight2[k * l->nFilters1 + 26] * inter[26]
		   + l->weight2[k * l->nFilters1 + 27] * inter[27]
		   + l->weight2[k * l->nFilters1 + 28] * inter[28]
		   + l->weight2[k * l->nFilters1 + 29] * inter[29]
		   + l->weight2[k * l->nFilters1 + 30] * inter[30]
		   + l->weight2[k * l->nFilters1 + 31] * inter[31]
		   + l->weight2[k * l->nFilters1 + 32] * inter[32]
		   + l->weight2[k * l->nFilters1 + 33] * inter[33]
		   + l->weight2[k * l->nFilters1 + 34] * inter[34]
		   + l->weight2[k * l->nFilters1 + 35] * inter[35]
		   + l->weight2[k * l->nFilters1 + 36] * inter[36]
		   + l->weight2[k * l->nFilters1 + 37] * inter[37]
		   + l->weight2[k * l->nFilters1 + 38] * inter[38]
		   + l->weight2[k * l->nFilters1 + 39] * inter[39]
		   + l->weight2[k * l->nFilters1 + 40] * inter[40]
		   + l->weight2[k * l->nFilters1 + 41] * inter[41]
		   + l->weight2[k * l->nFilters1 + 42] * inter[42]
		   + l->weight2[k * l->nFilters1 + 43] * inter[43]
		   + l->weight2[k * l->nFilters1 + 44] * inter[44]
		   + l->weight2[k * l->nFilters1 + 45] * inter[45]
		   + l->weight2[k * l->nFilters1 + 46] * inter[46]
		   + l->weight2[k * l->nFilters1 + 47] * inter[47]
		   + l->weight2[k * l->nFilters1 + 48] * inter[48]
		   + l->weight2[k * l->nFilters1 + 49] * inter[49]
		   + l->weight2[k * l->nFilters1 + 50] * inter[50]
		   + l->weight2[k * l->nFilters1 + 51] * inter[51]
		   + l->weight2[k * l->nFilters1 + 52] * inter[52]
		   + l->weight2[k * l->nFilters1 + 53] * inter[53]
		   + l->weight2[k * l->nFilters1 + 54] * inter[54]
		   + l->weight2[k * l->nFilters1 + 55] * inter[55]
		   + l->weight2[k * l->nFilters1 + 56] * inter[56]
		   + l->weight2[k * l->nFilters1 + 57] * inter[57]
		   + l->weight2[k * l->nFilters1 + 58] * inter[58]
		   + l->weight2[k * l->nFilters1 + 59] * inter[59]
		   + l->weight2[k * l->nFilters1 + 60] * inter[60]
		   + l->weight2[k * l->nFilters1 + 61] * inter[61]
		   + l->weight2[k * l->nFilters1 + 62] * inter[62]
		   + l->weight2[k * l->nFilters1 + 63] * inter[63]
		   + l->weight2[k * l->nFilters1 + 64] * inter[64]
		   + l->weight2[k * l->nFilters1 + 65] * inter[65]
		   + l->weight2[k * l->nFilters1 + 66] * inter[66]
		   + l->weight2[k * l->nFilters1 + 67] * inter[67]
		   + l->weight2[k * l->nFilters1 + 68] * inter[68]
		   + l->weight2[k * l->nFilters1 + 69] * inter[69]
		   + l->weight2[k * l->nFilters1 + 70] * inter[70]
		   + l->weight2[k * l->nFilters1 + 71] * inter[71]
		   + l->weight2[k * l->nFilters1 + 72] * inter[72]
		   + l->weight2[k * l->nFilters1 + 73] * inter[73]
		   + l->weight2[k * l->nFilters1 + 74] * inter[74]
		   + l->weight2[k * l->nFilters1 + 75] * inter[75]
		   + l->weight2[k * l->nFilters1 + 76] * inter[76]
		   + l->weight2[k * l->nFilters1 + 77] * inter[77]
		   + l->weight2[k * l->nFilters1 + 78] * inter[78]
		   + l->weight2[k * l->nFilters1 + 79] * inter[79]
		   + l->weight2[k * l->nFilters1 + 80] * inter[80]
		   + l->weight2[k * l->nFilters1 + 81] * inter[81]
		   + l->weight2[k * l->nFilters1 + 82] * inter[82]
		   + l->weight2[k * l->nFilters1 + 83] * inter[83]
		   + l->weight2[k * l->nFilters1 + 84] * inter[84]
		   + l->weight2[k * l->nFilters1 + 85] * inter[85]
		   + l->weight2[k * l->nFilters1 + 86] * inter[86]
		   + l->weight2[k * l->nFilters1 + 87] * inter[87]
		   + l->weight2[k * l->nFilters1 + 88] * inter[88]
		   + l->weight2[k * l->nFilters1 + 89] * inter[89]
		   + l->weight2[k * l->nFilters1 + 90] * inter[90]
		   + l->weight2[k * l->nFilters1 + 91] * inter[91]
		   + l->weight2[k * l->nFilters1 + 92] * inter[92]
		   + l->weight2[k * l->nFilters1 + 93] * inter[93]
		   + l->weight2[k * l->nFilters1 + 94] * inter[94]
		   + l->weight2[k * l->nFilters1 + 95] * inter[95]
		   + l->weight2[k * l->nFilters1 + 96] * inter[96]
		   + l->weight2[k * l->nFilters1 + 97] * inter[97]
		   + l->weight2[k * l->nFilters1 + 98] * inter[98]
		   + l->weight2[k * l->nFilters1 + 99] * inter[99]
		   + l->weight2[k * l->nFilters1 + 100] * inter[100]
		   + l->weight2[k * l->nFilters1 + 101] * inter[101]
		   + l->weight2[k * l->nFilters1 + 102] * inter[102]
		   + l->weight2[k * l->nFilters1 + 103] * inter[103]
		   + l->weight2[k * l->nFilters1 + 104] * inter[104]
		   + l->weight2[k * l->nFilters1 + 105] * inter[105]
		   + l->weight2[k * l->nFilters1 + 106] * inter[106]
		   + l->weight2[k * l->nFilters1 + 107] * inter[107]
		   + l->weight2[k * l->nFilters1 + 108] * inter[108]
		   + l->weight2[k * l->nFilters1 + 109] * inter[109]
		   + l->weight2[k * l->nFilters1 + 110] * inter[110]
		   + l->weight2[k * l->nFilters1 + 111] * inter[111]
		   + l->weight2[k * l->nFilters1 + 112] * inter[112]
		   + l->weight2[k * l->nFilters1 + 113] * inter[113]
		   + l->weight2[k * l->nFilters1 + 114] * inter[114]
		   + l->weight2[k * l->nFilters1 + 115] * inter[115]
		   + l->weight2[k * l->nFilters1 + 116] * inter[116]
		   + l->weight2[k * l->nFilters1 + 117] * inter[117]
		   + l->weight2[k * l->nFilters1 + 118] * inter[118]
		   + l->weight2[k * l->nFilters1 + 119] * inter[119]
		   + l->weight2[k * l->nFilters1 + 120] * inter[120]
		   + l->weight2[k * l->nFilters1 + 121] * inter[121]
		   + l->weight2[k * l->nFilters1 + 122] * inter[122]
		   + l->weight2[k * l->nFilters1 + 123] * inter[123]
		   + l->weight2[k * l->nFilters1 + 124] * inter[124]
		   + l->weight2[k * l->nFilters1 + 125] * inter[125]
		   + l->weight2[k * l->nFilters1 + 126] * inter[126]
		   + l->weight2[k * l->nFilters1 + 127] * inter[127];

	return retVal;
}