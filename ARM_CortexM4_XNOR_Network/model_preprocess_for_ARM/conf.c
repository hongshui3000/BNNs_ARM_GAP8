

/* descrption:	This file generates a 32 filter model.dat structure
 * \details 	model.dat
 *						| position 	|
 *					-----------------------------------------	
 *				fData:	|   0 -  31 |	start layer bias
 *						|  32 - 319 |	start layer weights
 *						| 320 - 351 |	start layer mean 1
 *						| 352 - 383 |	start layer sdt 1
 *						| 384 - 415	|	start layer gamma 1
 *						| 416 - 447	|  	start layer beta 1	
 *						| 448 - 479 |	start layer mean 2
 *						| 480 - 511 |	start layer sdt 2
 *						| 512 - 543	|	start layer gamma 2
 *						| 544 - 575	|  	start layer beta 2	
 *						| 576 -     |	last  layer bias 1	(length: 128)
 *						|     -     |	last  layer alpha 1
 *						|  			|	last  layer mean
 *						|  			|	last  layer sdt
 *						|  			|	last  layer gamma
 *						| 	  - 1343|  	last  layer beta	
 *						|1344 - 1371|	last  layer bias 2
 *						|1372 - 4955|	last  layer weights 2
 *				bData:	|	0 - 287 |	bin 1 weights 	for 1. 32-filters
 *						| 288 - 319 |	bin 1 signGamma for 1. 32-filters
 *						| 320 -     |	bin 1 threshold for 1. 32-filters
 *						|     -     |	bin 1 relu 		for 1. 32-filters
 *					    |     -     |	bin 1 compare 	for 1. 32-filters  
 *						|416  - 703 |	bin 1 weights 	for 2. 32-filters
 *					    |	  -     |	bin 1 signGamma for 2. 32-filters 
 *						|     -     |	bin 1 threshold for 2. 32-filters
 *					    |  .......  |	... 
 *						|     -     |	bin 4 threshold for 4. 32-filters
 *						|     -14399|	last layer weights 1  
 *
 * 	
 * option: generate 16bit fixpoint instead of floating point weights & parameters 
*/

#include <stdint.h>
#include <stdio.h>
#include <math.h>

/* 	model_generic data structure */
#include "model.dat"
#include "model_generic.h"


/******************************
* OPTIONS
******************************/
// #define FIXPOINT_16BIT 				/*< if set, input will be fix-point 16bit instead of floating point */
 #define FIXPOINT_32BIT				/*< if set, input will be fix-point 32bit instead of floating point */

/******************************
* OPTIONS
******************************/
#ifdef FIXPOINT_16BIT
#define FIXPOINT_SCALE	256.0	/*< scaling factor for fixpoint */
#elif defined(FIXPOINT_32BIT)
#define FIXPOINT_SCALE  1048576.0
#endif

#define eps1	1e-5
#define eps2 	1e-4
#define eps3    1e-3
#define binFilters  	(BIN_1_FILTERS + BIN_2_FILTERS + BIN_3_FILTERS + BIN_4_FILTERS)

/* local functions */
static int32_t  s32_sign(float s32_a);
static void binLayerManipulator(uint32_t filters, uint32_t LayerStartPos, uint32_t input_channels);
static void writeBin32Layer(uint32_t weight_size, uint32_t layer_nfilters, uint32_t blocksize, FILE* file);
static void writeBin8Layer(uint32_t layer_nfilters, uint32_t blocksize, FILE* file);

/* local variables */
static uint32_t sbpos = 0;
static uint32_t bpos = 0;

static uint32_t new_float_size = 0;
static uint32_t new_bin32_size = 0;
static uint32_t new_bin8_size = 0;

