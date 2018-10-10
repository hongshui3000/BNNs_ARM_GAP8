#include "model.h"

#ifdef myPopcount_methode_2
	#ifdef data_optimized
		#ifdef TestFix16
Model::Model(int32_t Z_, const uint32_t *b32Data_, const uint8_t *b8Data_, const int16_t *fData_){
    b32Data = b32Data_;
    b8Data = b8Data_;
    fData = fData_;
    b32Pos = 0;
    b8Pos = 0;
    fPos = 0;
		#elif defined(TestFix32)
Model::Model(int32_t Z_, const uint32_t *b32Data_, const uint8_t *b8Data_, const int32_t *fData_){
    b32Data = b32Data_;
    b8Data = b8Data_;
    fData = fData_;
    b32Pos = 0;
    b8Pos = 0;
    fPos = 0;
		#else
Model::Model(int32_t Z_, const uint32_t *b32Data_, const uint8_t *b8Data_, const float *fData_){
    b32Data = b32Data_;
    b8Data = b8Data_;
    fData = fData_;
    b32Pos = 0;
    b8Pos = 0;
    fPos = 0;
		#endif
	#else
Model::Model(int32_t Z_, const uint32_t *bData_, const float *fData_){
    bData = bData_;
    fData = fData_;
    bPos = 0;
    fPos = 0;
	#endif
    Z = Z_;
    nLayers = 0;

}

#elif defined(Popcount_sign_vectorized)
	#ifdef signGamma_signBetaStar_optimized
Model::Model(int32_t Z_, const uint32_t *b32Data_, const uint32_t *bSignData_, const float *fData_){
    b32Data = b32Data_;
    bSignData = bSignData_;
    fData = fData_;
    b32Pos = 0;
    bSignPos = 0;
    fPos = 0;
	#else
		#ifdef TestFix16
Model::Model(int32_t Z_, const uint32_t *b32Data_, const int8_t *b8Data_, const int16_t *fData_){
    b32Data = b32Data_;
    b8Data = b8Data_;
    fData = fData_;
    b32Pos = 0;
    b8Pos = 0;
    fPos = 0;
		#elif defined(TestFix32)
Model::Model(int32_t Z_, const uint32_t *b32Data_, const int8_t *b8Data_, const int32_t *fData_){
    b32Data = b32Data_;
    b8Data = b8Data_;
    fData = fData_;
    b32Pos = 0;
    b8Pos = 0;
    fPos = 0;
		#else
Model::Model(int32_t Z_, const uint32_t *bData_, const float *fData_){
    bData = bData_;
    fData = fData_;
    bPos = 0;
    fPos = 0;
		#endif
	#endif
    Z = Z_;
    nLayers = 0;

}

#else
Model::Model(int32_t Z_, const uint32_t *bData_, const float *fData_){
    bData = bData_;
    fData = fData_;
    bPos = 0;
    fPos = 0;
    Z = Z_;
    nLayers = 0;
}
#endif
Model::~Model(){

    int32_t i;

    for(i = 0; i < nLayers; i++){
	delete layers[i];
    }

}

void Model::addLayer(Layer *layer){

    layers[nLayers] = layer;
    nLayers++;

}


T* Model::forward(T* input){

	int32_t i;
	T *output;
	for(i = 0; i < nLayers; i++)
	{
		output = layers[i]->forward(input);
			input = output;
	}
    return output;
}

