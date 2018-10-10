#include "tensor.h"

#ifdef TestFix16
static uint16_t tfx1[T_FLOAT_SIZE];
#elif defined(TestFix32)
static int32_t tfx1[T_FLOAT_SIZE];
//#else
#endif
static float tf1[T_FLOAT_SIZE];
//#endif
static uint32_t tb1[T_BINARY_SIZE], tb2[T_BINARY_SIZE];

#ifdef TestFix16
uint16_t *const tfx[] = {tfx1};
#elif defined(TestFix32)
int32_t *const tfx[] = {tfx1};
//#else
#endif
float *const tf[] = {tf1};
//#endif
uint32_t *const tb[] = {tb1, tb2};

static int32_t tfused[TF_COUNT] = {0};
static int32_t tbused[TB_COUNT] = {0, 0};

T::T(){

	b = NULL;
	f = NULL;
#if defined(TestFix16)||defined(TestFix32)
	fixp = NULL;
#endif
}

T::T(int32_t type_, int32_t X_, int32_t Y_, int32_t Z_){

	int32_t i;

	b = NULL;
	f = NULL;
#if defined(TestFix16)||defined(TestFix32)
	fixp = NULL;
#endif
	resize(type_, X_, Y_, Z_);

	if(type == T_BINARY){
		for(i = 0; i < TB_COUNT; i++){
			if(!tbused[i]){
				tbused[i] = 1;
				b = tb[i];
				break;
			}
		}
	}
#if defined(TestFix16)||defined(TestFix32)
	else if(type == T_FIXPOINT)
	{
		for(i = 0; i < TF_COUNT; i++){
			if(!tfused[i]){
				tfused[i] = 1;
				fixp = tfx[i];
				break;
			}
		}

	}
#endif
	else{
		for(i = 0; i < TF_COUNT; i++){
			if(!tfused[i]){
				tfused[i] = 1;
				f = tf[i];
				break;
			}
		}
	}

}

T::~T(){

	clear();

}

void T::resize(int32_t type_, int32_t X_, int32_t Y_, int32_t Z_){

	type = type_;
	X = X_;
	Y = Y_;

	if(type == T_BINARY){
		Z = Z_ / 32;
	} else {
		Z = Z_;
	}

}

void T::clear(){

	int32_t i;


	if(type == T_BINARY){
		if(b){
			for(i = 0; i < TB_COUNT; i++){
				if(b == tb[i]){
					tbused[i] = 0;
					b = NULL;
					break;
				}
			}
		}
	}
#if defined(TestFix16)||defined(TestFix32)
	else if(type == T_FIXPOINT)
	{
		if(fixp){
			for(i = 0; i < TF_COUNT; i++){
				if(fixp == tfx[i]){
					tfused[i] = 0;
					fixp = NULL;
					break;
				}
			}
		}

	}
#endif
	else
	{
		if(f){
			for(i = 0; i < TF_COUNT; i++){
				if(f == tf[i]){
					tfused[i] = 0;
					f = NULL;
					break;
				}
			}
		}
	}

}

void T::binarize(){

	int32_t i, j, k, realK, shift;

	T *bin = new T(T_BINARY, X, Y, Z);

	for(i = 0; i < X; i++){
		for(j = 0; j < Y; j++){
			realK = 0;
			for(k = 0; k < bin->Z; k++){
				bin->b[(i*bin->Y+j)*bin->Z+k] = 0;
				for(shift = 0; shift < 32; shift++){
					if(f[(i*Y+j)*Z+realK] > 0){
						bin->b[(i*bin->Y+j)*bin->Z+k] |= 1<<shift;
					}
					realK++;
				}
			}
		}
	}

	bin->type = T_FLOAT;
	bin->f = f;
	f = NULL;

	type = T_BINARY;
	b = bin->b;
	Z = bin->Z;

	delete bin;

}
