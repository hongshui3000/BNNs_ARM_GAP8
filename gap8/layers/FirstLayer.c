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
#include "FirstLayer.h"

#include "../utility/misc.h"

/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/

/* ==================================================================================
 * Local/Global variables definitions
 * ================================================================================*/
//static  PLP_L1_DATA uint8_t 			kX2;
//static  PLP_L1_DATA uint8_t 			kY2;
//static  PLP_L1_DATA uint8_t 			x0;
//static  PLP_L1_DATA uint8_t 			y0;
//static  PLP_L1_DATA LAYER_CONFIG* 		con;

//static  PLP_L1_DATA uint32_t			filters_done;

static  uint8_t                   kX2;
static  uint8_t                   kY2;
static  uint8_t                     x0;
static  uint8_t                     y0;
static  LAYER_CONFIG*               con;
static  uint32_t                    filters_done;


/*PLP_L1_DATA*/
/* ==================================================================================
 * Local function declarations
 * ================================================================================*/

#ifdef FIXPOINT_16BIT
static inline void firstLayer_convolution_32filters_16bitFixPoint(TENSOR* __restrict__ input, FIRST_FEATURES* __restrict__ features, TENSOR* __restrict__ output);
#elif defined(FIXPOINT_32BIT)
static inline void firstLayer_convolution_32filters_32bitFixPoint(TENSOR* __restrict__ input, FIRST_FEATURES* __restrict__ features, TENSOR* __restrict__ output);
#else
static inline void firstLayer_convolution_32filters_float(TENSOR* __restrict__ input, FIRST_FEATURES* __restrict__ features, TENSOR* __restrict__ output);
#endif

/* ==================================================================================
 * Local function implementations
 * ================================================================================*/
/* ==================================================================================
 * Global function implementations
 * ================================================================================*/

	extern void firstLayer_setConfiguration
(
 LAYER_CONFIG* config
 )
{
	con = config;

	/* Half size of the filters */
	kX2 = con->kX / 2
		;    kY2 = con->kY / 2;

	/* starting position */
	x0 = kX2 - con->padX;
	y0 = kY2 - con->padY;

	filters_done = 0;
}

	extern void firstLayer_convolution_32filters
(
 TENSOR* __restrict__ input,
 FIRST_FEATURES* __restrict__ features,
 TENSOR* __restrict__ output	
 )
{


#ifdef FIXPOINT_16BIT 
	//	printf("do conv \n");
	firstLayer_convolution_32filters_16bitFixPoint(input, features, output);
#elif defined(FIXPOINT_32BIT) 
	//	printf("f32 \n");
	firstLayer_convolution_32filters_32bitFixPoint(input, features, output);
#else
	//	printf("ff \n");
	firstLayer_convolution_32filters_float(input, features, output);
#endif

}


#ifdef FIXPOINT_16BIT
	static inline void firstLayer_convolution_32filters_16bitFixPoint
(
 TENSOR* __restrict__ input,
 FIRST_FEATURES* __restrict__ features,
 TENSOR* __restrict__ output
 )

