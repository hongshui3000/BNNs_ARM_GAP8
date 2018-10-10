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
static  uint8_t                     x_0;
static  uint8_t                     y_0;
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
	kX2 = con->kX / 2;
	kY2 = con->kY / 2;

	/* starting position */
	x_0 = kX2 - con->padX;
	y_0 = kY2 - con->padY;

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
	firstLayer_convolution_32filters_16bitFixPoint(input, features, output);
#elif defined(FIXPOINT_32BIT) 
	firstLayer_convolution_32filters_32bitFixPoint(input, features, output);
#else
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

		uint32_t i, j, k, ki, kj, kk, outi, outj;
		uint32_t outX, outY;
		uint32_t leftX, rightX, leftY, rightY;
		uint32_t shift, bink;
		int16_t *filter;
		int32_t sum;
		int32_t index;
		bink = filters_done / 32;

#ifdef MULTICORES	    
#pragma omp parallel shared(input, output, features,x_0,y_0, kX2,kY2, filters_done, con)\
		 private(k,i,j,ki,kj,kk,shift,rightX,leftX,rightY,leftY,index,outi,outj,sum, filter)   
	{
#endif	

#ifdef MULTICORES	    
#pragma omp for schedule(dynamic)
#endif	
		for(k = 0; k < 32; k++)
		{
			//	printf("f16 fun 2\n");
			filter = &(features->weight[k * con->kX * con->kY * input->Z]);
			//	printf("filter : %p, value : %d\n", filter, *filter);
			shift = k % 32;
			outi = 0;
			// move filter in x-direction 
			for(i = x_0; i < input->X - x_0; i += con->dX)
			{
				// max 
				leftX = (0 > (int32_t) i - kX2) ? 0: i - kX2;
				// min 
				rightX = (input->X < i + kX2+1) ? input->X: i + kX2+1;
				//		    printf("f16 fun 3.5\n");
				outj = 0;
				// move filter in y-direction
				for(j = y_0; j < input->Y - y_0; j += con->dY)
				{
					// max 
					leftY = (0 > (int32_t)j - kY2) ? 0: j - kY2;
					// min 
					rightY = (input->Y < j + kY2+1) ? input->Y: j + kY2+1;
					//			printf("f16 fun 4\n");
					sum = 0;
					// convolution 
#ifdef UNFOLDED
					if((rightX - leftX == 3) && (rightY - leftY == 3))
					{
						/*sum +=  (((int32_t)filter[  0]  * (int32_t)input->memFixpoint[  leftX   *input->Y + leftY    ])>> FP_SCALE_BITs) 
							+ (((int32_t)filter[1]  * (int32_t)input->memFixpoint[  leftX   *input->Y + leftY + 1])>> FP_SCALE_BITs) 
							+ (((int32_t)filter[2]  * (int32_t)input->memFixpoint[  leftX   *input->Y + leftY + 2])>> FP_SCALE_BITs) 
							+ (((int32_t)filter[3]  * (int32_t)input->memFixpoint[ (leftX+1)*input->Y + leftY    ])>> FP_SCALE_BITs) 
							+ (((int32_t)filter[4]  * (int32_t)input->memFixpoint[ (leftX+1)*input->Y + leftY + 1])>> FP_SCALE_BITs) 
							+ (((int32_t)filter[5]  * (int32_t)input->memFixpoint[ (leftX+1)*input->Y + leftY + 2])>> FP_SCALE_BITs) 
							+ (((int32_t)filter[6]  * (int32_t)input->memFixpoint[ (leftX+2)*input->Y + leftY    ])>> FP_SCALE_BITs) 
							+ (((int32_t)filter[7]  * (int32_t)input->memFixpoint[ (leftX+2)*input->Y + leftY + 1])>> FP_SCALE_BITs) 
							+ (((int32_t)filter[8]  * (int32_t)input->memFixpoint[ (leftX+2)*input->Y + leftY + 2])>> FP_SCALE_BITs); */
						sum +=  (((int32_t)filter[  0]  * (int32_t)input->memFixpoint[  leftX   *input->Y + leftY    ])) 
							+ (((int32_t)filter[1]  * (int32_t)input->memFixpoint[  leftX   *input->Y + leftY + 1])) 
							+ (((int32_t)filter[2]  * (int32_t)input->memFixpoint[  leftX   *input->Y + leftY + 2])) 
							+ (((int32_t)filter[3]  * (int32_t)input->memFixpoint[ (leftX+1)*input->Y + leftY    ])) 
							+ (((int32_t)filter[4]  * (int32_t)input->memFixpoint[ (leftX+1)*input->Y + leftY + 1])) 
							+ (((int32_t)filter[5]  * (int32_t)input->memFixpoint[ (leftX+1)*input->Y + leftY + 2])) 
							+ (((int32_t)filter[6]  * (int32_t)input->memFixpoint[ (leftX+2)*input->Y + leftY    ])) 
							+ (((int32_t)filter[7]  * (int32_t)input->memFixpoint[ (leftX+2)*input->Y + leftY + 1])) 
							+ (((int32_t)filter[8]  * (int32_t)input->memFixpoint[ (leftX+2)*input->Y + leftY + 2])); 
						sum = sum >> FP_SCALE_BITs;

								/*for(kk=0;kk<3;kk++)
								{
							        	v2s Vin = *((v2s *) (&input->memFixpoint[(leftX+kk)*input->Y+ leftY])); // Input vector
						    			v2s Vc  = *((v2s *) (&filter[kk*3])); // Coefficients accessed as vectors
						     			// We use a built-in to evaluate 4 of the 5 products in 1 cycle
  						   			sum = (int32_t) __builtin_pulp_sdotsp2(Vin, Vc, sum)>> FP_SCALE_BITs;
  						  			// The last product is as usual 
  							        	sum += ((int32_t)input->memFixpoint[(leftX+kk)*input->Y+ leftY +2]*(int32_t)filter[kk*3+2])>> FP_SCALE_BITs; 
								}*/
					}
					else
#endif
						for(ki = leftX; ki < rightX; ki++){
							//printf("leftX rightX : ok");
							for(kj = leftY; kj < rightY; kj++){
								for(kk = 0; kk < input->Z; kk++)
								{
									//printf("f16 conv loop ok");
									sum += ((int32_t)filter[ ((kX2 + ki - i)*con->kY + kY2 + kj - j)*input->Z + kk ] *(int32_t)input->memFixpoint[ (ki*input->Y + kj )*input->Z + kk ]);
								}
							}
						}
					sum = sum >> FP_SCALE_BITs;
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

		uint32_t outi, outj;
		#ifdef SHIFT12
		int32_t sum;
		int64_t sum2;
		#else
		int64_t sum;
		#endif
		uint32_t i, j, k, ki, kj, kk;
		uint32_t leftX, rightX, leftY, rightY;
		uint32_t shift, bink;
		int32_t index;
		int32_t *filter;
		//bink = filters_done / 32;
		#ifdef MULTICORES
		rt_omp_start();
		#endif
#ifdef MULTICORES	    
		#ifdef SHIFT12
#pragma omp parallel shared(input, output, features,x_0,y_0, kX2,kY2, filters_done, con)\
		 private(k,i,j,ki,kj,kk,shift,rightX,leftX,rightY,leftY,index,outi,outj,sum, sum2,filter, bink)// num_threads(8)
	{
		#else
#pragma omp parallel default(shared) private(k,i,j,ki,kj,kk,shift,rightX,leftX,rightY,leftY,index,outi,outj,sum,filter, bink) num_threads(CORE_NUM_SPEC)
	{
		#endif
#endif	
#ifdef MULTICORES	    
#pragma omp for schedule(dynamic)
#endif	
		for(k = 0; k < 32; k++)
		{
			bink = filters_done / 32;
			filter = &(features->weight[k*con->kX*con->kY*input->Z]);
		//printf("(%d, %d) FIrst layer Entered PE with arg \n", rt_cluster_id(), rt_core_id());
			shift = k % 32;
			outi = 0;
			/* move filter in x-direction */
			for(i = x_0; i < input->X - x_0; i += con->dX)
			{
				/* max */
				leftX = (0 > (int32_t) i - kX2) ? 0: i - kX2;
				/* min */
				rightX = (input->X < i + kX2+1) ? input->X: i + kX2+1;
				outj = 0;
				/* move filter in y-direction */
				for(j = y_0; j < input->Y - y_0; j += con->dY)
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
	#ifdef INT_INPUT
								sum += ((( (int32_t)filter[  0] * (uint8_t)input->memFixpoint[ leftX   *input->Y + leftY    ])) 
									+ (((int32_t)filter[1]  * (uint8_t)input->memFixpoint[  leftX   *input->Y + leftY + 1])) 
									+ (((int32_t)filter[2]  * (uint8_t)input->memFixpoint[  leftX   *input->Y + leftY + 2])) 
									+ (((int32_t)filter[3]  * (uint8_t)input->memFixpoint[ (leftX+1)*input->Y + leftY    ])) 
									+ (((int32_t)filter[4]  * (uint8_t)input->memFixpoint[ (leftX+1)*input->Y + leftY + 1])) 
									+ (((int32_t)filter[5]  * (uint8_t)input->memFixpoint[ (leftX+1)*input->Y + leftY + 2])) 
									+ (((int32_t)filter[6]  * (uint8_t)input->memFixpoint[ (leftX+2)*input->Y + leftY    ])) 
									+ (((int32_t)filter[7]  * (uint8_t)input->memFixpoint[ (leftX+2)*input->Y + leftY + 1])) 
									+ (((int32_t)filter[8]  * (uint8_t)input->memFixpoint[ (leftX+2)*input->Y + leftY + 2]))); 
								//sum = sum >> FP_SCALE_BITs;
								sum = sum >> FP_SCALE_BIT_INs;
	#else
								sum += ((( (int64_t)filter[  0] * (int64_t)input->memFixpoint[ leftX   *input->Y + leftY    ])) 
									+ (((int64_t)filter[1]  * (int64_t)input->memFixpoint[  leftX   *input->Y + leftY + 1])) 
									+ (((int64_t)filter[2]  * (int64_t)input->memFixpoint[  leftX   *input->Y + leftY + 2])) 
									+ (((int64_t)filter[3]  * (int64_t)input->memFixpoint[ (leftX+1)*input->Y + leftY    ])) 
									+ (((int64_t)filter[4]  * (int64_t)input->memFixpoint[ (leftX+1)*input->Y + leftY + 1])) 
									+ (((int64_t)filter[5]  * (int64_t)input->memFixpoint[ (leftX+1)*input->Y + leftY + 2])) 
									+ (((int64_t)filter[6]  * (int64_t)input->memFixpoint[ (leftX+2)*input->Y + leftY    ])) 
									+ (((int64_t)filter[7]  * (int64_t)input->memFixpoint[ (leftX+2)*input->Y + leftY + 1])) 
									+ (((int64_t)filter[8]  * (int64_t)input->memFixpoint[ (leftX+2)*input->Y + leftY + 2]))); 
								//sum = sum >> FP_SCALE_BITs;
								sum = sum >> FP_SCALE_BIT_INs;

	#endif
/*								sum += (( (int64_t)filter[  0] * (int64_t)input->memFixpoint[ leftX   *input->Y + leftY    ])>> FP_SCALE_BITs) 
									+ (((int64_t)filter[1]  * (int64_t)input->memFixpoint[  leftX   *input->Y + leftY + 1])>> FP_SCALE_BITs) 
									+ (((int64_t)filter[2]  * (int64_t)input->memFixpoint[  leftX   *input->Y + leftY + 2])>> FP_SCALE_BITs) 
									+ (((int64_t)filter[3]  * (int64_t)input->memFixpoint[ (leftX+1)*input->Y + leftY    ])>> FP_SCALE_BITs) 
									+ (((int64_t)filter[4]  * (int64_t)input->memFixpoint[ (leftX+1)*input->Y + leftY + 1])>> FP_SCALE_BITs) 
									+ (((int64_t)filter[5]  * (int64_t)input->memFixpoint[ (leftX+1)*input->Y + leftY + 2])>> FP_SCALE_BITs) 
									+ (((int64_t)filter[6]  * (int64_t)input->memFixpoint[ (leftX+2)*input->Y + leftY    ])>> FP_SCALE_BITs) 
									+ (((int64_t)filter[7]  * (int64_t)input->memFixpoint[ (leftX+2)*input->Y + leftY + 1])>> FP_SCALE_BITs) 
									+ (((int64_t)filter[8]  * (int64_t)input->memFixpoint[ (leftX+2)*input->Y + leftY + 2])>> FP_SCALE_BITs); 


								for(kk=0;kk<3;kk++)
								{
							        	v2s Vin = *((v2s *) ((int32_t) &input->memFixpoint[(leftX+kk)*input->Y+ leftY])); // Input vector
						    			v2s Vc  = *((v2s *) (&filter[kk*3])); // Coefficients accessed as vectors
						     			// We use a built-in to evaluate 4 of the 5 products in 1 cycle
  						   			sum = (int64_t) __SUMDOTP2(Vin, Vc, sum)>> FP_SCALE_BITs;
  						  			// The last product is as usual 
  							        	sum += ((int64_t)input->memFixpoint[(leftX+kk)*input->Y+ leftY +2]*(int64_t)filter[kk*3+2])>> FP_SCALE_BITs; 
								}

*/
					}
					else
#endif
					{	
	#ifdef INT_INPUT
						for( ki = leftX; ki < rightX; ki++){
							for( kj = leftY; kj < rightY; kj++){
								for( kk = 0; kk < input->Z; kk++)
								{
									//sum += ((int64_t)filter[ ((kX2 + ki - i)*con->kY + kY2 + kj - j)*input->Z + kk ] *
//											(int64_t)input->memFixpoint[ (ki*input->Y + kj )*input->Z + kk ]) >> FP_SCALE_BITs;
									sum += ((int32_t)filter[ ((kX2 + ki - i)*con->kY + kY2 + kj - j)*input->Z + kk ] *
											(uint8_t)input->memFixpoint[ (ki*input->Y + kj )*input->Z + kk ]);
								}
							}
						}
						//sum = sum >> FP_SCALE_BITs;
						sum = sum >> FP_SCALE_BIT_INs;
	#else
						for( ki = leftX; ki < rightX; ki++){
							for( kj = leftY; kj < rightY; kj++){
								for( kk = 0; kk < input->Z; kk++)
								{
									/*sum += ((int64_t)filter[ ((kX2 + ki - i)*con->kY + kY2 + kj - j)*input->Z + kk ] *
											(int64_t)input->memFixpoint[ (ki*input->Y + kj )*input->Z + kk ]) >> FP_SCALE_BITs;*/
									sum += ((int64_t)filter[ ((kX2 + ki - i)*con->kY + kY2 + kj - j)*input->Z + kk ] *
										(int64_t)input->memFixpoint[ (ki*input->Y + kj )*input->Z + kk ]);
								}
							}
						}
						//sum = sum >> FP_SCALE_BITs;
						sum = sum >> FP_SCALE_BIT_INs;
	#endif
					}
					sum = sum + features->bias[k];

					/* batch normalization 1 */
					#ifdef SHIFT12
					//sum =(int32_t) (((int64_t)((int32_t)(((int64_t)((int32_t)(sum - features->mean1[k])*(int32_t)features->std1[k])) >> FP_SCALE_BITs) * (int32_t)features->gamma1[k])) >> FP_SCALE_BITs) + features->beta1[k];
					sum = (((((((sum - features->mean1[k])*features->std1[k])) >> FP_SCALE_BITs) * features->gamma1[k])) >> FP_SCALE_BITs) + features->beta1[k];
					#else
					sum = (((((((sum - features->mean1[k])*features->std1[k])) >> FP_SCALE_BITs) * features->gamma1[k])) >> FP_SCALE_BITs) + features->beta1[k];
					#endif

					/* ReLu */
					sum = (0 > sum) ? 0: sum;

					/* batch normalization 2 */
					#ifdef SHIFT12
					sum2 = (((((sum - features->mean2[k])*features->std2[k]) >> FP_SCALE_BITs) * features->gamma2[k]) >> FP_SCALE_BITs) + features->beta2[k];
					#else
					sum = (((((sum - features->mean2[k])*features->std2[k]) >> FP_SCALE_BITs) * features->gamma2[k]) >> FP_SCALE_BITs) + features->beta2[k];
					#endif
					//printf("sum2\n");
					/* binarize */
					#ifdef SHIFT12
					if(sum2 > 0)
					#else
					if(sum > 0)
					#endif
					{
						index = (outi*output->Y + outj)*output->Z + bink;
						//printf("output->Y = %ld\n", output->Y);
	
#ifdef MULTICORES
#pragma omp critical
#endif
						{

//printf("first layer index %ld \n", index );
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

//#ifdef MULTICORES	    
//#pragma omp parallel shared(input, output) 	shared(features) shared( x_0, y_0, kX2, kY2)    
//	{
//#endif	
		uint32_t i, j, k, ki, kj, kk, outi, outj;
		uint32_t outX, outY;
		uint32_t leftX, rightX, leftY, rightY;
		uint32_t shift, bink;
		int32_t index;
		float *filter;
		float sum;

//#ifdef MULTICORES	    
//#pragma omp for schedule(static)
//#endif	
#ifdef MULTICORES	    
#pragma omp parallel shared(input, output, features,x_0,y_0, kX2,kY2, filters_done, con)\
		 private(k,i,j,ki,kj,kk,shift,rightX,leftX,rightY,leftY,index,outi,outj,sum, filter, bink)
	{
#endif	
#ifdef MULTICORES	    
#pragma omp for schedule(dynamic)
#endif	
		for(k = 0; k < 32; k++)
		{
			bink = filters_done / 32;
			filter = &(features->weight[k * con->kX * con->kY * input->Z]);

			shift = k % 32;
			outi = 0;
			/* move filter in x-direction */
			for(i = x_0; i < input->X - x_0; i += con->dX)
			{
				/* max */
				leftX = (0 > (int32_t) i - kX2) ? 0: i - kX2;
				/* min */
				rightX = (input->X < i + kX2+1) ? input->X: i + kX2+1;
				outj = 0;
				/* move filter in y-direction */
				for(j = y_0; j < input->Y - y_0; j += con->dY)
				{
					/* max */
					leftY = (0 > (int32_t)j - kY2) ? 0: j - kY2;
					/* min */
					rightY = (input->Y < j + kY2+1) ? input->Y: j + kY2+1;
					sum = 0;
#ifdef UNFOLDED
					if((rightX - leftX == 3) && (rightY - leftY == 3))
					{
						sum += ( filter[  0] * input->memFloat[ leftX   *input->Y + leftY    ]) 
							+ ( filter[  + 1]   * input->memFloat[  leftX   *input->Y + leftY + 1]) 
							+ ( filter[  + 2]   * input->memFloat[  leftX   *input->Y + leftY + 2]) 
							+ ( filter[  + 3]   * input->memFloat[ (leftX+1)*input->Y + leftY    ]) 
							+ ( filter[  + 4]   * input->memFloat[ (leftX+1)*input->Y + leftY + 1]) 
							+ ( filter[  + 5]   * input->memFloat[ (leftX+1)*input->Y + leftY + 2]) 
							+ ( filter[  + 6]   * input->memFloat[ (leftX+2)*input->Y + leftY    ]) 
							+ ( filter[  + 7]   * input->memFloat[ (leftX+2)*input->Y + leftY + 1]) 
							+ ( filter[  + 8]   * input->memFloat[ (leftX+2)*input->Y + leftY + 2]); 
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
								}
							}
						}
					}

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

