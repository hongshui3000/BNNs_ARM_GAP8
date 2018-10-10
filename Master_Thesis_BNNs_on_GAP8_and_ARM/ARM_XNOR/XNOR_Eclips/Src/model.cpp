#include "model.h"

/* DWT (Data Watchpoint and Trace) registers, only exists on ARM Cortex with a DWT unit */
#define KIN1_DWT_CONTROL             (*((volatile uint32_t*)0xE0001000))
/*!< DWT Control register */
#define KIN1_DWT_CYCCNTENA_BIT       (1UL<<0)
/*!< CYCCNTENA bit in DWT_CONTROL register */
#define KIN1_DWT_CYCCNT              (*((volatile uint32_t*)0xE0001004))
/*!< DWT Cycle Counter register */
#define KIN1_DEMCR                   (*((volatile uint32_t*)0xE000EDFC))
/*!< DEMCR: Debug Exception and Monitor Control Register */
#define KIN1_TRCENA_BIT              (1UL<<24)
/*!< Trace enable bit in DEMCR register */

#define KIN1_InitCycleCounter() \
KIN1_DEMCR |= KIN1_TRCENA_BIT
/*!< TRCENA: Enable trace and debug block DEMCR (Debug Exception and Monitor Control Register */

#define KIN1_ResetCycleCounter() \
KIN1_DWT_CYCCNT = 0
/*!< Reset cycle counter */

#define KIN1_EnableCycleCounter() \
KIN1_DWT_CONTROL |= KIN1_DWT_CYCCNTENA_BIT
/*!< Enable cycle counter */

#define KIN1_DisableCycleCounter() \
KIN1_DWT_CONTROL &= ~KIN1_DWT_CYCCNTENA_BIT
/*!< Disable cycle counter */

#define KIN1_GetCycleCounter() \
KIN1_DWT_CYCCNT

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
Model::Model(int32_t Z_, const uint32_t *b32Data_, const uint32_t *bSignData_, const float *fData_) {
	b32Data = b32Data_;
	bSignData = bSignData_;
	fData = fData_;
	b32Pos = 0;
	bSignPos = 0;
	fPos = 0;
#else
#ifdef TestFix16
	Model::Model(int32_t Z_, const uint32_t *b32Data_, const int8_t *b8Data_, const int16_t *fData_) {
		b32Data = b32Data_;
		b8Data = b8Data_;
		fData = fData_;
		b32Pos = 0;
		b8Pos = 0;
		fPos = 0;
#else
		Model::Model(int32_t Z_, const uint32_t *bData_, const float *fData_) {
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
		Model::Model(int32_t Z_, const uint32_t *bData_, const float *fData_) {
			bData = bData_;
			fData = fData_;
			bPos = 0;
			fPos = 0;
			Z = Z_;
			nLayers = 0;
		}
#endif
Model::~Model() {

	int32_t i;

	for (i = 0; i < nLayers; i++) {
		delete layers[i];
	}

}

void Model::addLayer(Layer *layer) {

	layers[nLayers] = layer;
	nLayers++;

}

T* Model::forward(T* input) {

	int32_t i;
	//int32_t cycles;
	T *output;
	for (i = 0; i < nLayers; i++) {
		//KIN1_InitCycleCounter(); /* enable DWT hardware */
		//KIN1_ResetCycleCounter(); /* reset cycle counter */
		//KIN1_EnableCycleCounter(); /* start counting */
		output = layers[i]->forward(input);
		//cycles = KIN1_GetCycleCounter(); /* get cycle counter */
		//KIN1_DisableCycleCounter(); /* disable counting if not used any more */
		input = output;
	}
	return output;
}

