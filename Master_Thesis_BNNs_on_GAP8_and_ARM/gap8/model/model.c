/**
 * \file model.c
 * \brief
 * \date 18.01.2017
 * 
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
 *			note: total 80kBytes 
 */ 
/**
 * \defgroup Model <DESCRIPTION>
 * [\addtogroup] Model
 * @{
 */

/* ==================================================================================
 * Includes
 * ================================================================================*/
#include <math.h>
#include <stdlib.h>
#include "../config.h"
#include "model.h"

#ifdef FIXPOINT_16BIT
//#include "model_fp16.dat"
#include "model_mymodel_f16.dat"
#elif defined(FIXPOINT_32BIT)
#ifdef MYPOPCOUNT
//#include "model_mypopcount_f32.dat" //old model parameter
	#ifdef SHIFT12
	#include "model_mypopcount_07_01_shift12.dat" //new model parameter  (62%
	#else
	#include "model_mypopcount_f32_07_01.dat"     //new model parameter  (62%
	#endif
#else
//#include "model_fp32.dat"
//#include "model_mymodel_f32.dat" //les new model (60%
	#ifdef SHIFT12
#include "model_mymodel_f32_07_01_shift12.dat" //new model parameter  (62%
	#else
#include "model_mymodel_f32_07_01.dat" //new model parameter  (62%
	#endif
#endif
#else
//#include "model_mymodel.dat"
#include "model_mymodel_07_01.dat"
//#include "model.dat"
#endif
/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/
#define INPUT_CHANNELS		1

/* layers */
#define BIN_LAYERS			4				/* set number of binary layers */
#define NOF_LAYERS			6				/* set number of all layers */
/* ==================================================================================
 * Local/Global variables definitions
 * ================================================================================*/

/* data position */
static uint32_t 	fpos 		= 0;
static uint32_t 	bpos 		= 0;

/*dma identifier*/
static rt_dma_copy_t		dma_cop;

static	rt_perf_t *perf2;
/* layers */
static  LAYER 				layers[NOF_LAYERS];

/*  kX, kY, padX, padY, dX, dY */
static	LAYER_CONFIG config_0 = { 3, 3, 1, 1, 1, 1};
static	LAYER_CONFIG config_1 = { 3, 3, 1, 1, 2, 2};
static	LAYER_CONFIG config_2 = { 3, 3, 1, 1, 1, 1};
static	LAYER_CONFIG config_3 = { 3, 3, 1, 1, 2, 2};
static	LAYER_CONFIG config_4 = { 3, 3, 1, 1, 1, 1};
static	LAYER_CONFIG* configs[BIN_LAYERS+1] = {
	&config_0,
	&config_1, 
	&config_2,
	&config_3, 
	&config_4
};

static uint32_t nOfFilters[5]	= {32,64,128,128,128};

/* run-time configuration */
static void (*setConfiguration[3])() = {
	firstLayer_setConfiguration,
	binaryLayer_setConfiguration,
	lastLayer_setConfiguration
};


/* ==================================================================================
 * Local function declarations
 * ================================================================================*/
static void model_l1_byte_alloc(uint8_t size);
/**
 * \brief			initialize model configuration  
 * @param[in]	classes		..prediction classes
 */
static void model_init_configuration( uint32_t classes );
/**
 * \brief			load first layer features (with dma)
 * @param[in]	features	..features to load with dma transfer
 * @param[in]	kxky		..filter area size, kx multipled with ky
 * @param[in]	channels	..input feature map number of channels 
 */
void model_load_firstLayer_32filters( FIRST_FEATURES* features, uint8_t kxky, uint32_t channels );
/**
 * \brief			free first layer features 
 * @param[in]	features	..features to free
 */
static void model_free_firstLayer( FIRST_FEATURES* features, uint8_t kxky, uint32_t channels );

/**
 * \brief			load binary layer features (with dma)
 * @param[in]	features	..features to load with dma transfer
 * @param[in]	kxky		..filter area size, kx multipled with ky
 * @param[in]	channels	..input feature map number of channels 
 */
