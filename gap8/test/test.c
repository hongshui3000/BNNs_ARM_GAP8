/**
 * \file test.cpp
 * \brief				main fuction for testing purpose 
 * \date 18.01.2017
 * 
 * \details 
 */

#define L1_Base 0x10000008
#define L2_Base	0x1C000000

/* execution code defines */
// #define ALL
// #define SCRETCH
#define LAYER_CHECK
// #define IO 
// #define PERF_BOARD



#ifdef LAYER_CHECK
#include <stdio.h>
#include "../layers/Layer.h"
#include "../utility/tensor.h"
#include "../utility/misc.h"
#include <omp.h>
#include "../config.h"
#include "../model/model.h"
 #include "../model/model.dat"
/****************/
/* init model.c */
/****************/

/* parallel processing */
// #define MULTICORES

/* dma transfer l2 to l1 */
//define FIRST
//define FIXPOINT
 #define BINARY
// #define LAST

#define bin 				4				/* set which layer to check */



/****************/
/* init model.c */
/****************/

int main()
{
	int i;
	double predict[28];
	printf("start \n");

  // fpos = 0;
  // bpos = 0;
	int image_x[4] = {64,32,32,16};

	TENSOR* input = 0;
	TENSOR* output = 0;
  
	/********************
	* FIRST LAYER
	********************/
#ifdef FIRST 
	printf("first layer\n");
	layers[0].features[0] = (void*) calloc(1, sizeof(FIRST_FEATURES));
	layers[0].type = fLayer;
	layers[0].nFilters = nOfFilters[0];
	layers[0].config = configs[0];
#ifdef FIXPOINT
	input = tensor_allocate(TENSOR_FIXPOINT, 64, 20, 1);
	for(i= 0; i< 20; i++)
	{
		input->memFixpoint[i] = i<<FP_SCALE_BITs;
	}	
#else
	input = tensor_allocate(TENSOR_FLOAT, 64, 20, 1);
	for(i= 0; i< 20; i++)
	{
		input->memFloat[i] = i;
	}	
#endif
	output =  tensor_alloc_featureMap(input, layers[0].config, layers[0].nFilters);
	model_load_firstLayer_32filters((FIRST_FEATURES*)layers[0].features[0], 9, 1); 

	#ifdef DMA_TRANS
	/* dma ready */
	dma_wait(dma_identifier);
	#endif
	printf("conv\n");
	firstLayer_setConfiguration(layers[0].config);
	firstLayer_convolution_32filters(input, (FIRST_FEATURES*)layers[0].features[0], output);
	model_free_firstLayer((FIRST_FEATURES*)layers[0].features[0]); 	

	/********************
	* BIN LAYERs
	********************/
#elif defined(BINARY)
	printf("binary layer %u \n", bin);
	layers[bin].features[0] = (void*) calloc(1, sizeof(BIN_FEATURES));
	layers[bin].type = fLayer;
	layers[bin].nFilters = nOfFilters[bin];
	layers[bin].config = configs[bin];
	input = tensor_allocate(TENSOR_BINARY, image_x[bin-1], 20, nOfFilters[bin-1]);
	for(i= 0; i< 20; i++)
	{
		input->memBin[i] = i;
	}	
	output =  tensor_alloc_featureMap(input, layers[bin].config, layers[bin].nFilters);
	model_load_binaryLayer_32filters((BIN_FEATURES*)layers[bin].features[0], 9, nOfFilters[bin-1]); 

	#ifdef DMA_TRANS
	/* dma ready */
	dma_wait(dma_identifier);
	#endif
	printf("conv\n");
	binaryLayer_setConfiguration(layers[bin].config);
	binaryLayer_convolution_32filters(input, (BIN_FEATURES*)layers[bin].features[0], output);
	model_free_binaryLayer((BIN_FEATURES*)layers[bin].features[0]); 


#elif defined(LAST)
	printf("last layer \n");
	layers[BIN_LAYERS + 1].features[0] = (void*) calloc(1, sizeof(LAST_FEATURES));
	layers[BIN_LAYERS + 1].type = lLayer;
	layers[BIN_LAYERS + 1].nFilters = INPUT_CHANNELS;
	((LAST_FEATURES*)layers[BIN_LAYERS + 1].features[0])->nFilters1 = 128;
	((LAST_FEATURES*)layers[BIN_LAYERS + 1].features[0])->nFilters2 = 28;

	input = tensor_allocate(TENSOR_BINARY, 16, 20, 128);
	for(i= 0; i< 20; i++)
	{
		input->memBin[i] = i;
	}	

	model_load_lastLayer((LAST_FEATURES*)layers[BIN_LAYERS + 1].features[0], 128); 


	#ifdef DMA_TRANS
	/* dma ready */
	dma_wait(dma_identifier);
	#endif
	printf("conv\n");
	lastLayer_convolution(input, (LAST_FEATURES*)layers[BIN_LAYERS + 1].features[0], 2, 3, predict);
	model_free_lastLayer((LAST_FEATURES*)layers[BIN_LAYERS + 1].features[0]); 
#endif


	for(i= 0; i< 28; i++)
	{
		#ifdef LAST
		printf(" %u", predict[i]);
		#else
		printf(" %u", output->memBin[i]);
		#endif
	}

	printf("\n done \n");
	return 0;
}