/*static int8_t sbData_signGamma_positive[binFilters];
static int8_t sbData_signGamma_negative[binFilters];
static uint32_t sbData_threshold_1[binFilters];
static uint32_t sbData_threshold_2[binFilters];
static int8_t sbData_signBetaStar[binFilters];
*/
static uint32_t sbData_signGamma_positive[binFilters/32];
static uint32_t sbData_signGamma_negative[binFilters/32];
static uint32_t sbData_threshold_1[binFilters];
static uint32_t sbData_threshold_2[binFilters];
static uint32_t sbData_signBetaStar[binFilters/32];
static void signParameter_init();
/* main application */
int main(int argc, char *argv[])
{
	uint32_t i;
	uint32_t sum;
	signParameter_init(binFilters);
	printf("input size: \n");
	printf("size float %d    size bin %d \n", (int) sizeof(fData)/4, (int) sizeof(bData)/4 );

	/*
	************************************* 
	* calculate new model data 
	*************************************
	*/

	/* preprocess first layer std */
	for(i = 0 ; i < SL_FILTERS; i++)
	{
		fData[SL_STD_1+i] = 1.0 / sqrt(fData[SL_STD_1+i] + eps1);
		fData[SL_STD_2+i] = 1.0 / sqrt(fData[SL_STD_2+i] + eps2);
	}

	/* preprocess bin layers: signGamma, threshold, relu, compare */
	/* bin layer 1 */
	sbpos = 0;
	binLayerManipulator(BIN_1_FILTERS, BIN_1_BIAS, SL_FILTERS);

	/* bin layer 2 */
	binLayerManipulator(BIN_2_FILTERS, BIN_2_BIAS, BIN_1_FILTERS);

	/* bin layer 3 */
	binLayerManipulator(BIN_3_FILTERS, BIN_3_BIAS, BIN_2_FILTERS);

	/* bin layer 4 */
	binLayerManipulator(BIN_4_FILTERS, BIN_4_BIAS, BIN_3_FILTERS);

	/* preprocess layer layer std */
	for(i = 0 ; i < LL_FILTERS; i++)
	{
		//fData[LL_STD+i] = 1.0 / sqrt(fData[LL_STD+i] + eps1);
		fData[LL_STD+i] = 1.0 / sqrt(fData[LL_STD+i] + eps3);
	}

	/*
	************************************* 
	* write to file in 32 filter blocks
	*************************************
	*/
	printf("generate model.dat \n");

	int err;
	char str[20];
	uint32_t filterblockSize = 32;

	FILE* file = fopen("model_optimize_sign_vectorized.dat", "w");

#ifdef FIXPOINT_16BIT
	uint32_t checkvalue;
	int32_t fixpoint;
	err = fputs("const int16_t fData[] = {", file);
	/* first layer float data */
	for(i = 0; i < SL_1_END; i++)
	{
		if(err < 0)
		{
			printf("writting errror\n");
			return 0;
		}
		fixpoint = (int) (fData[i] * FIXPOINT_SCALE);
		checkvalue = fixpoint & 0xFFFF0000;
		if((checkvalue > 0 ) && (checkvalue != 0xFFFF0000))
		{
			printf("error 16 bit overflow: %x \n", fixpoint);
			return 0;
		}
		sprintf(str, "%d", fixpoint);
		err = fputs(str, file);
		fputs(",", file);
		new_float_size++;
	}
	printf("fpos %d \n", new_float_size );

	/* last layer float data */
	for(i = LL_START; i < LL_END_FLOAT; i++)
	{
		if(err < 0)
		{
			printf("writting errror\n");
			return 0;
		}

		fixpoint = (int) (fData[i] * FIXPOINT_SCALE);
		checkvalue = fixpoint & 0xFFFF0000;
		if((checkvalue > 0 ) && (checkvalue != 0xFFFF0000))
		{
			printf("error: 16 bit overflow: %x \n", fixpoint);
			return 0;
		}
		sprintf(str, "%d", fixpoint);
		err = fputs(str, file);
		new_float_size++;

		if(i < (LL_END_FLOAT - 1))
		{
			fputs(",", file);
		}
	}
	err = fputs("}; \n", file);

#elif defined(FIXPOINT_32BIT)
	uint64_t checkvalue;
	int64_t fixpoint;
	err = fputs("const int32_t fData[] = {", file);
	/* first layer float data */
	for(i = 0; i < SL_1_END; i++)
	{
		if(err < 0)
		{
			printf("writting errror\n");
			return 0;
		}
		fixpoint = (int64_t) (fData[i] * FIXPOINT_SCALE);
		checkvalue = fixpoint & 0xFFFFFFFF00000000;
		if((checkvalue > 0 ) && (checkvalue != 0xFFFFFFFF00000000))
		{
			printf("error 32 bit overflow: %x \n", (unsigned int) (fixpoint >> 32));
			return 0;
		}
		sprintf(str, "%d", (int32_t)fixpoint);
		err = fputs(str, file);
		fputs(",", file);
		new_float_size++;
	}
	printf("fpos %d \n", new_float_size );

	/* last layer float data */
	for(i = LL_START; i < LL_END_FLOAT; i++)
	{
		if(err < 0)
		{
			printf("writting errror\n");
			return 0;
		}

		fixpoint = (int64_t) (fData[i] * FIXPOINT_SCALE);
		checkvalue = fixpoint & 0xFFFFFFFF00000000;
		if((checkvalue > 0 ) && (checkvalue != 0xFFFFFFFF00000000))
		{
			printf("error: 32 bit overflow: %x \n", (unsigned int) (fixpoint >> 32));
			return 0;
		}
		sprintf(str, "%d", (int32_t)fixpoint);
		err = fputs(str, file);
		new_float_size++;

		if(i < (LL_END_FLOAT - 1))
		{
			fputs(",", file);
		}
	}
	err = fputs("}; \n", file);

#else /* floating point */
	err = fputs("const float fData[] = {", file);
	/* first layer float data */
	for(i = 0; i < SL_1_END; i++)
	{
		if(err < 0)
		{
			printf("writting errror\n");
			return 0;
		}
		sprintf(str, "%f", fData[i]);
		err = fputs(str, file);
		fputs(",", file);
		new_float_size++;
	}
	printf("fpos %d \n", new_float_size );

	/* last layer float data */
	for(i = LL_START; i < LL_END_FLOAT; i++)
	{
		if(err < 0)
		{
			printf("writting errror\n");
			return 0;
		}	
		sprintf(str, "%f", fData[i]);
		err = fputs(str, file);
		new_float_size++;

		if(i < (LL_END_FLOAT - 1))
		{
			fputs(",", file);
		}
	}
	err = fputs("}; \n", file);
#endif	


	/* write binary layer binary data */
	err = fputs("const uint32_t b32Data[] = {", file);

	sbpos = 0;
	bpos = 0;
	/* bin 1 */
	writeBin32Layer(BIN_1_WSEIZE, BIN_1_FILTERS, filterblockSize, file);
	printf("bpos %d \n", new_bin32_size );
	/* bin 2 */
	writeBin32Layer(BIN_2_WSEIZE, BIN_2_FILTERS, filterblockSize, file);
	printf("bpos %d \n", new_bin32_size );
	/* bin 3 */
	writeBin32Layer(BIN_3_WSEIZE, BIN_3_FILTERS, filterblockSize, file);
	printf("bpos %d \n", new_bin32_size );
	/* bin 4 */
	writeBin32Layer(BIN_4_WSEIZE, BIN_4_FILTERS, filterblockSize, file);
	printf("bpos %d \n", new_bin32_size );

	/* write last layer binary data */
	if(bpos != LL_WEIGHTS_1) {printf("error: binary weights \n"); return 0;}

	for(i = LL_WEIGHTS_1; i < LL_END_BIN; i++)
	{
		if(err < 0)
		{
			printf("writting errror\n");
			return 0;
		}	
		sprintf(str, "%u", bData[i]);
	//	printf("%d\n",i);
		err = fputs(str, file);
		new_bin32_size++;

		if(i < (LL_END_BIN - 1))
		{
			fputs(",", file);
		}
	}
	err = fputs("}; \n", file);	

        /* write 8 bit, signGamma1, signGamma2, BetaStar */
        err = fputs("const uint32_t bSignData[] = {", file);
	sbpos = 0;
        /* bin 1 */
        writeBin8Layer(BIN_1_FILTERS, filterblockSize, file);
        printf("sbpos %d \n", new_bin8_size );
        /* bin 2 */
        writeBin8Layer(BIN_2_FILTERS, filterblockSize, file);
        printf("sbpos %d \n", new_bin8_size );
        /* bin 3 */
        writeBin8Layer(BIN_3_FILTERS, filterblockSize, file);
        printf("sbpos %d \n", new_bin8_size );
        /* bin 4 */
        writeBin8Layer(BIN_4_FILTERS, filterblockSize, file);
        printf("sbpos %d \n", new_bin8_size );

        err = fputs("}; \n", file);


	fclose(file);

	printf("output size: \n");
	printf("size fData %d, size bSignData %d, size b32Data %d \n", new_float_size, new_bin8_size, new_bin32_size );

	//sum =  (new_float_size + new_bin32_size)*4 + new_bin8_size;
	sum =  (new_float_size + new_bin32_size)*4 + new_bin8_size*4;
	printf("required space %d bytes (%d kBytes) \n", sum, sum/1024);
	return 0;
}



