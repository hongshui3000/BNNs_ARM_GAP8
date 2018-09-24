/*

  Tensor class for managing feature map
  memory allocation between layers

  Initialize using

      T *tensor = new T(T_FLOAT, X, Y, Z);

  Data is stored in 1d blocks of memory
  Access element at position (i,j,k) using:

      tensor->f[(i*Y+j)*Z+k]

 */

#ifndef TENSOR_H
#define TENSOR_H

#include <cstdint>
#include <cstddef>
#include "config.h"
/*

  PARAMTERS:
  Preallocated blocks of memory, modify when changing
  network structure. Floating-point and binary blocks
  of memory are allocated separately.

*/

// Maximum size of tensor
const int32_t T_FLOAT_SIZE = 16*30*28;
const int32_t T_BINARY_SIZE = 64*120*2;

// Number of blocks
const int32_t TF_COUNT = 1;
const int32_t TB_COUNT = 2;

// Identifier for float or binary tensor
const int T_FLOAT = 0;
const int T_BINARY = 1;
const int T_FIXPOINT=2;

class T {
 public:
    T();
    T(int32_t type_, int32_t X_, int32_t Y_, int32_t Z_);
    ~T();

    // Size of the tensor
    int32_t X, Y, Z;
    // Type, either T_FLOAT or T_BINARY
    int32_t type;
    // Pointer to the allocated memory block
//#ifdef TestFix16
//	int16_t *f;
//#elif defined(TestFix32)
//	int32_t *f;
//#else
	float *f;
//#endif
    uint32_t *b;
#ifdef TestFix16
    uint16_t *fixp;
#elif defined(TestFix32)
    int32_t *fixp;
#else
#endif

    // Resize tensor
    void resize(int32_t type_, int32_t X_, int32_t Y_, int32_t Z_);
    // Deallocate
    void clear();

    // Binarize data and switch to T_BINARY type
    void binarize();

};

#endif
