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
    TENSOR* tensor = (TENSOR*)malloc(sizeof(TENSOR));

    tensor_resize(tensor, type, x, y, z);

    size = tensor->X*tensor->Y*tensor->Z;

    tensor->memBin = 0;
    tensor->memFloat = 0;

    if(TENSOR_BINARY == tensor->type)
    { 
        /* calloc is not 0-filling */
        tensor->memBin = (uint32_t*) malloc(size * sizeof(uint32_t));       

        #ifdef MULTICORES
        #pragma omp parallel for schedule(static)
        #endif
        for(i = 0; i < size; i++)
        {
            tensor->memBin[i] = 0;
        }
    } 
    else if(TENSOR_FLOAT == tensor->type)
    {
        /* calloc is not 0-filling */
        tensor->memFloat = (float*) malloc(size*sizeof(float)); 

        #ifdef MULTICORES
        #pragma omp parallel for schedule(static)
        #endif
        for(i = 0; i < size; i++)
        {
            tensor->memFloat[i] = 0;
        }
    }
    else /* tensor fix-point (16bit) */
    {
#ifdef FIXPOINT_16BIT
        tensor->memFixpoint = (int16_t*) malloc(size*sizeof(int32_t)); 
#else
        tensor->memFixpoint = (int32_t*) malloc(size*sizeof(int32_t)); 
#endif

        #ifdef MULTICORES
        #pragma omp parallel for schedule(static)
        #endif
        for(i = 0; i < size; i++)
        {
            tensor->memFixpoint[i] = 0;
        }
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
        free(tensor->memBin);
        tensor->memBin = 0;
    } 
    else if(TENSOR_FLOAT == tensor->type)
    {
        free(tensor->memFloat);
        tensor->memFloat = 0;
    }
    else /* fix-point */
    {
        free(tensor->memFixpoint);
        tensor->memFixpoint = 0;
    }
}
