/*

  Binary convolutional layer, the bOutput
  can be set to true to generate binary output
  with the BinCRRB layer and avoid full-precision
  operations during inference

*/

#ifndef BINCONVOLUTION_H
#define BINCONVOLUTION_H

#include <cstdint>
#include <cmath>

#include "model.h"
#include "layer.h"
#include "tensor.h"
#include "util.h"

#include "layers/BinCRBB.h"
#include "stm32469i_discovery.h"

void BinConvolution(Model *net, int32_t nFilters,
		    int32_t kX, int32_t kY, int32_t dX, int32_t dY,
		    int32_t padX, int32_t padY,
		    int32_t bOutput = false, float eps = 0.0);

class BinConvolution_ : public Layer {

 public:
    uint32_t *weight;
    float *bias, *alpha;
    int32_t nFilters, kX, kY;
    int32_t padX, padY;
    int32_t dX, dY;

    BinConvolution_(uint32_t *weight_, float *bias_, float *alpha_,
		    int32_t nFilters, int32_t kX_, int32_t kY_, 
		    int32_t dX_, int32_t dY_, 
		    int32_t padX_, int32_t padY_);
    ~BinConvolution_();

    T* forward(T *input);

};


#endif
