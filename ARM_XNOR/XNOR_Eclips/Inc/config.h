#ifndef __CONFIG_H_
#define __CONFIG_H_

//#define originalxnor
//#define my32minusPopcount
//#define myPopcount
#define myPopcount_methode_2
//#define Popcount_sign_vectorized

#ifdef myPopcount_methode_2
#define data_optimized
#define with_sign_Gamma_negative
//#define MFCC_PREPROCESS
#define MFCC_V2
#define REPOINTER
#define INLINEF
#define UNFOLDED
//#define LED_USED
//#define LCD_USED
//#define TestFix16
	#ifdef TestFix16
		#define TestFix16_7
		//#define TestFix16_8
    	#ifdef TestFix16_7
			#define INPUTSCALE  7
			#define FIXSCALE    7
			#define FIXVALUE    128.0 //2^7
		#elif defined TestFix16_8
			#define INPUTSCALE  8
			#define FIXSCALE    8
			#define FIXVALUE    256.0 //2^7
    	#endif
#endif
//#define TestFix32
    #ifdef TestFix32
		#define INPUTSCALE  20
		#define FIXSCALE    20
		#define FIXVALUE    1048576.0
    #endif
#endif

#ifdef Popcount_sign_vectorized
#define signGamma_signBetaStar_optimized
//#define with_sign_Gamma_negative
#define UNFOLDED
//#define TestFix16
#endif

#endif