{

	//	#ifdef MULTICORES	    
	//	#pragma omp parallel shared(input, output) shared(features) shared( x0, y0, kX2, kY2)    
	//#pragma omp parallel shared(input, output, features, x0, y0, kX2, kY2)    
	//	{
	//	#endif	
	uint32_t i, j, k, ki, kj, kk, outi, outj;
	uint32_t outX, outY;
	uint32_t leftX, rightX, leftY, rightY;
	uint32_t shift, bink;
	int16_t *filter;
	int32_t sum;
	int32_t index;
	bink = filters_done / 32;

	//	#ifdef MULTICORES	    
	//	#pragma omp for schedule(static)
	//	#endif	
	for(k = 0; k < 32; k++)
	{
		//	printf("f16 fun 2\n");
		filter = &(features->weight[k * con->kX * con->kY * input->Z]);
		//	printf("filter : %p, value : %d\n", filter, *filter);
		shift = k % 32;
		outi = 0;
		// move filter in x-direction 
		for(i = x0; i < input->X - x0; i += con->dX)
		{
			// max 
			leftX = (0 > (int32_t) i - kX2) ? 0: i - kX2;
			// min 
			rightX = (input->X < i + kX2+1) ? input->X: i + kX2+1;
			//		    printf("f16 fun 3.5\n");
			outj = 0;
			// move filter in y-direction
			for(j = y0; j < input->Y - y0; j += con->dY)
			{
				// max 
				leftY = (0 > (int32_t)j - kY2) ? 0: j - kY2;
				// min 
				rightY = (input->Y < j + kY2+1) ? input->Y: j + kY2+1;
				//			printf("f16 fun 4\n");
				sum = 0;
				// convolution 
				for(ki = leftX; ki < rightX; ki++){
					//printf("leftX rightX : ok");
					for(kj = leftY; kj < rightY; kj++){
						for(kk = 0; kk < input->Z; kk++)
						{
							//printf("f16 conv loop ok");
							sum += ((int32_t)filter[ ((kX2 + ki - i)*con->kY + kY2 + kj - j)*input->Z + kk ] *(int32_t)input->memFixpoint[ (ki*input->Y + kj )*input->Z + kk ]) >> FP_SCALE_BITs;
						}
					}
				}
				sum = sum + (int32_t) features->bias[k];
				// batch normalization 1 
				sum = (((((sum - features->mean1[k])*features->std1[k]) >> FP_SCALE_BITs) * features->gamma1[k]) >> FP_SCALE_BITs) + features->beta1[k];
				// ReLu 
				sum = (0 > sum) ? 0: sum;
				// batch normalization 2 
				sum = (((((sum - features->mean2[k])*features->std2[k]) >> FP_SCALE_BITs) * features->gamma2[k]) >> FP_SCALE_BITs) + features->beta2[k];
				// binarize 
				if(sum > 0)
				{
					index = (outi*output->Y + outj)*output->Z + bink;

					//				#ifdef MULTICORES
					//				#pragma omp critical
					//				#endif
					output->memBin[index] |=  1<<shift;

				}
				outj++;
			} 
			outi++;
		}
	}

	//	#ifdef MULTICORES	    
	//	}
	//    	#endif	

	// 32 filters applied 
	filters_done += 32;
}

#elif defined(FIXPOINT_32BIT)
	static inline void firstLayer_convolution_32filters_32bitFixPoint
