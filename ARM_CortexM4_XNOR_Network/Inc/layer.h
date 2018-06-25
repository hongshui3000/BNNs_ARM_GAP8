/*

  Generic layer class from which all the
  other layer classes are inherited

 */

#ifndef LAYER_H
#define LAYER_H

#include "tensor.h"

//#define originalxnor
//#define my32minusPopcount
//#define myPopcount
#define myPopcount_methode_2
//#define Popcount_sign_vectorized

#ifdef myPopcount_methode_2
#define data_optimized
//#define with_sign_Gamma_negative
#define MFCC_PREPROCESS
//#define MFCC_V2
#define UNFOLDED
//#define LCD_USED
//#define TestFix16
#endif

#ifdef Popcount_sign_vectorized
#define signGamma_signBetaStar_optimized
//#define with_sign_Gamma_negative
#define UNFOLDED
//#define TestFix16
#endif


class Layer {

 public:
    virtual ~Layer();
    
    virtual T* forward(T* __restrict__ input);

};

#endif