/*
*********************************
* local functions implementation
*********************************
*/ 

static void signParameter_init(uint32_t nFilters)
{
	uint32_t i;
	for(i=0;i<nFilters;i++)
	{	
	
		sbData_signGamma_positive[i]   = 0;
		sbData_signGamma_negative[i]   = 0;
		sbData_signGamma_negative[i]   = 0;
	}
}

static int32_t  s32_sign(float s32_a)
{
    return (s32_a > 0) ? 1: -1;
}

static void binLayerManipulator(uint32_t filters, uint32_t LayerStartPos, uint32_t input_channels)
{
	uint32_t i;
//	int8_t signGamma_positive, signGamma_negative;
//	int8_t signBetaStar;
	uint32_t signGamma_positive, signGamma_negative;
	uint32_t signBetaStar;
	int32_t threshold_1, threshold_2;
	float th1_f, th2_f, BetaStar;
	float mean, std, gamma, beta, bias, alpha;
	int32_t shift, bink;
	shift = 0;
	
	/* bin layer 1 */
	for(i = 0 ; i < filters; i++)
	{
        	bink = i / 32;
		bias 	= 		fData[LayerStartPos + 			 i];
		alpha 	=	 	fData[LayerStartPos +  filters + i];
		mean 	= 		fData[LayerStartPos +2*filters + i];
		std 	= 		fData[LayerStartPos +3*filters + i];
		gamma 	= 		fData[LayerStartPos +4*filters + i];
		beta 	= 		fData[LayerStartPos +5*filters + i];


		/* sginGamma */
		signGamma_positive = (1+s32_sign(gamma))/2;
		signGamma_negative = (1-s32_sign(gamma))/2;

		/* threshold 1 & 2*/
		BetaStar = beta*sqrt(std+eps1) - (gamma*mean);
		th1_f = bias/(2*alpha) + 16*9*input_channels/32;
		th2_f = th1_f + BetaStar/(2*alpha*gamma);
		//threshold_1 = int32_t (th1_f);
		//threshold_2 = int32_t (th2_f);

		/* signBetaStar */
		signBetaStar = (s32_sign(BetaStar) > 0) ? 1 : 0;

		/* store in sbData */ 
//		sbData_signGamma_positive[sbpos + i] 	= (int8_t) signGamma_positive;
//		sbData_signGamma_negative[sbpos + i]    = (int8_t) signGamma_negative;

		sbData_signGamma_positive[(sbpos/32) + bink]    |= signGamma_positive << shift;
//		sbData_signGamma_negative[(sbpos/32) + bink]    |= signGamma_negative << shift;
		sbData_threshold_1[sbpos + i] 	  = (uint32_t) th1_f;
		sbData_threshold_2[sbpos + i]    = (uint32_t) th2_f;
		sbData_signBetaStar[(sbpos/32) + bink] 	  |= signBetaStar << shift;

//		sbData_signBetaStar[sbpos + i] 	  = (int8_t) signBetaStar;
        	shift = (shift+1) % 32;
	}
	sbpos += filters;
}