(
 TENSOR* __restrict__ input,
 FIRST_FEATURES* __restrict__ features,
 TENSOR* __restrict__ output
 )
{

    #ifdef MULTICORES	    
    #pragma omp parallel shared(input, output) shared(features) shared( x0, y0, kX2, kY2)    
    {
    #endif	
	uint32_t i, j, k, ki, kj, kk, outi, outj;
	uint32_t outX, outY;
	uint32_t leftX, rightX, leftY, rightY;
	uint32_t shift, bink;
	int32_t *filter;
	int64_t sum;
	int32_t index;
	bink = filters_done / 32;

	#ifdef MULTICORES	    
	#pragma omp for schedule(static)
	#endif	
	for(k = 0; k < 32; k++)
	{
		//	printf("f32 conv 1 ok");
		filter = &(features->weight[k*con->kX*con->kY*input->Z]);

		shift = k % 32;
		outi = 0;
		/* move filter in x-direction */
		//	printf("f32 conv 2 ok");
		for(i = x0; i < input->X - x0; i += con->dX)
		{
			/* max */
			leftX = (0 > (int32_t) i - kX2) ? 0: i - kX2;
			/* min */
			rightX = (input->X < i + kX2+1) ? input->X: i + kX2+1;
			outj = 0;
			/* move filter in y-direction */
			for(j = y0; j < input->Y - y0; j += con->dY)
			{
				/* max */
				leftY = (0 > (int32_t)j - kY2) ? 0: j - kY2;
				/* min */
				rightY = (input->Y < j + kY2+1) ? input->Y: j + kY2+1;			
				sum = 0;
				/* convolution */

#ifdef UNFOLDED
				if((rightX - leftX == 3) && (rightY - leftY == 3))
				{
					switch(input->Z)
					{					
						case 0x1: 		/* input->Z = 1 */
							sum += (( (int64_t)filter[  0] * (int64_t)input->memFixpoint[ leftX   *input->Y + leftY    ])>> FP_SCALE_BITs) 
								+ (((int64_t)filter[1]  * (int64_t)input->memFixpoint[  leftX   *input->Y + leftY + 1])>> FP_SCALE_BITs) 
								+ (((int64_t)filter[2]  * (int64_t)input->memFixpoint[  leftX   *input->Y + leftY + 2])>> FP_SCALE_BITs) 
								+ (((int64_t)filter[3]  * (int64_t)input->memFixpoint[ (leftX+1)*input->Y + leftY    ])>> FP_SCALE_BITs) 
								+ (((int64_t)filter[4]  * (int64_t)input->memFixpoint[ (leftX+1)*input->Y + leftY + 1])>> FP_SCALE_BITs) 
								+ (((int64_t)filter[5]  * (int64_t)input->memFixpoint[ (leftX+1)*input->Y + leftY + 2])>> FP_SCALE_BITs) 
								+ (((int64_t)filter[6]  * (int64_t)input->memFixpoint[ (leftX+2)*input->Y + leftY    ])>> FP_SCALE_BITs) 
								+ (((int64_t)filter[7]  * (int64_t)input->memFixpoint[ (leftX+2)*input->Y + leftY + 1])>> FP_SCALE_BITs) 
								+ (((int64_t)filter[8]  * (int64_t)input->memFixpoint[ (leftX+2)*input->Y + leftY + 2])>> FP_SCALE_BITs); 
							break;
					}
				}
				else
#endif
				{	
					for(ki = leftX; ki < rightX; ki++){
						for(kj = leftY; kj < rightY; kj++){
							for(kk = 0; kk < input->Z; kk++)
							{
								sum += ((int64_t)filter[ ((kX2 + ki - i)*con->kY + kY2 + kj - j)*input->Z + kk ] *
										(int64_t)input->memFixpoint[ (ki*input->Y + kj )*input->Z + kk ]) >> FP_SCALE_BITs;
							}
						}
					}
				}
				sum = sum + features->bias[k];

				/* batch normalization 1 */
				sum = (((((sum - features->mean1[k])*features->std1[k]) >> FP_SCALE_BITs) * features->gamma1[k]) >> FP_SCALE_BITs) + features->beta1[k];

				/* ReLu */
				sum = (0 > sum) ? 0: sum;

				/* batch normalization 2 */
				sum = (((((sum - features->mean2[k])*features->std2[k]) >> FP_SCALE_BITs) * features->gamma2[k]) >> FP_SCALE_BITs) + features->beta2[k];

				/* binarize */
				if(sum > 0)
				{
					index = (outi*output->Y + outj)*output->Z + bink;

					#ifdef MULTICORES
					#pragma omp critical
					#endif
					{
					output->memBin[index] |=  1<<shift;
					}
				}
				outj++;
			} 
			outi++;
		}
	}

	#ifdef MULTICORES	    
	}
    #endif	

	/* 32 filters applied */
	filters_done += 32;
}



#else
	static inline void firstLayer_convolution_32filters_float
