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
#include "model.h"
#ifdef FIXPOINT_16BIT
#include "model_fp16.dat"
#elif defined(FIXPOINT_32BIT)
#include "model_fp32.dat"
#else
#include "model.dat"
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

/* dma */
static int32_t 		dma_identifier;		

/* layers */
static LAYER 				layers[NOF_LAYERS];

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
static void model_load_firstLayer_32filters( FIRST_FEATURES* features, uint8_t kxky, uint32_t channels );
/**
 * \brief			free first layer features 
 * @param[in]	features	..features to free
 */
static void model_free_firstLayer( FIRST_FEATURES* features );

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
static void model_free_binaryLayer( BIN_FEATURES* features );

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
static void model_free_lastLayer( LAST_FEATURES* features );

/**
 * \brief			convolution with dma transfers to l1
 * @param[in]	input		..feature map
 * @param[in]	invalidL	..overlapping left (through zero padding, not used to get prediction)
 * @param[in]	invalidR	..overlapping right (through zero padding, not used to get prediction)
 * @param[out]	predict		..prediction 
 */
static void model_forward_dma(TENSOR* input, uint32_t invalidL, uint32_t invalidR, double* predict);

/* ==================================================================================
 * Local function implementations
 * ================================================================================*/
static void model_init_configuration( uint32_t classes )
{
	uint8_t i = 0;

	/* init first layer */
	layers[0].features[0] = (void*) calloc(1, sizeof(FIRST_FEATURES));
	layers[0].type = fLayer;
	layers[0].nFilters = nOfFilters[0];
	layers[0].config = configs[0];

	/* init binary layers */
	while(i < BIN_LAYERS)
	{
		i++;
		layers[i].features[0] = (void*) calloc(1, sizeof(BIN_FEATURES)); 
		model_l1_byte_alloc(4); /*< only to slove fulmine error: 8byte dma base */
		layers[i].features[1] = (void*) calloc(1, sizeof(BIN_FEATURES));
		model_l1_byte_alloc(4); /*< only to slove fulmine error: 8byte dma base */
		layers[i].type = bLayer;
		layers[i].nFilters = nOfFilters[i];
		layers[i].config = configs[i];
	}

	/* init last layer */
	layers[BIN_LAYERS + 1].features[0] = (void*) calloc(1, sizeof(LAST_FEATURES));
	model_l1_byte_alloc(4); /*< only to slove fulmine error: 8byte dma base */
	layers[BIN_LAYERS + 1].type = lLayer;

	layers[BIN_LAYERS + 1].nFilters = INPUT_CHANNELS;
	((LAST_FEATURES*)layers[BIN_LAYERS + 1].features[0])->nFilters1 = 128;
	((LAST_FEATURES*)layers[BIN_LAYERS + 1].features[0])->nFilters2 = classes;

}

static void model_l1_byte_alloc(uint8_t size)
{
	uint8_t* tem = malloc(size);
}


static void model_load_firstLayer_32filters( FIRST_FEATURES* features, uint8_t kxky, uint32_t channels )
{

	uint32_t memory;
	uint8_t sizeOf;
	// float* fstart;

	uint32_t filters = 32;

	uint32_t weights_size = filters * kxky * channels;
	uint32_t parameter_size = 9 * filters;

	fpos = 0;
	bpos = 0;


	memory = weights_size;
	memory += parameter_size;

#ifdef FIXPOINT_16BIT
	int16_t* fstart = (int16_t*) malloc(memory*sizeof(int16_t));
	sizeOf = 2;
#elif defined(FIXPOINT_32BIT)
	int32_t* fstart = (int32_t*) malloc(memory*sizeof(int32_t));
	sizeOf = 4;	
#else	
	float* fstart = (float*) malloc(memory*sizeof(float)); /*2304 bytes*/
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
	dma_identifier = dma_transfer_ext2loc((uint32_t*)fData, &fpos, (uint32_t*)fstart, memory, sizeOf);
#else
	mcu_transfer_ext2loc((uint32_t*)fData, &fpos, (uint32_t*)fstart, memory, sizeOf);
#endif
}