#endif 



#ifdef IO

#include "utils.h"
#include <stdio.h>
#include "spi.h"
#include "timer.h"

int main()
{

  const int NUM_WAIT = 2000000;
  volatile int i = 0;

  if(get_core_id() == 0) {

    //set up leds
    set_pin_function(17, 1);
    set_gpio_pin_direction(16, 1);
    set_gpio_pin_value(16, 0);

    while(1) {
        set_gpio_pin_value(16, 1);
        for (i=0; i<NUM_WAIT; i++);
        set_gpio_pin_value(16, 0);
        for (i=0; i<NUM_WAIT; i++);
    }
  }

  return 0;
}

#endif




#ifdef PERF_BOARD
#include "omp.h"
#include <hwPerf.h>


static void dumpCoreCounters(int doBarrier, int event) {
  int i;
  // First generate some statistics
  // The number of HW counters varies depending on the platform.
  // There is only one with real chips and a lot on other platforms like FPGA, RTL platform or virtual platform.
  // All events can be read in one shot on most platform but we need to execute several times the same code on the real chip.
  // Thus this API is provided in order to iterate enough time to get all events depending on the platform.
  hw_perf_t perf;
  hw_perf_init(&perf);
  perf.step = event;
  // hw_perf_step will tell how many times we must iterate and will switch to a new event in case
  // we must continue
  for(i= 0; i <= event ; i++)
  {
  	hw_perf_step(&perf);
  }
    
    // In case we are running several cores, do a barrier to resynchronize everyone together
    // if (doBarrier) {
    //   #pragma omp barrier
    // }
    // The active events are counted between hw_perf_start and hw_perf_stop and are cumulated
    hw_perf_start(&perf);
      
    // Do dummy accesses to L1, this will generate contention when running multiple cores
    for (i=0; i<1000 + 100 * plp_coreId(); i++) *(volatile int *)0x10000000;
    
    // Do dummy accesses to L2, this will generate remote load stall cycles
    for (i=0; i<1000 + 100 * plp_coreId(); i++) *(volatile int *)0x1c000000;
      
    hw_perf_stop(&perf);
    // This calls transfers the number of events from the HW counters to our structure
    hw_perf_commit(&perf);
  
  // Now display the results for each core

    for(i=0; i<omp_get_num_threads(); i++)
    {	
	    if (doBarrier) {
	      #pragma omp barrier
	    }

	    if(i == plp_coreId())
	    {
		    printf("[CORE %d] ", plp_coreId());
		  
		    printf("%20s: %d\n", hw_perf_get_name(&perf, event), hw_perf_get_value(&perf, event));	    	
	    }
    }


	// #pragma omp parallel sections
	// {
 //    #pragma omp section
 //    { 
	// 	    printf("[CORE %d] HW performance counters results\n", plp_coreId());
		  
	// 	    // printf("%20s: %d\n", hw_perf_get_name(&perf, event), hw_perf_get_value(&perf, event));
 //    }

 //    #pragma omp section
 //    { 
	// 	    printf("[CORE %d] HW performance counters results\n", plp_coreId());
		  
	// 	    // printf("%20s: %d\n", hw_perf_get_name(&perf, event), hw_perf_get_value(&perf, event));
 //    }
 //    #pragma omp section
 //    { 
	// 	    printf("[CORE %d] HW performance counters results\n", plp_coreId());
		  
	// 	    // printf("%20s: %d\n", hw_perf_get_name(&perf, event), hw_perf_get_value(&perf, event));
 //    }

 //    #pragma omp section
 //    { 
	// 	    printf("[CORE %d] HW performance counters results\n", plp_coreId());
		  
	// 	    // printf("%20s: %d\n", hw_perf_get_name(&perf, event), hw_perf_get_value(&perf, event));
 //    }
 //  }
}

