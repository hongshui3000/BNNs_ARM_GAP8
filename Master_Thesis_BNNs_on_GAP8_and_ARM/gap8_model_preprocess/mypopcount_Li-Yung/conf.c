

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
//#include "model_generic.dat"
//#include "model_mydata.dat"
#include "model_mydata_07_01.dat"
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
//#define FIXPOINT_SCALE  1048576.0
#define FIXPOINT_SCALE  4096.0 //shift 12 bits
#endif

#define eps1	1e-5
#define eps2 	1e-4
#define eps3    1e-3
#define binFilters  	(BIN_1_FILTERS + BIN_2_FILTERS + BIN_3_FILTERS + BIN_4_FILTERS)

/* local functions */
static int32_t  s32_sign(float s32_a);
static void binLayerManipulator(uint32_t filters, uint32_t LayerStartPos, uint32_t input_channels);
static void writeBinLayer(uint32_t weight_size, uint32_t layer_nfilters, uint32_t blocksize, FILE* file);


/* local variables */
static uint32_t sbpos = 0;
static uint32_t bpos = 0;

static uint32_t new_float_size = 0;
static uint32_t new_bin_size = 0;

static int32_t sbData_signGamma[binFilters];
static int32_t sbData_threshhold[binFilters];
static int32_t sbData_relu[binFilters];
static int32_t sbData_compare[binFilters];


/* main application */
int main(int argc, char *argv[])
{
	uint32_t i;
	uint32_t sum;

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

	FILE* file = fopen("model_mypopcount_07_01_shift12.dat", "w");

#ifdef FIXPOINT_16BIT
	uint32_t checkvalue;
	int32_t fixpoint;
	err = fputs("int16_t fData[] = {", file);
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
	err = fputs("int32_t fData[] = {", file);
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
	err = fputs("float fData[] = {", file);
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
	err = fputs("uint32_t bData[] = {", file);

	sbpos = 0;
	bpos = 0;
	/* bin 1 */
	writeBinLayer(BIN_1_WSEIZE, BIN_1_FILTERS, filterblockSize, file);
	printf("bpos %d \n", new_bin_size );
	/* bin 2 */
	writeBinLayer(BIN_2_WSEIZE, BIN_2_FILTERS, filterblockSize, file);
	printf("bpos %d \n", new_bin_size );
	/* bin 3 */
	writeBinLayer(BIN_3_WSEIZE, BIN_3_FILTERS, filterblockSize, file);
	printf("bpos %d \n", new_bin_size );
	/* bin 4 */
	writeBinLayer(BIN_4_WSEIZE, BIN_4_FILTERS, filterblockSize, file);
	printf("bpos %d \n", new_bin_size );

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
		err = fputs(str, file);
		new_bin_size++;

		if(i < (LL_END_BIN - 1))
		{
			fputs(",", file);
		}
	}
	err = fputs("}; \n", file);	

	fclose(file);

	printf("output size: \n");
	printf("size fData %d    size bData %d \n", new_float_size, new_bin_size );

	sum =  (new_float_size + new_bin_size)*4 ;
	printf("required space %d bytes (%d kBytes) \n", sum, sum/1024);
	return 0;
}



/*
*********************************
* local functions implementation
*********************************
*/ 

static int32_t  s32_sign(float s32_a)
{
    return (s32_a > 0) ? 1: -1;
}

static void binLayerManipulator(uint32_t filters, uint32_t LayerStartPos, uint32_t input_channels)
{
	uint32_t i;
	int32_t signGamma, compare;
	float th,r;
	float th1_f, th2_f, BetaStar;
	int32_t signBetaStar;
	float mean, std, gamma, beta, bias, alpha;
	
	/* bin layer 1 */
	for(i = 0 ; i < filters; i++)
	{
		bias 	= 		fData[LayerStartPos + 			 i];
		alpha 	=	 	fData[LayerStartPos +  filters + i];
		mean 	= 		fData[LayerStartPos +2*filters + i];
		std 	= 		fData[LayerStartPos +3*filters + i];
		gamma 	= 		fData[LayerStartPos +4*filters + i];
		beta 	= 		fData[LayerStartPos +5*filters + i];


		/* sginGamma */
		//signGamma = (1+s32_sign(gamma))/2;
		signGamma = s32_sign(gamma);
		/* threshold */
		BetaStar = beta*sqrt(std+eps1) - (gamma*mean);
		th1_f = bias/(2*alpha) + 16*9*input_channels/32;
		th2_f = th1_f + BetaStar/(2*alpha*gamma);

		//th = (-beta * sqrt(std + eps1));
		//th = th / (signGamma * gamma);
		//th = th + signGamma * (mean - bias);
		//th = th / alpha;


		/* relu */
		//r = -bias / alpha;

		/* compare */
		signBetaStar = (s32_sign(BetaStar) > 0) ? 1 : 0;
		//compare = (signGamma*r > th) ? 1 : 0;

		/* store in sbData */ 
		sbData_signGamma[sbpos + i] 	= signGamma;
		sbData_threshhold[sbpos + i] 	= (uint32_t) th2_f;		//Threshold2
		sbData_relu[sbpos + i] 			= (uint32_t) th1_f;	//Threshold1
		sbData_compare[sbpos + i] 		= signBetaStar;		//signBetaStar
	}
	sbpos += filters;
}


void writeBinLayer(uint32_t weight_size, uint32_t layer_nfilters, uint32_t blocksize, FILE* file)
{
	uint32_t i, k;
	char str[30];
	int32_t err = 0;
	uint32_t iter = 0; 

	uint32_t iterations =  layer_nfilters / blocksize;
	uint32_t weights = weight_size / iterations;

	while(iter < iterations)
	{
		for(i = 0 ; i < weights; i++)
		{
			if(err < 0)
			{
				printf("writting errror\n");
				return;
			}	
			/* first weights */
			sprintf(str, "%u", bData[bpos + i]);
			err = fputs(str, file);
			fputs(",", file);
			new_bin_size++;
		}

		for(k = 0; k < blocksize; k++)
		{
			if(err < 0)
			{
				printf("writting errror\n");
				return;
			}	
			/* signGamma */
			sprintf(str, "%d", sbData_signGamma[sbpos + k]);
			err = fputs(str, file);
			fputs(",", file);
			new_bin_size++;		
		}

		for(k = 0; k < blocksize; k++)
		{
			if(err < 0)
			{
				printf("writting errror\n");
				return;
			}	
			/* threshhold */
			sprintf(str, "%d", sbData_threshhold[sbpos + k]);
			err = fputs(str, file);
			fputs(",", file);
			new_bin_size++;		
		}

		for(k = 0; k < blocksize; k++)
		{
			if(err < 0)
			{
				printf("writting errror\n");
				return;
			}	
			/* relu */
			sprintf(str, "%d", sbData_relu[sbpos + k]);
			err = fputs(str, file);
			fputs(",", file);
			new_bin_size++;		
		}	

		for(k = 0; k < blocksize; k++)
		{
			if(err < 0)
			{
				printf("writting errror\n");
				return;
			}	
			/* compare */
			sprintf(str, "%d", sbData_compare[sbpos + k]);
			err = fputs(str, file);
			fputs(",", file);
			new_bin_size++;		
		}
		sbpos += blocksize;
		bpos += weights;
		iter++;
	}
}
