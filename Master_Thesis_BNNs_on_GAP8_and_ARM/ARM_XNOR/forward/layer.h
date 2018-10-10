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
    
    virtual T* forward(T *input);

};

#endif
