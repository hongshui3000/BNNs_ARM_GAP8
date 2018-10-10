/*

  Generic layer class from which all the
  other layer classes are inherited

 */

#ifndef LAYER_H
#define LAYER_H

#include "tensor.h"
#include "config.h"


class Layer {

 public:
    virtual ~Layer();
#ifdef REPOINTER
    virtual T* forward(T* __restrict__ input);
#else
    virtual T* forward(T* __restrict__ input);
#endif
};

#endif
