/*

  Layer that combines into one the Binary Convolution, 
  ReLU, BatchNormalization, Binarization layers

*/

#ifndef BINCRBB_H
#define BINCRBB_H

#include <cstdint>
#include <cstdio>
#include "layer.h"
#include "tensor.h"
#include "util.h"


//#define originalxnor
//#define my32minusPopcount
//#define myPopcount
//#define myPopcount_methode_2

#ifdef originalxnor
class BinCRBB_ : public Layer {

 public:
    uint32_t *weight;
    int32_t *threshold2_int32; 
    int32_t *threshold1_int32;
    int8_t *signGamma, *compare;
    int32_t nFilters, kX, kY;
    int32_t padX, padY;
    int32_t dX, dY;
    BinCRBB_(uint32_t *weight_, int8_t *signGamma_, int32_t *threshold2_int32_,
	     int32_t *threshold1_int32_, int8_t *compare_, int32_t nFilters_,
	     int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, 
	     int32_t padX_, int32_t padY_);

    ~BinCRBB_();

    T* forward(T *input);

};
#elif defined(my32minusPopcount)
class BinCRBB_ : public Layer {

 public:
    uint32_t *weight;
    int32_t *threshold2_int32; 
    int32_t *threshold1_int32;
    int8_t *signGamma_positive, *signGamma_negative, *signBetaStar;
    int32_t nFilters, kX, kY;
    int32_t padX, padY;
    int32_t dX, dY;

    BinCRBB_(uint32_t *weight_, int8_t *signGamma_positive_, int8_t *signGamma_negative_, int32_t *threshold2_int32_,
	     int32_t *threshold1_int32_, int8_t *signBetaStar_, int32_t nFilters_,
	     int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, 
	     int32_t padX_, int32_t padY_);

    ~BinCRBB_();

    T* forward(T *input);

};
#elif defined(myPopcount)
class BinCRBB_ : public Layer {

 public:
    uint32_t *weight;
    int32_t *threshold1_int32, *threshold2_int32;
    int8_t *signGamma_positive, *signGamma_negative,*signBetaStar;
    int32_t nFilters, kX, kY;
    int32_t padX, padY;
    int32_t dX, dY;

    BinCRBB_(uint32_t *weight_, int8_t *signGamma_positive_, int8_t *signGamma_negative_, int32_t *threshold2_int32_,
	     int32_t *threshold1_int32_, int8_t *signBetaStar_, int32_t nFilters_,
	     int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, 
	     int32_t padX_, int32_t padY_);

    ~BinCRBB_();

    T* forward(T *input);

};


#elif defined(myPopcount_methode_2)
class BinCRBB_ : public Layer {

 public:
    uint32_t *weight;
    uint32_t *threshold1_int32, *threshold2_int32;
    uint8_t *signGamma_positive, *signBetaStar;
#ifdef with_sign_Gamma_negative
    uint8_t *signGamma_negative;
#endif
    int32_t nFilters, kX, kY;
    int32_t padX, padY;
    int32_t dX, dY;
#ifdef with_sign_Gamma_negative
    BinCRBB_(uint32_t *weight_, uint8_t *signGamma_positive_, uint8_t *signGamma_negative_, uint32_t *threshold2_int32_,
	     uint32_t *threshold1_int32_, uint8_t *signBetaStar_, int32_t nFilters_,
	     int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, 
	     int32_t padX_, int32_t padY_);
#else
    BinCRBB_(uint32_t *weight_, uint8_t *signGamma_positive_, uint32_t *threshold2_int32_,
	     uint32_t *threshold1_int32_, uint8_t *signBetaStar_, int32_t nFilters_,
	     int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, 
	     int32_t padX_, int32_t padY_);
#endif
    ~BinCRBB_();

    T* forward(T *input);

};

#elif defined(Popcount_sign_vectorized)
class BinCRBB_ : public Layer {

 public:
    uint32_t *weight;
    uint32_t *threshold1_int32, *threshold2_int32;
    uint32_t *signGamma_positive, *signBetaStar;
#ifdef with_sign_Gamma_negative
    uint32_t *signGamma_negative;
#endif
    int32_t nFilters, kX, kY;
    int32_t padX, padY;
    int32_t dX, dY;
#ifdef with_sign_Gamma_negative
    BinCRBB_(uint32_t *weight_, uint32_t *signGamma_positive_, uint32_t *signGamma_negative_, uint32_t *threshold2_int32_,
	     uint32_t *threshold1_int32_, uint32_t *signBetaStar_, int32_t nFilters_,
	     int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, 
	     int32_t padX_, int32_t padY_);
#else
    BinCRBB_(uint32_t *weight_, uint32_t *signGamma_positive_, uint32_t *threshold2_int32_,
	     uint32_t *threshold1_int32_, uint32_t *signBetaStar_, int32_t nFilters_,
	     int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, 
	     int32_t padX_, int32_t padY_);
#endif
    ~BinCRBB_();

    T* forward(T *input);

};

#endif
#endif
