/**
 * \file FirstLayer.c
 * \brief
 * \date 18.01.2017
 * 
 * \details 
 */ 									
/*					| input window  |	address		  			|	x  	|	y 	|
 * ------------------------------------------------------------------------------------
 *	Binary memory  	|		1		| 	0 - #bin-ch -1 			|	0	|	0   |
 *-------------------------------------------------------------------------------------
 *					|		2		| 1*#bin-ch - 2*#bin-ch -1	|	0	|   1   |
 *-------------------------------------------------------------------------------------
 *					|		3		| 2*#bin-ch - 3*#bin-ch -1	|	0	|   2   |
 *-------------------------------------------------------------------------------------
 *					|		..		| ...						|	..	|   ..  |
 *	
 * 	nFilters = 32 * #binary-channels (bin-ch)
 */

/**
 * \defgroup Layer <DESCRIPTION>
 * [\addtogroup] Layer
 * @{
 */

/* ==================================================================================
 * Includes
 * ================================================================================*/
#include "BinaryLayer.h"

/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/
/* ==================================================================================
 * Local/Global variables definitions
 * ================================================================================*/
/* ==================================================================================
 * Local function declarations
 * ================================================================================*/

//static PLP_L1_DATA uint8_t 			kX2;
//static PLP_L1_DATA uint8_t 			kY2;
//static PLP_L1_DATA uint8_t 			x0;
//static PLP_L1_DATA uint8_t 			y0;
//static PLP_L1_DATA LAYER_CONFIG* 	con;

//static PLP_L1_DATA uint32_t		filters_done;

static uint8_t                      kX2;
static uint8_t                      kY2;
static uint8_t                      x0;
static uint8_t                      y0;
static LAYER_CONFIG*        con;
static uint32_t             filters_done;

/* ==================================================================================
 * Local function implementations
 * ================================================================================*/
/* ==================================================================================
 * Global function implementations
 * ================================================================================*/

	extern void binaryLayer_setConfiguration
(
 LAYER_CONFIG* config
 )
{
	/*TODO: check layer type */
	con = config;

	/* Half size of the filters */
	kX2 = con->kX / 2;
	kY2 = con->kY / 2;

	/* starting position */
	x0 = kX2 - con->padX;
	y0 = kY2 - con->padY;

	filters_done = 0;
}

	extern void binaryLayer_convolution_32filters