static void model_load_binaryLayer_32filters(BIN_FEATURES* features, uint8_t kxky, uint32_t channels);
/**
 * \brief			free binary layer features 
 * @param[in]	features	..features to free
 */
static void model_free_binaryLayer( BIN_FEATURES* features, uint8_t kxky, uint32_t input_channels);

/**
 * \brief			load last layer features (with dma)
 * @param[in]	features	..features to load with dma transfer
 * @param[in]	channels	..input feature map number of channels 
 */
static void model_load_lastLayer(LAST_FEATURES* features, uint32_t channels);

/**
 * \brief			free last layer features 
 * @param[in]	features	..features to free
 */
static void model_free_lastLayer( LAST_FEATURES* features, uint32_t channels );

/**
 * \brief			convolution with dma transfers to l1
 * @param[in]	input		..feature map
 * @param[in]	invalidL	..overlapping left (through zero padding, not used to get prediction)
 * @param[in]	invalidR	..overlapping right (through zero padding, not used to get prediction)
 * @param[out]	predict		..prediction 
 */
static void model_forward_dma(TENSOR* input, uint32_t invalidL, uint32_t invalidR, float* predict);

/* ==================================================================================
 * Local function implementations
 * ================================================================================*/
static void model_init_configuration( uint32_t classes )
{
	uint8_t i = 0;

	/* init first layer */
	//	layers[0].features[0] = (void*) calloc(1, sizeof(FIRST_FEATURES));
	layers[0].features[0] = (void*) rt_alloc(ALLOC_LOCATION, sizeof(FIRST_FEATURES));
	if(layers[0].features[0] != NULL)
	{
		#ifdef DEBUG
		printf("memset\n");
		#endif
		memset(layers[0].features[0], 0, sizeof(FIRST_FEATURES));
	}
	layers[0].type = fLayer;
	layers[0].nFilters = nOfFilters[0];
	layers[0].config = configs[0];
	/* init binary layers */
	while(i < BIN_LAYERS)
	{
		i++;
		layers[i].features[0] = (void*) rt_alloc(ALLOC_LOCATION, sizeof(BIN_FEATURES));
		if(layers[i].features[0] != NULL )
		{
			memset(layers[i].features[0], 0, sizeof(BIN_FEATURES));
		}

		layers[i].features[1] = (void*) rt_alloc(ALLOC_LOCATION, sizeof(BIN_FEATURES));
		if(layers[i].features[1] != NULL )
		{
			memset(layers[i].features[1], 0, sizeof(BIN_FEATURES));
		}
		layers[i].type = bLayer;
		layers[i].nFilters = nOfFilters[i];
		layers[i].config = configs[i];
	}

	/* init last layer */
	layers[BIN_LAYERS + 1].features[0] = (void*) rt_alloc(ALLOC_LOCATION, sizeof(LAST_FEATURES));
	if(layers[BIN_LAYERS + 1].features[0] != NULL)
	{
		memset(layers[BIN_LAYERS + 1].features[0], 0, sizeof(LAST_FEATURES));
	}
	//printf("Initialization \n");
	layers[BIN_LAYERS + 1].type = lLayer;
	//layers[BIN_LAYERS + 1].config = configs[BIN_LAYERS + 1];
	layers[BIN_LAYERS + 1].nFilters = INPUT_CHANNELS;
	((LAST_FEATURES*)layers[BIN_LAYERS + 1].features[0])->nFilters1 = 128;
	((LAST_FEATURES*)layers[BIN_LAYERS + 1].features[0])->nFilters2 = classes;
}

/*static void model_l1_byte_alloc(uint8_t size)
  {
//uint8_t* tem = malloc(size);
uint8_t* tem = rt_alloc(ALLOC_LOCATION,size);
}*/