static void model_free_firstLayer( FIRST_FEATURES* features )
{
	if(features->bias != 0)
	{
		free(features->bias);
		features->bias = 0;
	}
}


static void model_load_binaryLayer_32filters(BIN_FEATURES* features, uint8_t kxky, uint32_t channels)
{

	int32_t* sstart;
	uint32_t filters 		= 32;
	uint32_t binChannels 	= channels/32;

	uint32_t weights_size 	= filters * kxky * binChannels; 
	uint32_t parameter_size = 4*filters;
	uint32_t memory 		= weights_size + parameter_size;


	uint32_t* ustart	= (uint32_t*) malloc(memory*sizeof(uint32_t)); 


	features->weight 		= ustart;	
	
	sstart 					= (int32_t*) (ustart + weights_size);
	features->signGamma 	= sstart;
	features->threshhold 	= sstart + 1*filters;
	features->relu 			= sstart + 2*filters;
	features->compare 		= sstart + 3*filters;

	#ifdef DMA_TRANS
	dma_identifier = dma_transfer_ext2loc(bData, &bpos, ustart, memory, 4);
	#else
	mcu_transfer_ext2loc(bData, &bpos, ustart, memory,4);
	#endif
}


static void model_free_binaryLayer( BIN_FEATURES* features )
{
	if(features->weight != 0)
	{
		free(features->weight);
		features->weight = 0;
		features->signGamma = 0;
		features->threshhold = 0;
		features->relu = 0;
		features->compare = 0;
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
	ustart = (uint32_t*) malloc(weights1_size*sizeof(uint32_t));

	#ifdef DMA_TRANS
	dma_identifier = dma_transfer_ext2loc(bData, &bpos, ustart, weights1_size, 4);
	#endif

	/* load second weights and parameters */
	weights2_size 	= features->nFilters1 * features->nFilters2;
	parameter_size 	= 6 * features->nFilters1 + features->nFilters2;
	memory2 		= weights2_size + parameter_size;

#ifdef FIXPOINT_16BIT
	sizeOf = 2;
	int16_t* fstart = (int16_t*) malloc(memory2*sizeof(int16_t)); 
#elif defined(FIXPOINT_32BIT)
	sizeOf = 4;
	int32_t* fstart = (int32_t*) malloc(memory2*sizeof(int32_t)); 
#else
	sizeOf = 4;
	float* fstart = (float*) malloc(memory2*sizeof(float)); 
#endif
	

	features->weight1 	= ustart;
	features->bias1 	= fstart;

	features->alpha1	= fstart  + 1*features->nFilters1;
	features->mean 		= fstart  + 2*features->nFilters1; 
	features->std 		= fstart  + 3*features->nFilters1;
	features->gamma 	= fstart  + 4*features->nFilters1;
	features->beta 		= fstart  + 5*features->nFilters1;
	features->bias2		= fstart  + 6*features->nFilters1;

	features->weight2 	= fstart  + parameter_size;

	#ifdef DMA_TRANS
	/* dma ready */
	dma_wait(dma_identifier);
	dma_identifier = dma_transfer_ext2loc((uint32_t*)fData, &fpos, (uint32_t*)fstart, memory2, sizeOf);
	
	#else
	/* tranfer to l1 (copy) */
	mcu_transfer_ext2loc(bData, &bpos, ustart, weights1_size,sizeOf);
	mcu_transfer_ext2loc((uint32_t*)fData, &fpos, (uint32_t*)fstart, memory2,sizeOf);
	#endif
}

static void model_free_lastLayer( LAST_FEATURES* features )
{
	if(features->weight1 != 0)
	{
		free(features->weight1);
		features->weight1 = 0;
	}
	if(features->bias1 != 0)
	{
		free(features->bias1);
		features->bias1 = 0;
	}
}



static void model_forward_dma
(
	TENSOR* input,
	uint32_t invalidL,
	uint32_t invalidR,
	double* predict
)
{
	uint8_t i,k,n;
	uint8_t next; /*< load next layer */
	uint8_t step; /*< filter step */
	uint32_t filters;
	uint32_t channels = INPUT_CHANNELS;
	uint8_t kxky;

	TENSOR* output;

	n 		= 1; 	/*< first features should already be loaded */
	k 		= 0;	/*< first features are not applied but loaded */
	step 	= 32;   /*< 32 filters convolution */

	/* iterate over all layers */
	for(i = 0; i < NOF_LAYERS; i++)
	{
		#ifdef LED
		led_toggle();
		#endif

		/* allocate feature map tensor */
		if(i < NOF_LAYERS-1)
		{
			output =  tensor_alloc_featureMap(input, layers[i].config, layers[i].nFilters);
		}
		else
		{
			output = 0;
		}

		/* apply layer configuration */
		(*setConfiguration[layers[i].type])(layers[i].config);
		
		/* load next and apply filters */
		filters = 0;
		while(filters < layers[i].nFilters)
		{

			/* load new layer */
			next 		= ((filters + step) >= layers[i].nFilters) ? 1 : 0;	/*< 1 when new layer must be loaded*/ 
			n 	 		= (next == 1) ? 0 : n;								/*< start form 0 when new layer*/
			channels 	= (next == 1) ? layers[i].nFilters : channels;		/*< update channels for next layer */
			next 		= (i + next) % NOF_LAYERS;							/*< next index of layers arra */
			
			kxky = layers[next].config->kX * layers[next].config->kY;

			#ifdef DMA_TRANS
			/* dma ready */
			dma_wait(dma_identifier);
			#endif

			/* load next to l1 */
			switch(layers[next].type)
			{
				case fLayer:
					model_load_firstLayer_32filters((FIRST_FEATURES*)layers[next].features[n], kxky, INPUT_CHANNELS); 
					break;

				case bLayer:
					model_load_binaryLayer_32filters((BIN_FEATURES*)layers[next].features[n], kxky, channels); 
					break;

				case lLayer:
					#ifdef DEBUG
					printf("last layer load: next = %d, n = %d \n",next, n);
					#endif
					model_load_lastLayer((LAST_FEATURES*)layers[next].features[n], channels); 
					break;
			}

			/* convolution & free l1 from features */
			switch(layers[i].type)
			{
				case fLayer:
					#ifdef DEBUG
					printf("first layer 32-filters\n");
					#endif
					/* first layer convolution */
					firstLayer_convolution_32filters(input, (FIRST_FEATURES*)layers[i].features[k], output);
					model_free_firstLayer((FIRST_FEATURES*)layers[i].features[k]); 
					break;

				case bLayer:
					#ifdef DEBUG
					printf("binary layer %d 32-filters\n", i);
					#endif
					
					/* binary layer convolution */
					binaryLayer_convolution_32filters(input, (BIN_FEATURES*)layers[i].features[k], output);
					model_free_binaryLayer((BIN_FEATURES*)layers[i].features[k]);

					break;

				case lLayer:
					#ifdef DEBUG
					printf("last layer i = %d, k = %d \n",i, k);
					#endif
					/* last layer convolution */
					lastLayer_convolution(input, (LAST_FEATURES*)layers[i].features[k], invalidL, invalidR, predict);
					model_free_lastLayer((LAST_FEATURES*)layers[i].features[k]); 
					break;		
			}
			filters += step;
			k = n;
			n = (n+1) % 2;
		}

		/* free input map from l1 */	
		tensor_free(input); 
		free(input);
		input = output;
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
	
	/* load first layer to l1*/
	uint8_t kxky = layers[0].config->kX * layers[0].config->kY; 
	model_load_firstLayer_32filters((FIRST_FEATURES*)layers[0].features[0], kxky, INPUT_CHANNELS);	
}


extern void model_forward
(
	TENSOR* input,
	uint32_t invalidL,
	uint32_t invalidR,
	double* predict
)
{
	model_forward_dma(input,invalidL,invalidR, predict);
}