void writeBin32Layer(uint32_t weight_size, uint32_t layer_nfilters, uint32_t blocksize, FILE* file)
{
	uint32_t i, k;
	char str[30];
	uint32_t err = 0;
	uint32_t iter = 0; 

	uint32_t iterations =  layer_nfilters / blocksize;
	uint32_t weights = weight_size / iterations;

/*	while(iter < iterations)
	{
		for(i = 0 ; i < weights; i++)
		{
			if(err < 0)
			{
				printf("writting errror\n");
				return;
			}	
			// first weights 
			sprintf(str, "%u", bData[bpos + i]);
			err = fputs(str, file);
			fputs(",", file);
			new_bin32_size++;
		}

		for(k = 0; k < blocksize; k++)
		{
			if(err < 0)
			{
				printf("writting errror\n");
				return;
			}	
			// threshhold 
			sprintf(str, "%u", sbData_threshold_1[sbpos + k]);
			err = fputs(str, file);
			fputs(",", file);
			new_bin32_size++;		
		}

                for(k = 0; k < blocksize; k++)
                {
                        if(err < 0)
                        {
                                printf("writting errror\n");
                                return;
                        }
                        // threshhold 
                        sprintf(str, "%u", sbData_threshold_2[sbpos + k]);
                        err = fputs(str, file);
                        fputs(",", file);
                        new_bin32_size++;
                }


		sbpos += blocksize;
		bpos += weights;
		iter++;
	}
*/

	while(iter < iterations)
	{
		for(i = 0 ; i < weights; i++)
		{
			if(err < 0)
			{
				printf("writting errror\n");
				return;
			}	
			// first weights 
			sprintf(str, "%u", bData[bpos + i]);
			err = fputs(str, file);
			fputs(",", file);
			new_bin32_size++;
		}
		bpos += weights;
		iter++;
	}

		for(k = 0; k < layer_nfilters; k++)
		{
			if(err < 0)
			{
				printf("writting errror\n");
				return;
			}	
			// threshhold 1
			sprintf(str, "%u", sbData_threshold_1[sbpos + k]);
			err = fputs(str, file);
			fputs(",", file);
			new_bin32_size++;
		}	
		for(k = 0; k < layer_nfilters; k++)
		{
                        if(err < 0)
                        {
                                printf("writting errror\n");
                                return;
                        }
                        // threshhold 2 
                        sprintf(str, "%u", sbData_threshold_2[sbpos + k]);
                        err = fputs(str, file);
                        fputs(",", file);
                        new_bin32_size++;	
		}
		sbpos += layer_nfilters;


}

