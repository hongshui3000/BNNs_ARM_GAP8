
#ifndef CONFIG_H_
#define CONFIG_H_

/*--------------------------------------------------
 1. Select a XNOR Network version 
	originalxnor: previous semester project configuration "XNOR Networks on Embedded Systems"
	myPopcount_methode_2: 	Li-Yung Chen's Contribution, optimized version.
	Popcount_sign_vectorized:Li-Yung Chen's Contribution, with comparison vectorization, but less efficient then 					myPopcount_methode_2
--------------------------------------------------*/

//#define originalxnor
//#define my32minusPopcount
//#define myPopcount
#define myPopcount_methode_2
//#define Popcount_sign_vectorized

/*---------------------------------------------------------
 Configuration for myPopcound_methode_2
	data_optimized: model parameter select (it must be selected)
	UNFOLDED: 	unrolling for loop in convolution operation, i.e. xnor and popcount
	with_sign_Gamma_negative: affects the comparison step. sign_Gamma_negative: gamma > 0 ===> 0 otherwise===>1
	MFCC_PREPROCESS: not finished yet.
	TestFix32: Forward 32 bits fixpoint input with 32 bits fixpoint weights and other parameters in First and Last Layers
 ---------------------------------------------------------*/

#ifdef myPopcount_methode_2
#define data_optimized
#define with_sign_Gamma_negative
#define UNFOLDED
//#define MFCC_PREPROCESS
//#define TestFix16
//#define TestFix32
//#define Testaccuracy
#endif

/*-------------------------------------------
 Configuration for Popcount_sign_vectorized
 -------------------------------------------*/
#ifdef Popcount_sign_vectorized
#define signGamma_signBetaStar_optimized
//#define with_sign_Gamma_negative
#define UNFOLDED
//#define TestFix16
//#define TestFix32
#endif





#endif /* define CONFIG_H_ */