void model_load_firstLayer_32filters( FIRST_FEATURES* features, uint8_t kxky, uint32_t channels )
{
	uint32_t memory;
	uint8_t sizeOf;
	uint32_t filters = 32;
	uint32_t weights_size = filters * kxky * channels;
	uint32_t parameter_size = 9 * filters;

	fpos = 0;
	bpos = 0;

	memory = weights_size;
	memory += parameter_size;
#ifdef FIXPOINT_16BIT
	int16_t* fstart = (int16_t*) rt_alloc(ALLOC_LOCATION, memory*sizeof(int16_t));
	printf("First layer is loaded with : %ld \n",memory*sizeof(int16_t));
	sizeOf = 2;
#elif defined(FIXPOINT_32BIT)
#ifdef DEBUG
	rt_user_alloc_dump(rt_alloc_l1(0));
#endif
	int32_t* fstart = (int32_t*) rt_alloc(ALLOC_LOCATION,memory*sizeof(int32_t));
#ifdef DEBUG
	if(fstart == NULL)
	{
		printf("############################################################\n");
		printf("memory overflowxyz\n");
		printf("############################################################\n");
	}
	printf("First layer is loaded with : %ld Byte \n",memory*sizeof(int32_t));
	rt_user_alloc_dump(rt_alloc_l1(0));
#endif
	sizeOf = 4;	
#else	
	float* fstart = (float*) rt_alloc(ALLOC_LOCATION, memory*sizeof(float)); /*2304 bytes*/
#ifdef DEBUG
	if(fstart == NULL)
	{
		printf("############################################################\n");
		printf("memory overflow\n");
		printf("############################################################\n");
	}
	printf("First layer is loaded with : %ld \n",memory*sizeof(float));
#endif
	sizeOf = 4;
#endif	
	features->bias 			= fstart;
	features->weight 		= fstart + filters;
	features->mean1 		= fstart + weights_size + 1*filters;	
	features->std1 			= fstart + weights_size + 2*filters;
	features->gamma1 		= fstart + weights_size + 3*filters;
	features->beta1 		= fstart + weights_size + 4*filters;
	features->mean2 		= fstart + weights_size + 5*filters;
	features->std2 			= fstart + weights_size + 6*filters;
	features->gamma2 		= fstart + weights_size + 7*filters;
	features->beta2 		= fstart + weights_size + 8*filters;

#ifdef DMA_TRANS
	/* dma transfer */
	dma_transfer_ext2loc((uint32_t*)fData, &fpos, (uint32_t*)fstart, memory, sizeOf);

	//uint32_t l2_address; 
	//l2_address = ((uint32_t) fData) + sizeOf * fpos;
	//rt_dma_memcpy((uint32_t) l2_address, (uint32_t) fstart, memory*sizeOf, RT_DMA_DIR_EXT2LOC, 0, &dma_cop);
	//fpos =  fpos + memory;   
#else
	mcu_transfer_ext2loc((uint32_t*)fData, &fpos, (uint32_t*)fstart, memory, sizeOf);
#endif

}

static void model_free_firstLayer( FIRST_FEATURES* features, uint8_t kxky, uint32_t channels )
{
	uint32_t filters = 32;
	if(features->bias != 0)
	{
#ifdef FIXPOINT_16BIT
	#ifdef DEBUG
		printf("Release Firstlayer with %ld Byte \n",(filters*kxky*channels+9*filters)*sizeof(int16_t));
	#endif
		rt_free(ALLOC_LOCATION,features->bias,(filters*kxky*channels+9*filters)*sizeof(int16_t));
		features->bias = NULL;
#elif defined(FIXPOINT_32BIT)
	#ifdef DEBUG
		printf("Release Firstlayer with %ld Byte \n",(filters*kxky*channels+9*filters)*sizeof(int32_t));
	#endif
		rt_free(ALLOC_LOCATION,features->bias,(filters*kxky*channels+9*filters)*sizeof(int32_t));
	#ifdef DEBUG
		rt_user_alloc_dump(rt_alloc_l1(0));
	#endif
		features->bias = NULL;
#else
	#ifdef DEBUG
		printf("Release Firstlayer with %ld Byte \n",(filters*kxky*channels+9*filters)*sizeof(float));
	#endif
		rt_free(ALLOC_LOCATION,features->bias,(filters*kxky*channels+9*filters)*sizeof(float));
	#ifdef DEBUG
		rt_user_alloc_dump(rt_alloc_l1(0));
	#endif
		features->bias = NULL;
#endif
	}
}