void writeBin8Layer(uint32_t layer_nfilters, uint32_t blocksize, FILE* file)
{
	uint32_t i, k;
	char str[30];
	uint32_t err = 0;
	uint32_t iter = 0; 
	static uint32_t x = 0;
	//uint32_t iterations =  layer_nfilters / blocksize;


		for(k = 0; k < layer_nfilters/32; k++)
		{
			if(err < 0)
			{
				printf("writting errror\n");
				return;
			}	
			/* signGamma */
			x++;
			sprintf(str, "%u", sbData_signGamma_positive[sbpos + k]);
			err = fputs(str, file);
			fputs(",", file);
			new_bin8_size++;		
		}

/*                for(k = 0; k < layer_nfilters/32; k++)
                {
                        if(err < 0)
                        {
                                printf("writting errror\n");
                                return;
                        }
                        // signGamma 
			x++;
                        sprintf(str, "%u", sbData_signGamma_negative[sbpos + k]);
                        err = fputs(str, file);
                        fputs(",", file);
                        new_bin8_size++;
                }*/


		for(k = 0; k < layer_nfilters/32; k++)
		{
			if(err < 0)
			{
				printf("writting errror\n");
				return;
			}	
			/* BetaStar */
			x++;
			sprintf(str, "%u", sbData_signBetaStar[sbpos + k]);
			err = fputs(str, file);
			if(x < 96*14/32)
//			if(x < 96*14)
			{fputs(",", file);}
			new_bin8_size++;		
		}	
		sbpos += layer_nfilters/32;

}