(
 TENSOR* __restrict__ input,
 FIRST_FEATURES* __restrict__ features,
 TENSOR* __restrict__ output
 )
{

	#ifdef MULTICORES	    
	#pragma omp parallel shared(input, output) 	shared(features) shared( x0, y0, kX2, kY2)    
	{
	#endif	
	uint32_t i, j, k, ki, kj, kk, outi, outj;
	uint32_t outX, outY;
	uint32_t leftX, rightX, leftY, rightY;
	uint32_t shift, bink;
	int32_t index;
	float *filter;
	float sum;

	bink = filters_done / 32;

	#ifdef MULTICORES	    
	#pragma omp for schedule(static)
	#endif	
	for(k = 0; k < 32; k++)
	{
		filter = &(features->weight[k * con->kX * con->kY * input->Z]);

		shift = k % 32;
		outi = 0;
		/* move filter in x-direction */
		for(i = x0; i < input->X - x0; i += con->dX)
		{
			/* max */
			leftX = (0 > (int32_t) i - kX2) ? 0: i - kX2;
			/* min */
			rightX = (input->X < i + kX2+1) ? input->X: i + kX2+1;
			outj = 0;
			/* move filter in y-direction */
			for(j = y0; j < input->Y - y0; j += con->dY)
			{
				/* max */
				leftY = (0 > (int32_t)j - kY2) ? 0: j - kY2;
				/* min */
				rightY = (input->Y < j + kY2+1) ? input->Y: j + kY2+1;
				sum = 0;
#ifdef UNFOLDED
				if((rightX - leftX == 3) && (rightY - leftY == 3))
				{
					switch(input->Z)
					{					
						case 0x1: 		/* input->Z = 1 */
							sum += ( filter[  0] * input->memFloat[ leftX   *input->Y + leftY    ]) 
								+ ( filter[  + 1]   * input->memFloat[  leftX   *input->Y + leftY + 1]) 
								+ ( filter[  + 2]   * input->memFloat[  leftX   *input->Y + leftY + 2]) 
								+ ( filter[  + 3]   * input->memFloat[ (leftX+1)*input->Y + leftY    ]) 
								+ ( filter[  + 4]   * input->memFloat[ (leftX+1)*input->Y + leftY + 1]) 
								+ ( filter[  + 5]   * input->memFloat[ (leftX+1)*input->Y + leftY + 2]) 
								+ ( filter[  + 6]   * input->memFloat[ (leftX+2)*input->Y + leftY    ]) 
								+ ( filter[  + 7]   * input->memFloat[ (leftX+2)*input->Y + leftY + 1]) 
								+ ( filter[  + 8]   * input->memFloat[ (leftX+2)*input->Y + leftY + 2]); 
							break;
						case 0x2: 
							// printf("z = 2\n");
							sum += ( filter[  0] * input->memFloat[( leftX   *input->Y + leftY 	 )*2 	]) 
								+ ( filter[ 1]   * input->memFloat[ ( leftX   *input->Y + leftY  )*2 + 1	]) 
								+ ( filter[ 2]   * input->memFloat[ ( leftX   *input->Y + leftY  + 1)*2		]) 
								+ ( filter[ 3]   * input->memFloat[ ( leftX   *input->Y + leftY  + 1)*2 + 1	]) 
								+ ( filter[ 4]   * input->memFloat[ ( leftX   *input->Y + leftY  + 2)*2		]) 
								+ ( filter[ 5]   * input->memFloat[ ( leftX   *input->Y + leftY  + 2)*2 + 1	]) 
								+ ( filter[ 6]   * input->memFloat[ ((leftX+1)*input->Y + leftY  )*2 		]) 
								+ ( filter[ 7]   * input->memFloat[ ((leftX+1)*input->Y + leftY  )*2 + 1	]) 
								+ ( filter[ 8]   * input->memFloat[ ((leftX+1)*input->Y + leftY  + 1)*2		]) 
								+ ( filter[ 9]   * input->memFloat[ ((leftX+1)*input->Y + leftY  + 1)*2 + 1	]) 
								+ ( filter[ 10]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 2)*2		]) 
								+ ( filter[ 11]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 2)*2 + 1	]) 
								+ ( filter[ 12]  * input->memFloat[ ((leftX+2)*input->Y + leftY  )*2 		]) 
								+ ( filter[ 13]  * input->memFloat[ ((leftX+2)*input->Y + leftY  )*2 + 1	]) 
								+ ( filter[ 14]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 1)*2		]) 
								+ ( filter[ 15]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 1)*2 + 1	]) 
								+ ( filter[ 16]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 2)*2		]) 
								+ ( filter[ 17]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 2)*2 + 1	]); 
							break;
						case 0x3: 
							sum += ( filter[ 0] * input->memFloat[ ( leftX   *input->Y + leftY  )*3]) 
								+ ( filter[ 1]   * input->memFloat[ ( leftX   *input->Y + leftY     )*3 + 1]) 
								+ ( filter[ 2]   * input->memFloat[ ( leftX   *input->Y + leftY     )*3 + 2	]) 
								+ ( filter[ 3]   * input->memFloat[ ( leftX   *input->Y + leftY  + 1)*3 	]) 
								+ ( filter[ 4]   * input->memFloat[ ( leftX   *input->Y + leftY  + 1)*3 + 1	]) 
								+ ( filter[ 5]   * input->memFloat[ ( leftX   *input->Y + leftY  + 1)*3 + 2	]) 
								+ ( filter[ 6]   * input->memFloat[ ( leftX   *input->Y + leftY  + 2)*3 	]) 
								+ ( filter[ 7]   * input->memFloat[ ( leftX   *input->Y + leftY  + 2)*3 + 1	]) 
								+ ( filter[ 8]   * input->memFloat[ ( leftX   *input->Y + leftY  + 2)*3 + 2	]) 
								+ ( filter[ 9]   * input->memFloat[ ((leftX+1)*input->Y + leftY     )*3		]) 
								+ ( filter[ 10]  * input->memFloat[ ((leftX+1)*input->Y + leftY     )*3 + 1	]) 
								+ ( filter[ 11]  * input->memFloat[ ((leftX+1)*input->Y + leftY     )*3 + 2	]) 
								+ ( filter[ 12]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 1)*3 		]) 
								+ ( filter[ 13]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 1)*3 + 1	]) 
								+ ( filter[ 14]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 1)*3 + 2	]) 
								+ ( filter[ 15]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 2)*3		]) 
								+ ( filter[ 16]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 2)*3 + 1	]) 
								+ ( filter[ 17]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 2)*3 + 2	])  
								+ ( filter[ 18]  * input->memFloat[ ((leftX+2)*input->Y + leftY     )*3		]) 
								+ ( filter[ 19]  * input->memFloat[ ((leftX+2)*input->Y + leftY     )*3 + 1	]) 
								+ ( filter[ 20]  * input->memFloat[ ((leftX+2)*input->Y + leftY     )*3 + 2	]) 
								+ ( filter[ 21]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 1)*3		]) 
								+ ( filter[ 22]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 1)*3 + 1	]) 
								+ ( filter[ 23]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 1)*3 + 2	]) 
								+ ( filter[ 24]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 2)*3		]) 
								+ ( filter[ 25]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 2)*3 + 1	]) 
								+ ( filter[ 26]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 2)*3 + 2	]);  					
							break;
						case 0x4: 
							sum += ( filter[ 0] * input->memFloat[( leftX   *input->Y + leftY 	 )*4 		]) 
								+ ( filter[ 1]   * input->memFloat[ ( leftX   *input->Y + leftY 	 )*4 + 1	]) 
								+ ( filter[ 2]   * input->memFloat[ ( leftX   *input->Y + leftY     )*4 + 2	]) 
								+ ( filter[ 3]   * input->memFloat[ ( leftX   *input->Y + leftY     )*4 + 3	]) 
								+ ( filter[ 4]   * input->memFloat[ ( leftX   *input->Y + leftY  + 1)*4		]) 
								+ ( filter[ 5]   * input->memFloat[ ( leftX   *input->Y + leftY  + 1)*4 + 1	]) 
								+ ( filter[ 6]   * input->memFloat[ ( leftX   *input->Y + leftY  + 1)*4 + 2	]) 
								+ ( filter[ 7]   * input->memFloat[ ( leftX   *input->Y + leftY  + 1)*4 + 3	]) 
								+ ( filter[ 8]   * input->memFloat[ ( leftX   *input->Y + leftY  + 2)*4		]) 
								+ ( filter[ 9]   * input->memFloat[ ( leftX   *input->Y + leftY  + 2)*4 + 1	]) 
								+ ( filter[ 10]  * input->memFloat[ ( leftX   *input->Y + leftY  + 2)*4 + 2	]) 
								+ ( filter[ 11]  * input->memFloat[ ( leftX   *input->Y + leftY  + 2)*4 + 3	]) 
								+ ( filter[ 12]  * input->memFloat[ ((leftX+1)*input->Y + leftY  )*4 		]) 
								+ ( filter[ 13]  * input->memFloat[ ((leftX+1)*input->Y + leftY  )*4 + 1	]) 
								+ ( filter[ 14]  * input->memFloat[ ((leftX+1)*input->Y + leftY  )*4 + 2	]) 
								+ ( filter[ 15]  * input->memFloat[ ((leftX+1)*input->Y + leftY  )*4 + 3	]) 
								+ ( filter[ 16]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 1)*4		]) 
								+ ( filter[ 17]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 1)*4 + 1	])  
								+ ( filter[ 18]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 1)*4 + 2	]) 
								+ ( filter[ 19]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 1)*4 + 3	]) 
								+ ( filter[ 20]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 2)*4 	]) 
								+ ( filter[ 21]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 2)*4 + 1	]) 
								+ ( filter[ 22]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 2)*4 + 2	]) 
								+ ( filter[ 23]  * input->memFloat[ ((leftX+1)*input->Y + leftY  + 2)*4 + 3	]) 
								+ ( filter[ 24]  * input->memFloat[ ((leftX+2)*input->Y + leftY     )*4 	]) 
								+ ( filter[ 25]  * input->memFloat[ ((leftX+2)*input->Y + leftY     )*4 + 1	]) 
								+ ( filter[ 26]  * input->memFloat[ ((leftX+2)*input->Y + leftY     )*4 + 2	]) 
								+ ( filter[ 27]  * input->memFloat[ ((leftX+2)*input->Y + leftY     )*4 + 3	]) 
								+ ( filter[ 28]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 1)*4  	]) 
								+ ( filter[ 29]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 1)*4 + 1	]) 
								+ ( filter[ 30]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 1)*4 + 2	]) 
								+ ( filter[ 31]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 1)*4 + 3	]) 
								+ ( filter[ 32]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 2)*4  	]) 
								+ ( filter[ 33]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 2)*4 + 1	]) 
								+ ( filter[ 34]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 2)*4 + 2	]) 
								+ ( filter[ 35]  * input->memFloat[ ((leftX+2)*input->Y + leftY  + 2)*4 + 3	]);  				
							break;
					}
				}

				else
#endif
				{
					/* convolution */
					for(ki = leftX; ki < rightX; ki++){
						for(kj = leftY; kj < rightY; kj++){
							for(kk = 0; kk < input->Z; kk++)
							{

								sum += filter[ ((kX2 + ki - i)*con->kY + kY2 + kj - j)*input->Z + kk ] *
									input->memFloat[ (ki*input->Y + kj )*input->Z + kk ];
							}}}}

				sum = sum + features->bias[k];

				/* batch normalization 1 */
				sum = (sum - features->mean1[k])*features->std1[k]*features->gamma1[k] + features->beta1[k];

				/* ReLu */
				sum = (0.0 > sum) ? 0.0: sum;

				/* batch normalization 2 */
				sum = (sum - features->mean2[k])*features->std2[k]*features->gamma2[k] + features->beta2[k];

				/* binarize */
				if(sum > 0)
				{
					index = (outi*output->Y + outj)*output->Z + bink;

					#ifdef MULTICORES
					#pragma omp critical
					#endif
					{
					output->memBin[index] |=  1<<shift;
					}
				}
				outj++;
			} 
			outi++;
		}
	}

	#ifdef MULTICORES	    
	}
	#endif	

	/* 32 filters applied */
	filters_done += 32;
}
#endif