(
 TENSOR* input,
 BIN_FEATURES*  features,
 TENSOR* output
 )
{

#ifdef MULTICORES	
#pragma omp parallel shared(input, output) shared(features) shared( x0, y0, kX2, kY2, filters_done)
	{
#endif
		uint32_t i, j, k;
		uint32_t ki, kj, kk;
		uint32_t leftX, rightX, leftY, rightY; 
		uint32_t shift;
		uint32_t outi, outj; 
		int32_t  sum = 0;
		uint32_t *filter;
		int32_t filter_index, io_index;
		uint32_t X_diff, Y_diff;
		uint32_t bink = filters_done / 32;

#ifdef MULTICORES
#pragma omp for schedule(static) 
#endif
		for(k = 0; k < 32; k++)
		{
			/* get current filter */
			filter = &(features->weight[k * con->kX * con->kY * input->Z]);

			outi = 0;
			shift = k % 32;

			/* move filter in x-direction */
			for(i = x0; i < input->X - x0; i += con->dX)
			{
				/* max */
				leftX = (0 > (int32_t) i - kX2) ? 0: i - kX2;
				/* min */
				rightX = (input->X < i + kX2+1) ? input->X: i + kX2+1;
				X_diff = rightX - leftX;
				outj = 0;
				/* move filter in y-direction */
				for(j = y0; j < input->Y - y0; j += con->dY)
				{
					/* max */
					leftY = (0 > (int32_t)j - kY2) ? 0: j - kY2;
					/* min */
					rightY = (input->Y < j + kY2+1) ? input->Y: j + kY2+1;
					Y_diff = rightY - leftY;
					/* binary convolution */
#ifdef UNFOLDED
					if((X_diff == 3) && (Y_diff == 3))
					{
						sum=0;
						switch(input->Z)
						{
							/* fitler = 1 */
							case 0x1: 		/* input->Z = 1 */
								sum += 144; /* 9*16 		*/
								sum -= (popcount( filter[  0] ^ input->memBin[ leftX   *input->Y + leftY    ]) 
										+ popcount( filter[ 1] ^ input->memBin[  leftX   *input->Y + leftY + 1]) 
										+ popcount( filter[ 2] ^ input->memBin[  leftX   *input->Y + leftY + 2]) 
										+ popcount( filter[ 3] ^ input->memBin[ (leftX+1)*input->Y + leftY    ]) 
										+ popcount( filter[ 4] ^ input->memBin[ (leftX+1)*input->Y + leftY + 1]) 
										+ popcount( filter[ 5] ^ input->memBin[ (leftX+1)*input->Y + leftY + 2]) 
										+ popcount( filter[ 6] ^ input->memBin[ (leftX+2)*input->Y + leftY    ]) 
										+ popcount( filter[ 7] ^ input->memBin[ (leftX+2)*input->Y + leftY + 1]) 
										+ popcount( filter[ 8] ^ input->memBin[ (leftX+2)*input->Y + leftY + 2])); 
								break;
							case 0x2: 
								sum += 288; /* 18*16 */
								sum -= (popcount( filter[0] ^ input->memBin[( leftX   *input->Y + leftY  )*2 		]) 
										+ popcount( filter[1] ^ input->memBin[ ( leftX   *input->Y + leftY )*2 + 1	]) 
										+ popcount( filter[2] ^ input->memBin[ ( leftX   *input->Y + leftY  + 1)*2	]) 
										+ popcount( filter[3] ^ input->memBin[ ( leftX   *input->Y + leftY + 1)*2 + 1	]) 
										+ popcount( filter[4] ^ input->memBin[ ( leftX   *input->Y + leftY + 2)*2 	]) 
										+ popcount( filter[5] ^ input->memBin[ ( leftX   *input->Y + leftY + 2)*2 + 1	]) 
										+ popcount( filter[6] ^ input->memBin[ ((leftX+1)*input->Y + leftY )*2 		]) 
										+ popcount( filter[7] ^ input->memBin[ ((leftX+1)*input->Y + leftY )*2 + 1	]) 
										+ popcount( filter[8] ^ input->memBin[ ((leftX+1)*input->Y + leftY + 1)*2 	]) 
										+ popcount( filter[9] ^ input->memBin[ ((leftX+1)*input->Y + leftY + 1)*2 + 1	]) 
										+ popcount( filter[10] ^ input->memBin[ ((leftX+1)*input->Y + leftY + 2)*2	]) 
										+ popcount( filter[11] ^ input->memBin[ ((leftX+1)*input->Y + leftY + 2)*2 + 1	]) 
										+ popcount( filter[12] ^ input->memBin[ ((leftX+2)*input->Y + leftY )*2 	]) 
										+ popcount( filter[13] ^ input->memBin[ ((leftX+2)*input->Y + leftY )*2 + 1]) 
										+ popcount( filter[14] ^ input->memBin[ ((leftX+2)*input->Y + leftY + 1)*2	]) 
										+ popcount( filter[15] ^ input->memBin[ ((leftX+2)*input->Y + leftY + 1)*2 + 1	]) 
										+ popcount( filter[16] ^ input->memBin[ ((leftX+2)*input->Y + leftY + 2)*2	]) 
										+ popcount( filter[17] ^ input->memBin[ ((leftX+2)*input->Y + leftY + 2)*2 + 1	])); 
								break;
							case 0x4: 
								sum = 576; /* 36*16 */
								sum -=  ( popcount( filter[  0]^ input->memBin[ ( leftX *input->Y + leftY )*4]) 
										+ popcount( filter[  1]^ input->memBin[ ( leftX *input->Y + leftY )*4 + 1	]) 
										+ popcount( filter[  2]^ input->memBin[ ( leftX *input->Y + leftY )*4 + 2	]) 
										+ popcount( filter[  3]^ input->memBin[ ( leftX *input->Y + leftY )*4 + 3	]) 
										+ popcount( filter[  4]^ input->memBin[ ( leftX *input->Y + leftY + 1)*4	]) 
										+ popcount( filter[  5]^ input->memBin[ ( leftX *input->Y + leftY + 1)*4 + 1	]) 
										+ popcount( filter[  6]^ input->memBin[ ( leftX *input->Y + leftY + 1)*4 + 2	]) 
										+ popcount( filter[  7]^ input->memBin[ ( leftX *input->Y + leftY + 1)*4 + 3	]) 
										+ popcount( filter[  8]^ input->memBin[ ( leftX *input->Y + leftY + 2)*4        ]) 
										+ popcount( filter[  9]^ input->memBin[ ( leftX *input->Y + leftY + 2)*4 + 1	]) 
										+ popcount( filter[ 10]^ input->memBin[ ( leftX *input->Y + leftY  + 2)*4 + 2	]) 
										+ popcount( filter[ 11]^ input->memBin[ ( leftX *input->Y + leftY  + 2)*4 + 3	]) 
										+ popcount( filter[ 12]^ input->memBin[ ((leftX+1)*input->Y + leftY     )*4 	]) 
										+ popcount( filter[ 13]^ input->memBin[ ((leftX+1)*input->Y + leftY     )*4 + 1	]) 
										+ popcount( filter[ 14]^ input->memBin[ ((leftX+1)*input->Y + leftY     )*4 + 2	]) 
										+ popcount( filter[ 15]^ input->memBin[ ((leftX+1)*input->Y + leftY     )*4 + 3	]) 
										+ popcount( filter[ 16]^ input->memBin[ ((leftX+1)*input->Y + leftY  + 1)*4  	]) 
										+ popcount( filter[ 17]^ input->memBin[ ((leftX+1)*input->Y + leftY  + 1)*4 + 1	])  
										+ popcount( filter[ 18]^ input->memBin[ ((leftX+1)*input->Y + leftY  + 1)*4 + 2	]) 
										+ popcount( filter[ 19]^ input->memBin[ ((leftX+1)*input->Y + leftY  + 1)*4 + 3	]) 
										+ popcount( filter[ 20]^ input->memBin[ ((leftX+1)*input->Y + leftY  + 2)*4  	]) 
										+ popcount( filter[ 21]^ input->memBin[ ((leftX+1)*input->Y + leftY  + 2)*4 + 1	]) 
										+ popcount( filter[ 22]^ input->memBin[ ((leftX+1)*input->Y + leftY  + 2)*4 + 2	]) 
										+ popcount( filter[ 23]^ input->memBin[ ((leftX+1)*input->Y + leftY  + 2)*4 + 3	]) 
										+ popcount( filter[ 24]^ input->memBin[ ((leftX+2)*input->Y + leftY     )*4 	]) 
										+ popcount( filter[ 25]^ input->memBin[ ((leftX+2)*input->Y + leftY     )*4 + 1	]) 
										+ popcount( filter[ 26]^ input->memBin[ ((leftX+2)*input->Y + leftY     )*4 + 2	]) 	
										+ popcount( filter[ 27]^ input->memBin[ ((leftX+2)*input->Y + leftY     )*4 + 3	]) 
										+ popcount( filter[ 28]^ input->memBin[ ((leftX+2)*input->Y + leftY  + 1)*4  	]) 
										+ popcount( filter[ 29]^ input->memBin[ ((leftX+2)*input->Y + leftY  + 1)*4 + 1	]) 
										+ popcount( filter[ 30]^ input->memBin[ ((leftX+2)*input->Y + leftY  + 1)*4 + 2	]) 
										+ popcount( filter[ 31]^ input->memBin[ ((leftX+2)*input->Y + leftY  + 1)*4 + 3	]) 
										+ popcount( filter[ 32]^ input->memBin[ ((leftX+2)*input->Y + leftY  + 2)*4  	]) 
										+ popcount( filter[ 33]^ input->memBin[ ((leftX+2)*input->Y + leftY  + 2)*4 + 1	]) 
										+ popcount( filter[ 34]^ input->memBin[ ((leftX+2)*input->Y + leftY  + 2)*4 + 2	]) 
										+ popcount( filter[ 35]^ input->memBin[ ((leftX+2)*input->Y + leftY  + 2)*4 + 3	])); 	
								break;
						}
					}
					else
#endif
					{
						sum = 0;
						for(ki = leftX; ki < rightX; ki++){
							for(kj = leftY; kj < rightY; kj++){
								for(kk = 0; kk < input->Z; kk++)
								{	
									filter_index = ((kX2 + ki - i)*con->kY + kY2 + kj - j)*input->Z + kk;
									io_index = (ki*input->Y + kj )*input->Z + kk; 
									sum += 16 - popcount( filter[ filter_index]^ input->memBin[ io_index]);//hardware popcount 
								}
							}
						}
					}
					sum *= 2;
					/* ReLU, Batch Normalization and Binarization using only compares */
					if(sum > features->relu[k])
					{
						if(features->signGamma[k] * sum > features->threshhold[k]) /*TODO: sing can be divided through threshold, and replaced*/
						{ 
							io_index = (outi * output->Y + outj)*output->Z + bink;

#ifdef MULTICORES
#pragma omp critical
#endif
							{
								output->memBin[io_index] |= 1<<shift; /*shared*/	
							}
						}
					} 
					else 
					{
						if(features->compare[k])
						{
							io_index = (outi * output->Y + outj)*output->Z + bink;

#ifdef MULTICORES
#pragma omp critical
#endif
							{
								output->memBin[io_index] |= 1<<shift; /*shared*/	
							}
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

	filters_done += 32;
}