int main()
{
  // // Each core has its own private performance counters so it is possible to dump them from anycore.
  // // However wen using OpenMP, it is mainly useful to get them from the master core
  // // First dump only from master core as it is the only one executing
  // printf("\n");
  // printf("HW counters values from master core\n");
  // printf("***********************************\n");
  // printf("\n");
  // dumpCoreCounters(0);
  // And then from all cores from within an OMP parallel region as they are all executing
  printf("\n");
  printf("\n");
  printf("HW counters values from all cores\n");
  printf("*********************************\n");
  printf("\n");

  #pragma omp parallel
    {
    	int i;
   		for (i=0; i<17; i++) 
   		{
      	dumpCoreCounters(1,i);
    	}
    }
  return 0;
}
#endif




#ifdef ALL 

static  void 			v_exampleLocal(void);
static  void 			v_magicNumber(void);
static int8_t 			u8_exampleLocal(uint8_t* u8ptr_status);
// static int8_t* floatex(int8_t status, uint8_t mul);


typedef struct {
        uint32_t (*u32ptr_init)(void);
        uint32_t (*u32ptr_read_reg)(uint8_t* u8ptr_status);
        int value;
} st_ITMES;

typedef struct s1
{
	int a;
	int b;
};

typedef struct s2
{
	unsigned int a;
	unsigned int b;
	unsigned int c;
};

typedef union u
{
    float f;
    uint32_t u32;
}u_dam;

void myPrintff(float fVal)
{
    int dVal, i;
    unsigned int long dec;
    char c[5];

    // fVal += 0.0000005;   // added after a comment from Matt McNabb, see below.

    dVal = fVal;
    dec = (int)(fVal * 100000) % 100000;

    if(fVal < 0)
    {
    	dec *= (-1);
    	if(dVal == 0)
    	{
    		printf("-");
    	}
    }

    for(i = 5-1; i >= 0; i--)
    {
        c[i] = dec % 10 + '0';
        dec /= 10;
    }

    printf("%d.", dVal);
    printf("%s", c);
    printf("\n");
}

/* ==================================================================================
 * Local/Global variables definitions
 * ================================================================================*/

static st_ITMES st_types = {
    .u32ptr_init = v_exampleLocal,
    .u32ptr_read_reg = u8_exampleLocal,
};

/* function pointer */
static int8_t* (*arr[2])() = { v_magicNumber, v_magicNumber };

static int8_t dma_test_st[10] = {0,0,0,0,0,0,0,0,0,0};
static int8_t* s8ptr_calloc_st;

static uint32_t dma_u32[5] = {1,2,3,4,5}; 
static float dma_f[5] = {1,2,3,4,5}; 

static float  dma_variable_st = 66;
int8_t dma_variable_st_w = 10;
/* ==================================================================================
 * Local function declarations
 * ================================================================================*/

uint32_t* u32ptraa;

/* ==================================================================================
 * Local function implementation
 * ================================================================================*/
static  void v_exampleLocal(void)
{
	printf("here we are \n");
}

static  void v_magicNumber(void)
{
	printf("here we are %d\n", 42);
}

static int8_t u8_exampleLocal(uint8_t* u8ptr_status)
{
	printf("value is: %d\n", *u8ptr_status);

	return *u8ptr_status * (-1); 
}

void myPrintf(double fVal)
{
    char result[5];
    int dVal, i;
    int long dec;

    // fVal += 0.0000005;   // added after a comment from Matt McNabb, see below.
    printf("function address execution %d\n", myPrintf );
    printf("allocations arr%d\n", result );
    printf("allocations var %d\n", &fVal );

    dVal = fVal;
    dec = (int)(fVal * 100000) % 100000;

    if(dec < 0)
    {
    	dec *= (-1);
    }

	printf("%d.", dVal);
	printf("%d", dec);
    printf("\n");
    u32ptraa = malloc(1000);
    printf("allocations calloc %d\n", u32ptraa );
}


  int i;
#pragma omp critical
  {
    printf("[CORE %d] HW performance counters results\n", plp_coreId());
  
    for (i=0; i<hw_perf_nb_events(&perf); i++)
      {
        printf("%20s: %d\n", hw_perf_get_name(&perf, i), hw_perf_get_value(&perf, i));
  
      }
  }
  
}

// static int plp_dma_extToL1(unsigned int loc, unsigned int ext, unsigned short size
/* ==================================================================================
 * 		Main
 * ================================================================================*/
