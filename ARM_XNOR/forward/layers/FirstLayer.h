/*

  Hard coded first layer of the convolutional network
       Convolution(&net, conv1, 32, 3, 3, 1, 1, 1, 1);
       BatchNormalization(&net, 1e-5);
       ReLU(&net);
       BatchNormalization(&net, 1e-4);

*/

#ifndef FIRSTLAYER_H
#define FIRSTLAYER_H

#include <cstdint>
#include <cmath>

#include "model.h"
#include "layer.h"
#include "tensor.h"
#include "util.h"


#if defined(myPopcount_methode_2)||defined(signGamma_signBetaStar_optimized)
void FirstLayer(Model *net, int32_t nFilters,
		int32_t kX, int32_t kY, int32_t dX, int32_t dY, 
		int32_t padX, int32_t padY, float eps1 = 0.0, 
		float eps2 = 0.0);
class FirstLayer_ : public Layer {

 public:
#ifdef TestFix16
    int16_t *weight, *bias;
    int16_t *mean1, *std1, *gamma1, *beta1;
    int16_t *mean2, *std2, *gamma2, *beta2;
    int32_t nFilters, kX, kY;
    int32_t padX, padY;
    int32_t dX, dY;

    FirstLayer_(int16_t* weight_, int16_t *bias_, int32_t nFilters,
		int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, int32_t padX_, int32_t padY_,
		int16_t *mean1_, int16_t *std1_, int16_t *gamma1_, int16_t *beta1_,
		int16_t *mean2_, int16_t *std2_, int16_t *gamma2_, int16_t *beta2_);
#elif defined(TestFix32)
    int32_t *weight, *bias;
    int32_t *mean1, *std1, *gamma1, *beta1;
    int32_t *mean2, *std2, *gamma2, *beta2;
    int32_t nFilters, kX, kY;
    int32_t padX, padY;
    int32_t dX, dY;

    FirstLayer_(int32_t* weight_, int32_t *bias_, int32_t nFilters,
		int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, int32_t padX_, int32_t padY_,
		int32_t *mean1_, int32_t *std1_, int32_t *gamma1_, int32_t *beta1_,
		int32_t *mean2_, int32_t *std2_, int32_t *gamma2_, int32_t *beta2_);
#else
    float *weight, *bias;
    float *mean1, *std1, *gamma1, *beta1;
    float *mean2, *std2, *gamma2, *beta2;
    int32_t nFilters, kX, kY;
    int32_t padX, padY;
    int32_t dX, dY;

    FirstLayer_(float* weight_, float *bias_, int32_t nFilters,
		int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, int32_t padX_, int32_t padY_,
		float *mean1_, float *std1_, float *gamma1_, float *beta1_,
		float *mean2_, float *std2_, float *gamma2_, float *beta2_);
#endif
    ~FirstLayer_();

    T* forward(T *input);

};

#else

void FirstLayer(Model *net, int32_t nFilters,
		int32_t kX, int32_t kY, int32_t dX, int32_t dY, 
		int32_t padX, int32_t padY, float eps1 = 0.0, 
		float eps2 = 0.0);
class FirstLayer_ : public Layer {

 public:

    float *weight, *bias;
    float *mean1, *std1, *gamma1, *beta1;
    float *mean2, *std2, *gamma2, *beta2;
    int32_t nFilters, kX, kY;
    int32_t padX, padY;
    int32_t dX, dY;

    FirstLayer_(float* weight_, float *bias_, int32_t nFilters,
		int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, int32_t padX_, int32_t padY_,
		float *mean1_, float *std1_, float *gamma1_, float *beta1_,
		float *mean2_, float *std2_, float *gamma2_, float *beta2_);
    ~FirstLayer_();

    T* forward(T *input);

};

#endif

#endif