static void model_load_binaryLayer_32filters(BIN_FEATURES* features, uint8_t kxky, uint32_t channels)
{

	//int32_t* sstart;
	uint32_t filters 	= 32;
	uint32_t binChannels 	= channels/32;
	uint32_t weights_size 	= filters * kxky * binChannels; 
	uint32_t parameter_size = 4*filters;
	uint32_t memory		= weights_size + parameter_size;

	uint32_t* ustart        = (uint32_t*) rt_alloc(ALLOC_LOCATION,weights_size*sizeof(uint32_t));
	int32_t* sstart        = (int32_t*) rt_alloc(ALLOC_LOCATION,parameter_size*sizeof(int32_t));
#ifdef DEBUG	
	printf("It is loaded with : %ld Byte \n", weights_size*sizeof(uint32_t));
	printf("It is loaded with : %ld Byte \n", parameter_size*sizeof(uint32_t));
	rt_user_alloc_dump(rt_alloc_l1(0));
#endif
	features->weight 	= ustart;	
	features->signGamma 	= sstart;
	features->threshhold 	= sstart + 1*filters;
	features->relu		= sstart + 2*filters;
	features->compare 	= sstart + 3*filters;

#ifdef DMA_TRANS
	dma_transfer_ext2loc( bData, &bpos,(uint32_t*) ustart, weights_size, 4);
	//uint32_t l2_address; 
	//l2_address = ((uint32_t) bData) + 4 * bpos;
	//rt_dma_memcpy((uint32_t) l2_address, (uint32_t) ustart, weights_size*4, RT_DMA_DIR_EXT2LOC, 0, &dma_cop);
	//bpos =  bpos + weights_size;   
	dma_transfer_ext2loc( bData, &bpos,(uint32_t*) sstart, parameter_size, 4);
	//l2_address = ((uint32_t) bData) + 4 * bpos;
	//rt_dma_memcpy((uint32_t) l2_address, (uint32_t) sstart, parameter_size*4, RT_DMA_DIR_EXT2LOC, 0, &dma_cop);
	//bpos =  bpos + parameter_size;   
#else
	mcu_transfer_ext2loc( bData, &bpos,(uint32_t*) ustart, weights_size,4);
	mcu_transfer_ext2loc( bData, &bpos,(uint32_t*) sstart, parameter_size,4);
#endif
}


static void model_free_binaryLayer( BIN_FEATURES* features, uint8_t kxky, uint32_t input_channels)
{
	uint32_t filters = 32;
	if(features->weight != 0)
	{
		rt_free(ALLOC_LOCATION,features->weight, input_channels*kxky*sizeof(uint32_t));
		rt_free(ALLOC_LOCATION,features->signGamma, 4*filters*sizeof(int32_t));
		features->weight = NULL;
		features->signGamma = NULL;
		features->threshhold = NULL;
		features->relu = NULL;
		features->compare = NULL;
#ifdef DEBUG
		printf("4*filters*sizeof(int32_t) is released with :%ld Byte \n", 4*filters*sizeof(int32_t));
		printf("input_channels*kxky*sizeof(uint32_t) is released with :%ld Byte \n", input_channels*kxky*sizeof(uint32_t));
		rt_user_alloc_dump(rt_alloc_l1(0));
		printf("input_channels %lu\n", input_channels);
#endif
	}
}


