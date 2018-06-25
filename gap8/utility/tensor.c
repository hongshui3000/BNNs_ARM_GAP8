/**
 * \file tensor.c
 * \brief
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
#include "tensor.h"
/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/
/* ==================================================================================
 * Local/Global variables definitions
 * ================================================================================*/
/* ==================================================================================
 * Local function declarations
 * ================================================================================*/

static void tensor_resize(TENSOR* tensor, TENSOR_TYPE type, uint32_t x, uint32_t y, uint32_t z);

/* ==================================================================================
 * Local function implementations
 * ================================================================================*/
static void tensor_resize(TENSOR* tensor, TENSOR_TYPE type, uint32_t x, uint32_t y, uint32_t z)
{
	tensor->type =   type;
	tensor->X =     x;
	tensor->Y =     y;
	if(TENSOR_BINARY == tensor->type)
	{
		tensor->Z = z / 32;
	} 
	else 
	{
		tensor->Z = z;
	}	
}


/* ==================================================================================
 * Global function implementations
 * ================================================================================*/


extern TENSOR* tensor_allocate(TENSOR_TYPE type, uint32_t x, uint32_t y, uint32_t z)
{
	uint32_t size, i; 

	//    TENSOR* tensor = (TENSOR*)malloc(sizeof(TENSOR));
	TENSOR* tensor = (TENSOR*) rt_alloc(ALLOC_LOCATION,sizeof(TENSOR));
#ifdef DEBUG
	if(tensor == NULL)
	{
		printf("memory overflow xyz\n");
	}
#endif
	tensor_resize(tensor, type, x, y, z);
	size = tensor->X*tensor->Y*tensor->Z;

	tensor->memBin = 0;
	tensor->memFloat = 0;
	tensor->memFixpoint = 0;
	if(TENSOR_BINARY == tensor->type)
	{ 
		/* calloc is not 0-filling */
		#ifdef DEBUG
		rt_user_alloc_dump(rt_alloc_l1(0));
		#endif
		tensor->memBin = (uint32_t*) rt_alloc(ALLOC_LOCATION, size * sizeof(uint32_t));
		#ifdef DEBUG
		if(tensor->memBin == NULL)
		{	
			printf("Failed to allocate output in L1. Size: %ld Byte\n",size * sizeof(uint32_t));
		}
		#endif
		#ifdef MULTICORES
		#pragma omp parallel
				{
		#pragma omp for schedule(static)
		#endif
		for(i = 0; i < size; i++)
		{
			tensor->memBin[i] = 0;
		}
		#ifdef MULTICORES
				}
		#endif
		#ifdef DEBUG
		printf("%ld Byte \n",size*sizeof(uint32_t));
		#endif

	} 
	else if(TENSOR_FLOAT == tensor->type)
	{
		/* calloc is not 0-filling */
#ifdef DEBUG
		rt_user_alloc_dump(rt_alloc_l1(0));
#endif
		tensor->memFloat = (float*) rt_alloc(ALLOC_LOCATION, size*sizeof(float));
		//rt_user_alloc_dump(rt_alloc_l1(0));
#ifdef MULTICORES
#pragma omp parallel
		{
#pragma omp for schedule(static)
#endif
			for(i = 0; i < size; i++)
			{
				tensor->memFloat[i] = 0.0;
			}
#ifdef MULTICORES
		}
#endif
		#ifdef DEBUG
		printf("%ld Byte \n",size*sizeof(float));
		#endif

	}
	else /* tensor fix-point (16bit) */
	{
#ifdef FIXPOINT_16BIT
		tensor->memFixpoint = (int16_t*) rt_alloc(ALLOC_LOCATION, size*sizeof(int32_t));
#else
		#ifdef DEBUG
		rt_user_alloc_dump(rt_alloc_l1(0));
		#endif

		tensor->memFixpoint = (int32_t*) rt_alloc(ALLOC_LOCATION, size*sizeof(int32_t));

		#ifdef DEBUG
		if(tensor->memFixpoint == NULL)
		{	
			//printf("memory overflow\n");
			printf("Failed to allocate Fixpoint in L1. Size: %ld Byte\n",size * sizeof(uint32_t));
			//return -1;
		}
		#endif
		#ifdef MULTICORES
		#pragma omp parallel
				{
		#pragma omp for schedule(static)
		#endif
		for(i = 0; i < size; i++)
		{
			tensor->memFixpoint[i] = 0;
		}
		#ifdef MULTICORES
				}
		#endif
#endif
		#ifdef DEBUG
		printf("%ld Byte \n",size*sizeof(int32_t));
		#endif
	}
	return tensor;
}

extern TENSOR* tensor_alloc_featureMap(TENSOR* input, LAYER_CONFIG* config, uint32_t nFilters)
{
	uint32_t outX = (input->X + 2*config->padX - config->kX)/config->dX + 1;
	uint32_t outY = (input->Y + 2*config->padY - config->kY)/config->dY + 1;
	return tensor_allocate(TENSOR_BINARY, outX, outY, nFilters);
}


extern void tensor_free(TENSOR* tensor)
{
	uint32_t i;

	if(TENSOR_BINARY == tensor->type)
	{
		rt_free(ALLOC_LOCATION,tensor->memBin, tensor->X*tensor->Y*tensor->Z*sizeof(uint32_t));
		#ifdef DEBUG
		printf("%ld Byte\n", tensor->X*tensor->Y*tensor->Z*sizeof(uint32_t));
		#endif
		tensor->memBin = 0;
	} 
	else if(TENSOR_FLOAT == tensor->type)
	{
		rt_free(ALLOC_LOCATION,tensor->memFloat, tensor->X*tensor->Y*tensor->Z*sizeof(float));
		#ifdef DEBUG
		printf("%ld Byte\n", tensor->X*tensor->Y*tensor->Z*sizeof(float));
		#endif
		tensor->memFloat = 0;
	}
	else /* fix-point */
	{
#ifdef FIXPOINT_16BIT
		rt_free(ALLOC_LOCATION, tensor->memFixpoint, tensor->X*tensor->Y*tensor->Z*sizeof(int32_t));
#else
		rt_free(ALLOC_LOCATION, tensor->memFixpoint, tensor->X*tensor->Y*tensor->Z*sizeof(int32_t));
		#ifdef DEBUG
		printf("%ld Byte\n", tensor->X*tensor->Y*tensor->Z*sizeof(int32_t));
		#endif
#endif
		tensor->memFixpoint = 0;
	}
}
