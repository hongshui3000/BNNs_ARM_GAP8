Li-Yung, Chen. Masther thesis at ETH Zurich

Topic: Implementation and Optimization of BNNs in Embedded Platform and IoT

Folder:
	/model		: Model data on GAP8, DMA, memory allocation, 

	/layers		: XNOR Network on GAP8

	/utility	: functions definitions, main functions.

	/preprocess	: Precalculate floating point model data into 32 bits or 16 bits fixpoint

	/mfcc           : MFCC preprocessing using Greenwaves FFT library

Yout will see more details in individual folder "README.md"

--------------------------------------------------------------
1. How to use:
--------------------------------------------------------------
To set up default configuration

	$ make conf

To use virtual platform

	$ make conf platform=gvsoc

To use the board, loading binary on gap8 board

	$ make conf platform=board

After the configuration, then for the compiling, (loading binary) and run:

To compile and run on virtual platform/board

	$ make clean all run
	
------------------------------------------------------------
2. Configuration using MACRO :
------------------------------------------------------------	

===========
cluster.c
cluster.h
These two are for testing basic parallelization operation of OpenMP
Not-project relevant

===========


api.c					: Code for Fabric Controller, run the function "firstfunction" in utility/main_function.c.
utility/main_function.c : Call MFCCs preprocessing function, call BNNs forwarding function.
config.h				:
	MACRO:
		#define UNFOLDED       --> enable for loop unrolling
		#define FIXPOINT_32BIT --> using 32 bit to store model in first layer and last layer
		#define MYPOPCOUNT     --> map: 32-2*popcount(I xor W)  -----> popcount(I xor W) in binary layers (my contribution)
		#define I2S_DEMO       --> activate microphone application
		#define HIGH_PASS      --> remove low frequency bins in MFCCs preprocessing on Estimeate_power step
		#define MFCC_PREPROCESSING --> enable MFCCs preprocessing, input is sound samples, 16kHz
		#define DMA_TRANS --> enable DMA for loading model from L2 to L1 
		#define MULTICORES --> enable parallelization of BNNs and MFCC.



			******some issues when using pulp_sdk git tag: 2018.08.06******
			MFCC: filtering still 4 cores. Full 8 cores parallelization in filter causes wrong results in this sdk version
					(fully 8 cores parallelization work in previous version of pulp_sdk before 2018.08.04 version)
				--The number of barriers in Radix2FFT_DIF_args2 function in mfcc.c should be readuced to only 1.
			FirstLayer: 
				 #pragma omp for schedule(static) --> not works (#pragma omp for schedule(dynamic) is used instead.)
			BinaryLayer:
				 #pragma omp for schedule(static) --> not works (#pragma omp for schedule(dynamic) is used instead.)

mfcc/mfcc.c		   	    	: 
			the look-up table will be first loaded from L2 to L1 and after generating 1 tile of feature map, the look-up table will be released from L1. There is no logarithm after filering in this implementation


			default setting	
			#define SAMP_FREQ         16000   //sampling rate of sound
			#define NUM_FBANK_BINS    24	  //number of triangular filters
			#define NUM_MFCC_FEATURES 64
			#define WIN_LENGTH        512     //512 points windows
			#define WIN_OVERLAP       384     //384 points overlap with next window --> 128 points windowing step
			#define IMG_WIDTH         400     //MFCCs feature dimension x (time)
			#define MFCC_BINS         64      //MFCCs feature dimension y (Mel scale (frequency) after DCT)

mfcc/mfcc.h		   	    	: 
			including look up table for DCT, FFT, filtering with Q2.13 16 bit integer.


--------------------------------------------------------------
3. GAPuino board issues:
--------------------------------------------------------------
Prototype number PROTO0217:
1. Poor USB connection, easily disturbed and disconnected.
2. Binary cannot be loaded into the board. JTAG issue occurs.

Prototype number PROTO0219:
1. USB cannot power up the board.
2. Have to power up by the Power Supply with 10V (current 70-110 mA) so that USB can then load the binary into the board. The board can still work but it just can not powered by USB.

--------------------------------------------------------------
4. Other files in /gap8 :
--------------------------------------------------------------
There are many testing data in /gap8 folder.
Basically, including one of the input data in utility/main_function.c  (sound samples with MFCC preprocessing and BNNs /or preprocessed MFCCs features with BNNs only), then we can test with sound sample input/ MFCCs feature input.

--------------------------------------------------------------
5. Some tricks :
--------------------------------------------------------------

	1. "printf("%f", (float) float_var)" is not supported by sdk. Cannot print float data. Please using myPrintf function in utility/misc.c
	2. For freeing L1 memory, the pointer and the size of memory to be released must be specified:
		rt_free(RT_ALLOC_CL_DATA, pointer, size_to_be_pointed)
