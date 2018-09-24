---------------------------------------------------------
1. Setting up
---------------------------------------------------------

Before changing the network structure modify the parameters in the following files:

tensor.h: 	Size and number of memory blocks
model.h: 	Maximum depth and maximum number of layers in the network
main.cpp: 	Network structure initialization and the and for the
        	weights of the convolutional filters
layers/BinConvolution.cpp:	Allocating memory for binarized weights, Batch Norm parameters and thresholds for Comparison
layers/FirstLayer.cpp:		Floating point weight and input, convolution, Batchn Normalization and finally Binarization
layers/BinCRBB.cpp:		Binary Convolution and Comparisons and generates binary output feature maps.
layers/LastLayer.cpp:		Last binary Convolution and followed by LastLayer Floating point convolution.

config.h:	By selecting #define, you can manipulate different configurations:
		    In Li-Yung's Thesis:
			#define myPopcount_methode_2  //select this for using the threshold comparison method
			#define data_optimized        //select this and not to precalculate the reciprocal std on runtime (already computed)
			#define with_sign_Gamma_negative
			#define UNFOLDED		// enable for loop unrolling
			#define TestFix32         //To test fixed-point in 32 bit integer form.
			#define INPUTSCALE  20   //for Qn.20 format of MFCCs features
			#define FIXSCALE    10   //for Qn.10 format of model data 
			#define FIXVALUE    1024.0 // 2 to the power of FIXSCALE (10)
			#define Testaccuracy    // For testing dataset of MFCC features 64x400 csv file.
						// Select the correct the dataset Path. (instead of in /sem17f11)

---------------------------------------------------------
2. Usage
---------------------------------------------------------
This file is for the host machine.

$ make clean main
$ ./main

---------------------------------------------------------
3. Observations
---------------------------------------------------------

- The model file included reached a precision of 63%
- The test image included is test image number 2
- The network can not handle convolutions with feature maps
  that don't have a depth of 32
- Due to floating point precisions the final numerical result may vary
  by less than 1% (classification remains the same for all the images in
  the test set)
- MFCC preprocessing is not integrated for host machine.
