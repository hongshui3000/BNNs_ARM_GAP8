Basically, these scripts are same in /ARM_XNOR/forward, but this one is for generating binary on STM32F469 board.
The IDE Eclipse is used.
The optimization is using -O2.

Important:

Frequency: 180 MHz.
Microphone is not used.
MFCC preprocessing raw sound samples from dataset.

/Inc/config.h : 
	#define data_optimized   //no need to calculate reciprocal of std on runtime
	#define with_sign_Gamma_negative  //store also 0 1 for sign positive and negative (1 0 for sign positive)
	#define MFCC_V2       //MFCC using CMSIS library, floating point
	#define REPOINTER     //enable restrict pointer
	#define INLINEF       //enable inline function for popcount
	#define UNFOLDED      //enable for loop unrolling
