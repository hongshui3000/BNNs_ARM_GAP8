/*

  Hard coded last layer of the convolutional network

      BinConvolution(&net, conv6, 128, 1, 1, 1, 1, 0, 0);
      ReLU(&net);
      BatchNormalization(&net, 1e-3);
      Convolution(&net, conv7, nClasses, 1, 1, 1, 1, 0, 0);


 */

#ifndef LASTLAYER_H
#define LASTLAYER_H

#include <cstdint>
#include <cmath>

#include "model.h"
#include "layer.h"
#include "tensor.h"
#include "util.h"
#include "stm32469i_discovery.h"

#if defined(myPopcount_methode_2)||defined(signGamma_signBetaStar_optimized)
void LastLayer(Model *net, int32_t nFilters1,
	       int32_t nFilters2, float eps = 0.0);
class LastLayer_ : public Layer {

 public:
#ifdef TestFix16
    uint32_t *weight1;
    int16_t *bias1, *alpha1;
    int16_t *weight2, *bias2;
    int16_t *mean, *std, *gamma, *beta;
    int32_t nFilters1, nFilters2;

    LastLayer_(uint32_t *weight1_, int16_t *bias1_, int16_t *alpha1_,
	       int32_t nFilters1_, int16_t *weight2_, int16_t *bias2_,
	       int32_t nFilters2_, int16_t *mean_, int16_t *std_,
	       int16_t *gamma_, int16_t *beta_);
#elif defined(TestFix32)
    uint32_t *weight1;
    int32_t *bias1, *alpha1;
    int32_t *weight2, *bias2;
    int32_t *mean, *std, *gamma, *beta;
    int32_t nFilters1, nFilters2;

    LastLayer_(uint32_t *weight1_, int32_t *bias1_, int32_t *alpha1_,
	       int32_t nFilters1_, int32_t *weight2_, int32_t *bias2_,
	       int32_t nFilters2_, int32_t *mean_, int32_t *std_,
	       int32_t *gamma_, int32_t *beta_);
#else
    uint32_t *weight1;
    float *bias1, *alpha1;
    float *weight2, *bias2;
    float *mean, *std, *gamma, *beta;
    int32_t nFilters1, nFilters2;

    LastLayer_(uint32_t *weight1_, float *bias1_, float *alpha1_,
	       int32_t nFilters1_, float *weight2_, float *bias2_,
	       int32_t nFilters2_, float *mean_, float *std_,
	       float *gamma_, float *beta_);
#endif

    ~LastLayer_();
#ifdef REPOINTER
    T* forward(T *input);
#else
    T* forward(T *input);
#endif
};

#else
void LastLayer(Model *net, int32_t nFilters1,
	       int32_t nFilters2, float eps = 0.0);
class LastLayer_ : public Layer {

 public:

    uint32_t *weight1;
    float *bias1, *alpha1; 
    float *weight2, *bias2;
    float *mean, *std, *gamma, *beta;
    int32_t nFilters1, nFilters2;

    LastLayer_(uint32_t *weight1_, float *bias1_, float *alpha1_, 
	       int32_t nFilters1_, float *weight2_, float *bias2_,
	       int32_t nFilters2_, float *mean_, float *std_, 
	       float *gamma_, float *beta_);
    ~LastLayer_();

    T* forward(T *input);

};
#endif

#endif