int main (int argc, char *argv[]) 
{
	float* first_allc = (float*) malloc(4);
	printf("first alloc %d\n", (int) first_allc );


	/* functions in struct */ 
	int i_max = 2;
	int k_max = 2;
	int n_max = 3;

	int i,k,n;
	int base = 100;
	int a, b; 
	uint32_t c;

/*-------------------------------------
* 	printff
*--------------------------------------*/
	float ff = -0.2;
	myPrintff(ff);


/*-------------------------------------
* 	dma tests 2
*--------------------------------------*/
	float *ptr1, *ptr2, *ptr3, *ptr4, *ptr5;
	uint32_t memory;
	uint32_t weights_size = 10;
	uint32_t parameter_size = 5;

	memory = weights_size;
	memory += 9 * parameter_size;

	ptr1 		= (float*) calloc(memory, sizeof(float)); 

	ptr2		= ptr1 + weights_size;

	ptr3 		= ptr1 + weights_size + 1*parameter_size;	
	ptr4 		= ptr1 + weights_size + 2*parameter_size;
	ptr5		= ptr1 + weights_size + 3*parameter_size;	

	printf("ptr1 %d\n", ptr1 );
	printf("ptr2 %d\n", ptr2 );
/*-------------------------------------
* 	performence start
*--------------------------------------*/	
	hw_perf_t perf;
	hw_perf_init(&perf);
	hw_perf_step(&perf);
	hw_perf_start(&perf);
/*-------------------------------------
* 	dma test 
*--------------------------------------*/
	int counter;
	int8_t* ptr = 0x10001000;
	int8_t dma_variable = 42;
	float* fptr_calloc = (float*) calloc(10,4);
	static int8_t dma_test[10] = {0,0,0,0,0,0,0,0,0,0};

	printf("static calloc before: %d \n", s8ptr_calloc_st);
	s8ptr_calloc_st = (int8_t*) calloc(10,1);
	/* stroage addresses */
	printf("array: %d \n", dma_test);
	printf("calloc: %d \n", fptr_calloc);
	printf("static array: %d \n", dma_test_st);
	printf("static calloc: %d \n", s8ptr_calloc_st);

	printf("static header variable: %d \n", &dma_variable_st);
	printf("header variable: %d \n", &dma_variable_st_w);
	printf("dynamic variable: %d \n", &dma_variable);

	uint8_t q = 0;
	/* transfer binary data */
	uint32_t* r_u32 =  (uint32_t*) malloc(5*4);
	printf("address %d \n", (int)r_u32 );

	counter = plp_dma_memcpy(&dma_u32, (unsigned int)r_u32, 20, PLP_DMA_EXT2LOC);
	plp_dma_wait(counter);
	for(q=0;q<5;q++)
	{
		printf("transfered binary %d\n", r_u32[q]);	
	}

	/* transfer float */
	float* r_f =  (float*) malloc(5*4);
	printf("address %d \n", (int)r_f );

	counter = plp_dma_memcpy(&dma_f, (unsigned int)r_f, 20, PLP_DMA_EXT2LOC);
	plp_dma_wait(counter);
	for(q=0;q<5;q++)
	{
		myPrintf( r_f[q]);	
	}




	printf("function address %d\n", myPrintf );

	myPrintf(48.61);

	free(fptr_calloc);
/*-------------------------------------
* 	performence end
*--------------------------------------*/
	hw_perf_stop(&perf);
	hw_perf_commit(&perf);

    printf("[CORE %d] HW performance counters results\n", plp_coreId());
  
    for (i=0; i<hw_perf_nb_events(&perf); i++)
      {
        printf("%20s: %d\n", hw_perf_get_name(&perf, i), hw_perf_get_value(&perf, i));
  
      }	
/*-------------------------------------
* 	parallel programming test 
*--------------------------------------*/
	uint8_t value = 2;
	uint8_t array[i_max*k_max*n_max];
	b = 0;
	#pragma omp parallel \
		shared(b) shared(base)
	{
		// printf("%d\n", base);
		int iter;
	#pragma omp for schedule(static) collapse(3) 
	for(i = 0; i < i_max; i++){
	for(k = 0; k < k_max; k++){
	for(n = 0; n < n_max; n++)
	{
		iter = (i*k_max + k)*n_max + n; 
		printf("%d\n", base);

		array[iter] = 1;

		#pragma omp critical 
		{
			b |= (0x1 << iter);
		}
	}}}
	} /* parallel */	
	
	printf("array: \n");
	for(a = 0; a < value; a++)
	{
		printf("%d\n", array[a]);
	}

	printf("shared value is: ");
	for(a = 31; a >= 0; a--)
	{
		c = ((0x1 << a) & b ) >> a;
		printf("%d", c);
	}
	printf("\n");
	// myPrintf(b);

/*-------------------------------------
* 	function pointer test 
*--------------------------------------*/
	// do
	// {
	printf("%d\n", value);
	   	// scanf("%c", &a);
	// } while(a != 'n');

	st_types.value = 2;

	st_types.u32ptr_init();

	st_types.value = st_types.u32ptr_read_reg(&(st_types.value));

	printf("now, value is: %d\n", st_types.value );

	/* functions in array */
	int8_t number = -1;
	uint8_t m = 3;

	(arr[0])();

	int8_t* result = (arr[1])(number,m);
  
	printf("now, number is %d  \n", *result );
}

#endif
