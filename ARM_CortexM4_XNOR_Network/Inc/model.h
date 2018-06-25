/*

  Class that holds the model

 */

#ifndef MODEL_H
#define MODEL_H

#include <cstdint>

#include "tensor.h"
#include "layer.h"

/* 

   PARAMETERS:
   The maximum number of layers in the network and
   the maximum depth/number of channels in a layer

*/

const int32_t MAX_LAYERS = 32;
const int32_t MAX_DEPTH = 128;

#ifdef myPopcount_methode_2

class Model {
 public:
#ifdef data_optimized
    Model(int32_t Z_, const uint32_t *b32Data_, const uint8_t *b8Data_, const float *fData_);
#else
#ifdef TestFix16
    Model(int32_t Z_, const uint32_t *b32Data_, const int8_t *b8Data_, const int16_t *fData_);
#else
    Model(int32_t Z_, const uint32_t *bData_, const float *fData_);
#endif
#endif
    ~Model();

    // Pointers to the paramters of the network
#ifdef data_optimized
    const uint32_t *b32Data;
    const uint8_t *b8Data;
    const float *fData;
    // Position in the parameters array
    int32_t b32Pos, b8Pos, fPos;
#else
#ifdef TestFix16
    const uint32_t *b32Data;
    const int8_t *b8Data;
    const int16_t *fData;
    // Position in the parameters array
    int32_t b32Pos, b8Pos, fPos;
#else
    const uint32_t *bData;
    const float *fData;
    // Position in the parameters array
    int32_t bPos, fPos;
#endif
#endif

    // Number of output channels
    int32_t Z;
    // Number of layers
    int32_t nLayers;
    Layer *layers[MAX_LAYERS];
    void addLayer(Layer *layer);
 
    // Forward pass, the input tensor will be 
    // deallocated inside the method
    T* forward(T* input);

};

#elif defined(Popcount_sign_vectorized)

class Model {
 public:
#ifdef signGamma_signBetaStar_optimized
    Model(int32_t Z_, const uint32_t *b32Data_, const uint32_t *bSignData_, const float *fData_);
#else
#ifdef TestFix16
    Model(int32_t Z_, const uint32_t *b32Data_, const int8_t *b8Data_, const int16_t *fData_);
#else
    Model(int32_t Z_, const uint32_t *bData_, const float *fData_);
#endif
#endif
    ~Model();

    // Pointers to the paramters of the network
#ifdef signGamma_signBetaStar_optimized
    const uint32_t *b32Data;
    const uint32_t *bSignData;
    const float *fData;
    // Position in the parameters array
    int32_t b32Pos, bSignPos, fPos;

#else
#ifdef TestFix16
    const uint32_t *b32Data;
    const int8_t *b8Data;
    const int16_t *fData;
    // Position in the parameters array
    int32_t b32Pos, b8Pos, fPos;
#else
    const uint32_t *bData;
    const float *fData;
    // Position in the parameters array
    int32_t bPos, fPos;
#endif
#endif

    // Number of output channels
    int32_t Z;
    // Number of layers
    int32_t nLayers;
    Layer *layers[MAX_LAYERS];
    void addLayer(Layer *layer);

    // Forward pass, the input tensor will be
    // deallocated inside the method
    T* forward(T* input);

};

#else
class Model {
 public:
    Model(int32_t Z_, const uint32_t *bData_, const float *fData_);
    ~Model();

    // Pointers to the paramters of the network
    const uint32_t *bData;
    const float *fData;
    // Position in the parameters array
    int32_t bPos, fPos;

    // Number of output channels
    int32_t Z;
    // Number of layers
    int32_t nLayers;
    Layer *layers[MAX_LAYERS];
    void addLayer(Layer *layer);

    // Forward pass, the input tensor will be
    // deallocated inside the method
    T* forward(T* input);

};
#endif
#endif
