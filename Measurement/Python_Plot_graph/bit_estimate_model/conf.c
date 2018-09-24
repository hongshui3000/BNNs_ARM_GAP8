

/* descrption:	resize input 
 *
 *	option: fix-point values (16 bit)
 */

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>

//#include "input_generic.dat"
//#include "model_mymodel.dat"
//#include "model_mymodel_F_07_05.dat"
#include "model_mymodel_F_07_01.dat"


/******************************
 * OPTIONS
 ******************************/
#define FIXPOINT_16BIT 				/*< if set, input will be fix-point 16bit instead of floating point */
//#define FIXPOINT_32BIT				/*< if set, input will be fix-point 32bit instead of floating point */

/******************************
 * OPTIONS
 ******************************/
#ifdef FIXPOINT_16BIT
#define FIXPOINT_SCALE	512.0	/*< scaling factor for fixpoint */
#elif defined(FIXPOINT_32BIT)
#define FIXPOINT_SCALE  1048576.0
#endif

#define SIZE 	400				/*< number of input-y values */
#define BINS 	64				/*< number of input-x values */


static uint32_t bitcounter[32];	/*< count how many values use bit position */

int main(int argc, char *argv[])
{
	int i,j,k;
	int32_t bit;
	char str[20];

	for(i=0;i<32;i++)
	{
		bitcounter[i] = 0;
	}

	FILE* file = fopen("model_mymodel_F_07_01.csv", "w");
	printf("size of fData %d\n",sizeof(fData)/4);
	/*fputs("const unsigned int MFCC_BINS = ", file);
	  sprintf(str, "%u", BINS);
	  fputs(str, file);
	  fputs("; \n", file);	
	  fputs("const unsigned int IMG_WIDTH = ", file);
	  sprintf(str, "%u", SIZE);
	  fputs(str, file);
	  fputs("; \n", file);*/

#ifdef FIXPOINT_16BIT
	int32_t fixpoint;
	uint32_t checkvalue;
	float ff;
	//	fputs("const int16_t mfcc[64][", file);
	//	fputs(str, file);
	//	fputs("] = {", file);

	//	for(i = 0; i < BINS; i++)
	//	{
	//fputs("{", file);
	for(j = 0; j < 4956; j++)
	{

		/* get bit statistics */
		for(k=8; k<20; k++)
		{
			bit = ((int) fData[j]);
			if(bit < 0)
			{
				bit = bit * (-1);
			}
			bit = bit  &  ((unsigned int)0x1) << (k-8);
			if(bit > 0)
			{
				// printf("asdjfl\n");
				bitcounter[k] += 1; 
				// break;
			}
		}


		fixpoint = (int32_t) (fData[j] * FIXPOINT_SCALE);
		checkvalue = fixpoint & 0xFFFF0000;
		if((checkvalue > 0 ) && (checkvalue != 0xFFFF0000))
		{
			printf("16 bit overflow: %x \n", (int) (fixpoint>>16));
		}
		else if((checkvalue < 0)&& (checkvalue != 0xFFFF0000))
		{
			printf("16 bit overflow minus: %x \n", (int) (fixpoint>>16));
		}
//		sprintf(str, "%d", fixpoint);
//		fputs(str, file);
//		if(j != SIZE-1)
//		{
//			fputs(",", file);
//		}

		/* get bit statistics */
		for(k=0; k<8; k++)
		{
			bit = fixpoint & ( ((unsigned int)0x1) << k);
			if(bit > 0)
			{
				// printf("asdjfl\n");
				bitcounter[k] += 1; 
				// break;
			}
		}

	}
	//fputs("}", file);
	/*if(i != BINS-1)
	  {
	  fputs(",", file);
	  }*/
	//	}
	//fputs("};",file);

#elif defined(FIXPOINT_32BIT)
	int64_t fixpoint;
	uint64_t checkvalue;
	//fputs("const int32_t fData[4956] = {", file);
	for(j = 0; j < 4956; j++)
	{

		// get bit statistics 
		for(k=20; k<32; k++)
		{
			bit = ((int) fData[j]);
			if(bit < 0)
			{
				bit = bit * (-1);
			}
			bit = bit  &  ((unsigned int)0x1) << (k-20);
			if(bit > 0)
			{
				// printf("asdjfl\n");
				bitcounter[k] += 1; 
				// break;
			}
		}

		fixpoint = (int64_t) (fData[j] * FIXPOINT_SCALE);
		checkvalue = fixpoint & 0xFFFFFFFF00000000;
		if((checkvalue > 0 ) && (checkvalue != 0xFFFFFFFF00000000))
		{
			printf("32 bit overflow: %x \n", (int) (fixpoint>>32));
		}
		else if((checkvalue < 0)&& (checkvalue != 0xFFFFFFFF00000000))
		{
			printf("32 bit overflow minus: %x \n", (int) (fixpoint>>32));
		}
		//sprintf(str, "%d", (int32_t)fixpoint);
		//fputs(str, file);
		/*if(j != 4955)
		  {
		  fputs(",", file);
		  }*/

		// get bit statistics 
		for(k=0; k<20; k++)
		{
			bit = fixpoint & ( ((unsigned int)0x1) << k);
			if(bit > 0)
			{
				// printf("asdjfl\n");
				bitcounter[k] += 1; 
				// break;
			}
		}


	}
	/*fputs("}", file);
	  if(i != BINS-1)
	  {
	  fputs(",", file);
	  }*/

	//fputs("};",file);	

#else // floating-point 
	fputs("const float mfcc[64][", file);
	fputs(str, file);
	fputs("] = {", file);

	for(i = 0; i < BINS; i++)
	{
		fputs("{", file);
		for(j = 0; j < SIZE; j++)
		{

			sprintf(str, "%f", mfcc[i][j]);
			fputs(str, file);
			if(j != SIZE-1)
			{
				fputs(",", file);
			}
		}
		fputs("}", file);
		if(i != BINS-1)
		{
			fputs(",", file);
		}
	}
	fputs("};",file);
#endif

	bit = 0;
	for(k=0; k<32; k++)
	{
		bit += bitcounter[k];
		printf("%d %d\n", k, bitcounter[k]);
		sprintf(str, "%d", k);
		fputs(str, file);
		fputs(",", file);
		sprintf(str, "%u", bitcounter[k]);
		fputs(str, file);
		fputs("\n", file);
	}
	fclose(file);
	printf("total %d  \n", bit );
	printf("..Done \n");
	return 0;
}