static void model_load_lastLayer(LAST_FEATURES* features, uint32_t channels)
{
	uint32_t* 	ustart;
	uint32_t weights1_size;
	uint32_t weights2_size;
	uint32_t parameter_size;
	uint32_t memory2;
	uint32_t binChannels;
	uint8_t sizeOf;

	/* load first weights */
	binChannels = channels/32;
	weights1_size = features->nFilters1 * binChannels; 
	ustart = (uint32_t*) rt_alloc(ALLOC_LOCATION,weights1_size*sizeof(uint32_t));	

#ifdef DMA_TRANS
	dma_transfer_ext2loc(bData, &bpos, ustart, weights1_size, 4);
	//uint32_t l2_address; 
	//l2_address = ((uint32_t) bData) + 4 * bpos;
	//rt_dma_memcpy((uint32_t) l2_address, (uint32_t) ustart, weights1_size*4, RT_DMA_DIR_EXT2LOC, 0, &dma_cop);
	//bpos =  bpos + weights1_size;   
#else
	mcu_transfer_ext2loc(bData, &bpos, ustart, weights1_size, 4);
#endif
	/* load second weights and parameters */
	weights2_size 	= features->nFilters1 * features->nFilters2;
	parameter_size 	= 6 * features->nFilters1 + features->nFilters2;
	memory2 		= weights2_size + parameter_size;

#ifdef FIXPOINT_16BIT
	sizeOf = 2;
	int16_t* ffstart = (int16_t*) rt_alloc(ALLOC_LOCATION,weights2_size*sizeof(int16_t)); 
	int16_t* fstart= (int16_t*) rt_alloc(ALLOC_LOCATION,parameter_size*sizeof(int16_t));
#ifdef DEBUG
	printf("Last layer is loaded with : %ld \n", memory2*sizeof(int16_t) + weights1_size*sizeof(uint32_t));
#endif
#elif defined(FIXPOINT_32BIT)
	sizeOf = 4;
	int32_t* ffstart = (int32_t*) rt_alloc(ALLOC_LOCATION,weights2_size*sizeof(int32_t)); 
	int32_t* fstart= (int32_t*) rt_alloc(ALLOC_LOCATION,parameter_size*sizeof(int32_t));
#ifdef DEBUG
	printf("Last layer is loaded with : %ld Byte \n", weights1_size*sizeof(uint32_t) + memory2*sizeof(int32_t));
	rt_user_alloc_dump(rt_alloc_l1(0));
#endif
#else
	sizeOf = 4;
	float* ffstart = (float*) rt_alloc(ALLOC_LOCATION,weights2_size*sizeof(float)); 
	float* fstart = (float*) rt_alloc(ALLOC_LOCATION,parameter_size*sizeof(float)); 
#ifdef DEBUG
	printf("Last layer is loaded with : %ld Byte \n", weights1_size*sizeof(uint32_t) + memory2*sizeof(float));
	rt_user_alloc_dump(rt_alloc_l1(0));
#endif
#endif

	features->weight1 	= ustart;

	features->bias1 	= fstart;
	features->alpha1	= fstart  + 1*features->nFilters1;
	features->mean 		= fstart  + 2*features->nFilters1; 
	features->std 		= fstart  + 3*features->nFilters1;
	features->gamma 	= fstart  + 4*features->nFilters1;
	features->beta 		= fstart  + 5*features->nFilters1;
	features->bias2		= fstart  + 6*features->nFilters1;
	features->weight2 	= ffstart;

#ifdef DMA_TRANS
	#ifdef DMA_PERFxx
	perf2 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
	rt_perf_init(perf2);
	rt_perf_conf(perf2, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
	rt_perf_reset(perf2);
	rt_perf_start(perf2);
	#endif
	dma_transfer_ext2loc((uint32_t*)fData, &fpos, (uint32_t*)fstart, parameter_size, sizeOf);
	dma_transfer_ext2loc((uint32_t*)fData, &fpos, (uint32_t*)ffstart,weights2_size, sizeOf);
	#ifdef DMA_PERFxx
	rt_perf_stop(perf2);
	rt_perf_save(perf2);
	int instr1 = rt_perf_get(perf2, RT_PERF_INSTR);
	int cycles1 = rt_perf_get(perf2, RT_PERF_CYCLES);
	printf("last model load : instr : %d\n", instr1);
	printf("last model load : cycles : %d\n", cycles1);
	#endif 
	//l2_address = ((uint32_t) fData) + sizeOf * fpos;
	//rt_dma_memcpy((uint32_t) l2_address, (uint32_t) fstart, memory2*sizeOf, RT_DMA_DIR_EXT2LOC, 0, &dma_cop);
	//fpos =  fpos + memory2;  
#else
	/* tranfer to l1 (copy) */
	mcu_transfer_ext2loc((uint32_t*)fData, &fpos, (uint32_t*)fstart, parameter_size, sizeOf);
	mcu_transfer_ext2loc((uint32_t*)fData, &fpos, (uint32_t*)ffstart, weights2_size, sizeOf);
#endif
}

static void model_free_lastLayer( LAST_FEATURES* features, uint32_t channels )
{
	uint32_t binChannels;
	binChannels = channels/32;
	if(features->weight1 != 0)
	{
		rt_free(ALLOC_LOCATION, features->weight1, features->nFilters1*binChannels*sizeof(uint32_t));
#ifdef DEBUG
		printf("release Last weight1 : %ld Byte\n",features->nFilters1*binChannels*sizeof(uint32_t));
#endif
		features->weight1 = NULL;
	}
	if(features->bias1 != 0)
	{
#ifdef FIXPOINT_16BIT
		rt_free(ALLOC_LOCATION, features->bias1,(6*features->nFilters1 + features->nFilters2)*sizeof(int16_t));
		rt_free(ALLOC_LOCATION, features->weight2,(features->nFilters1*features->nFilters2)*sizeof(int16_t));
	#ifdef DEBUG
		printf("release Last bias1 : %ld Byte \n",(features->nFilters1*features->nFilters2 + 6*features->nFilters1 + features->nFilters2)*sizeof(int16_t));
	#endif
#elif defined(FIXPOINT_32BIT)
		rt_free(ALLOC_LOCATION, features->bias1,(6*features->nFilters1 + features->nFilters2)*sizeof(int32_t));
		rt_free(ALLOC_LOCATION, features->weight2,(features->nFilters1*features->nFilters2)*sizeof(int32_t));

	#ifdef DEBUG
		printf("release Last bias1 : %ld Byte \n",(features->nFilters1*features->nFilters2 + 6*features->nFilters1 + features->nFilters2)*sizeof(int32_t));
		rt_user_alloc_dump(rt_alloc_l1(0));
	#endif
#else
		rt_free(ALLOC_LOCATION, features->bias1,(6*features->nFilters1 + features->nFilters2)*sizeof(float));
		rt_free(ALLOC_LOCATION, features->weight2,(features->nFilters1*features->nFilters2)*sizeof(float));
#ifdef DEBUG
		printf("release Last bias1 : %ld Byte \n",(features->nFilters1*features->nFilters2 + 6*features->nFilters1 + features->nFilters2)*sizeof(float));
#endif
#endif
		features->bias1 = NULL;
	}
}


static void model_forward_dma
(
 TENSOR* input,
 uint32_t invalidL,
 uint32_t invalidR,
 float* predict
 )
{
	uint8_t i,k,n;
	uint8_t next; /*< load next layer */
	uint8_t step; /*< filter step */
	uint32_t filters;
	uint32_t channels = INPUT_CHANNELS;
	uint8_t kxky;
	TENSOR* output=NULL;

	n 		= 1; 	/*< first features should already be loaded */
	k 		= 0;	/*< first features are not applied but loaded */
	step 	= 32;   /*< 32 filters convolution */
	/* iterate over all layers */
	for(i = 0; i < NOF_LAYERS; i++)
	{
#ifdef LED
		led_toggle();
#endif
#ifdef DEBUG
		printf("---------------------%d Layer-----------------\n",i);
#endif

		/* allocate feature map tensor */
		if(i < NOF_LAYERS-1)
		{
#ifdef DEBUG
			printf("Allocate output to L1 : \n");
			printf("input->X  before output = %ld \n", input->X);
#endif
			output =  tensor_alloc_featureMap(input, layers[i].config, layers[i].nFilters);
		}
		else
		{
			output = NULL;
		}

		/* apply layer configuration */
		(*setConfiguration[layers[i].type])(layers[i].config);

		/* load next and apply filters */
		filters = 0;
			//printf("model.c 1 input->Y %ld  \n", input->Y);
		while(filters < layers[i].nFilters)
		{
			/* load new layer */
			next 		= ((filters + step) >= layers[i].nFilters) ? 1 : 0;	/*< 1 when new layer must be loaded*/ 
			n 	 		= (next == 1) ? 0 : n;								/*< start form 0 when new layer*/
			channels 	= (next == 1) ? layers[i].nFilters : channels;		/*< update channels for next layer */
			next 		= (i + next) % NOF_LAYERS;							/*< next index of layers arra */
			if(layers[next].type == lLayer)
			{
				kxky = layers[i].config->kX * layers[i].config->kY;
			}else{
				kxky = layers[next].config->kX * layers[next].config->kY;
			}


#ifdef DMA_TRANS
			/* dma ready */
			//rt_dma_wait(&dma_cop);

#endif
#ifdef DEBUG
			printf("----load layer to L1---- \n");
			/* load next to l1 */
			rt_user_alloc_dump(rt_alloc_l1(0));
#endif
			switch(layers[next].type)
			{
				case fLayer:
#ifdef DEBUG
					printf("fLayer load next to l1 for next layer[%d].features[%d] \n", next,n);
					printf("how much is loaded for next: %ld in layers[%d].features[%d] \n", channels, next, n);
#endif
#ifdef DMA_PERF
	 
	perf2 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
	rt_perf_init(perf2);
	rt_perf_conf(perf2, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
	rt_perf_reset(perf2);
	rt_perf_start(perf2);
#endif
					model_load_firstLayer_32filters((FIRST_FEATURES*)layers[next].features[n], kxky, INPUT_CHANNELS); 
#ifdef DMA_PERF
	rt_perf_stop(perf2);
	rt_perf_save(perf2);
	int instr1 = rt_perf_get(perf2, RT_PERF_INSTR);
	int cycles1 = rt_perf_get(perf2, RT_PERF_CYCLES);
	printf("first model load : instr : %d\n", instr1);
	printf("first model load : cycles : %d\n", cycles1);
#endif 
					break;

				case bLayer:
#ifdef DEBUG
					printf("bLayer load next to l1 for next layer[%d].features[%d] \n",next,n);
					printf("how much is loaded for next: %ld in layers[%d].features[%d] \n", channels, next, n);
#endif

#ifdef DMA_PERF
	 
	perf2 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
	rt_perf_init(perf2);
	rt_perf_conf(perf2, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
	rt_perf_reset(perf2);
	rt_perf_start(perf2);
#endif
					model_load_binaryLayer_32filters((BIN_FEATURES*)layers[next].features[n], kxky, channels); 

#ifdef DMA_PERF
	rt_perf_stop(perf2);
	rt_perf_save(perf2);
	int instr2 = rt_perf_get(perf2, RT_PERF_INSTR);
	int cycles2 = rt_perf_get(perf2, RT_PERF_CYCLES);
	printf("binary model load : instr : %d\n", instr2);
	printf("binary model load : cycles : %d\n", cycles2);
#endif 

					//printf("bLayer load next to l1 for next layer [%d] finished\n", next);
					break;

				case lLayer:
#ifdef DEBUG
					printf("lLayer load next to l1 for next layer.[%d].features[%d]\n", next,n);
					printf("Channels for Last layer : %ld\n", channels);
#endif
#ifdef DMA_PERF
	 
	perf2 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
	rt_perf_init(perf2);
	rt_perf_conf(perf2, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
	rt_perf_reset(perf2);
	rt_perf_start(perf2);
#endif
					model_load_lastLayer((LAST_FEATURES*)layers[next].features[n], channels); 
#ifdef DMA_PERF
	rt_perf_stop(perf2);
	rt_perf_save(perf2);
	int instr3 = rt_perf_get(perf2, RT_PERF_INSTR);
	int cycles3 = rt_perf_get(perf2, RT_PERF_CYCLES);
	printf("last model load : instr : %d\n", instr3);
	printf("last model load : cycles : %d\n", cycles3);
#endif 
					//printf("Load Channel: %ld \n",channels);
					break;
			}
#ifdef DEBUG
			printf("----finished---- \n");
#endif
			/* convolution & free l1 from features */
			switch(layers[i].type)
			{
				case fLayer:
#ifdef DEBUG
					printf("first layer 32-filters\n");
					printf("Preparing first layer 32-filters 1 \n");
					printf("input : %p, feature : %p, output : %p \n",input,layers[i].features[k],output);
					printf("Preparing first layer with output channels : %ld\n", layers[i].nFilters);
#endif
					firstLayer_convolution_32filters(input, (FIRST_FEATURES*)layers[i].features[k], output);
					model_free_firstLayer((FIRST_FEATURES*)layers[i].features[k], kxky, INPUT_CHANNELS);
					break;

				case bLayer:
#ifdef DEBUG
					printf("binary layer %d 32-filters\n", i);
					printf("Preparing binary layer %d with input channels : %ld, output channels : %ld\n", i, layers[i-1].nFilters, layers[i].nFilters);
#endif
					binaryLayer_convolution_32filters(input, (BIN_FEATURES*)layers[i].features[k], output);
					model_free_binaryLayer((BIN_FEATURES*)layers[i].features[k], kxky, layers[i-1].nFilters);
					break;

				case lLayer:
#ifdef DEBUG
					printf("Preparing Last layer %d with output channels : %ld\n", i, ((LAST_FEATURES*)layers[BIN_LAYERS + 1].features[0])->nFilters1);
#endif
					lastLayer_convolution(input, (LAST_FEATURES*)layers[i].features[k], invalidL, invalidR, predict);
#ifdef DEBUG
					printf("Last layer[%d].features[%d] convolution  finished \n",i,k);
#endif
					model_free_lastLayer((LAST_FEATURES*)layers[i].features[k], ((LAST_FEATURES*)layers[BIN_LAYERS + 1].features[0])->nFilters1);
#ifdef DEBUG
					printf("Free Channels: %ld \n", ((LAST_FEATURES*)layers[BIN_LAYERS + 1].features[0])->nFilters1);
#endif
					break;		
			}
			filters += step;
			k = n;
			n = (n+1) % 2;
		}
		/* free input map from l1 */
		tensor_free(input); 
		rt_free(ALLOC_LOCATION, input, sizeof(TENSOR));

		input = output; //original
#ifdef DEBUG
#endif
	}
#ifdef LED
	led_toggle();
#endif
}


/* ==================================================================================
 * Global function implementations
 * ================================================================================*/

extern void model_init(uint32_t classes)
{
	model_init_configuration(classes);
#ifdef DEBUG
	printf("model_init_configuration \n");
#endif
	/* load first layer to l1*/
	uint8_t kxky = layers[0].config->kX * layers[0].config->kY;
#ifdef DMA_PERF
	 
	perf2 = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
	rt_perf_init(perf2);
	rt_perf_conf(perf2, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
	rt_perf_reset(perf2);
	rt_perf_start(perf2);
#endif
	model_load_firstLayer_32filters((FIRST_FEATURES*)layers[0].features[0], kxky, INPUT_CHANNELS);	
	//printf("(%d, %d) model_init \n", rt_cluster_id(), rt_core_id());
#ifdef DMA_PERF
	rt_perf_stop(perf2);
	rt_perf_save(perf2);
	int instr3 = rt_perf_get(perf2, RT_PERF_INSTR);
	int cycles3 = rt_perf_get(perf2, RT_PERF_CYCLES);
	printf("First model load : instr : %d\n", instr3);
	printf("First model load : cycles : %d\n", cycles3);
#endif 
}

extern void model_free(uint32_t classes)
{
    model_free_firstLayer((FIRST_FEATURES*)layers[0].features[0], 9, INPUT_CHANNELS);
	rt_free(ALLOC_LOCATION, layers[0].features[0], sizeof(FIRST_FEATURES));
	//rt_free(ALLOC_LOCATION, layers[0], sizeof(LAYER));
	/* load first layer to l1*/
	int i = 0;
	while(i < BIN_LAYERS)
	{
		i++;
		rt_free(ALLOC_LOCATION, layers[i].features[0], sizeof(BIN_FEATURES));
		rt_free(ALLOC_LOCATION, layers[i].features[1], sizeof(BIN_FEATURES));
	}
	rt_free(ALLOC_LOCATION, layers[BIN_LAYERS + 1].features[0], sizeof(LAST_FEATURES));
}

	extern void model_forward
(
 TENSOR* input,
 uint32_t invalidL,
 uint32_t invalidR,
 float* predict
 )
{
	//printf("(%d, %d) model_forward_dma \n", rt_cluster_id(), rt_core_id());
	model_forward_dma(input,invalidL,invalidR, predict);
}
