#include "LastLayer.h"
//#define data_optimized


void LastLayer(Model *net, int32_t nFilters1, 
	       int32_t nFilters2, float eps){
    int32_t i, binZ;
#ifdef TestFix16
    // Binary layer
    int16_t *bias1 = new int16_t[nFilters1];
    read(bias1, net->fData, nFilters1, net->fPos);

    int16_t *alpha1 = new int16_t[nFilters1];
    read(alpha1, net->fData, nFilters1, net->fPos);
#elif defined(TestFix32)
    // Binary layer
    int32_t *bias1 = new int32_t[nFilters1];
    read(bias1, net->fData, nFilters1, net->fPos);

    int32_t *alpha1 = new int32_t[nFilters1];
    read(alpha1, net->fData, nFilters1, net->fPos);
#else
    // Binary layer
    float *bias1 = new float[nFilters1];
    read(bias1, net->fData, nFilters1, net->fPos);

    float *alpha1 = new float[nFilters1];
    read(alpha1, net->fData, nFilters1, net->fPos);
#endif
    binZ = net->Z/32;
    uint32_t *weight1 = new uint32_t[nFilters1*binZ];

#ifdef data_optimized
    for(i = 0; i < nFilters1; i++){
	read(&(weight1[i*binZ]), net->b32Data, 1, 1, binZ, net->b32Pos);
    }
#elif defined(signGamma_signBetaStar_optimized)
    for(i = 0; i < nFilters1; i++){
	read(&(weight1[i*binZ]), net->b32Data, 1, 1, binZ, net->b32Pos);
    }
#else
    for(i = 0; i < nFilters1; i++){
	read(&(weight1[i*binZ]), net->bData, 1, 1, binZ, net->bPos);
    }
#endif

#ifdef TestFix16
    // Batch Normalization
    int16_t *mean = new int16_t[MAX_DEPTH];
    read(mean, net->fData, nFilters1, net->fPos);
    
    int16_t *std = new int16_t[MAX_DEPTH];
    read(std, net->fData, nFilters1, net->fPos);
    
    int16_t *gamma = new int16_t[MAX_DEPTH];
    read(gamma, net->fData, nFilters1, net->fPos);
    
    int16_t *beta = new int16_t[MAX_DEPTH];
    read(beta, net->fData, nFilters1, net->fPos);
#elif defined(TestFix32)
    // Batch Normalization
    int32_t *mean = new int32_t[MAX_DEPTH];
    read(mean, net->fData, nFilters1, net->fPos);
    
    int32_t *std = new int32_t[MAX_DEPTH];
    read(std, net->fData, nFilters1, net->fPos);
    
    int32_t *gamma = new int32_t[MAX_DEPTH];
    read(gamma, net->fData, nFilters1, net->fPos);
    
    int32_t *beta = new int32_t[MAX_DEPTH];
    read(beta, net->fData, nFilters1, net->fPos);
#else
    // Batch Normalization
    float *mean = new float[MAX_DEPTH];
    read(mean, net->fData, nFilters1, net->fPos);
    
    float *std = new float[MAX_DEPTH];
    read(std, net->fData, nFilters1, net->fPos);
    
    float *gamma = new float[MAX_DEPTH];
    read(gamma, net->fData, nFilters1, net->fPos);
    
    float *beta = new float[MAX_DEPTH];
    read(beta, net->fData, nFilters1, net->fPos);
#endif

#if defined (data_optimized) || defined(signGamma_signBetaStar_optimized)
#else
    for(i = 0; i < nFilters1; i++){
	std[i] = 1.0/sqrt(std[i]+eps);
    }
#endif
    net->Z = nFilters1;

#ifdef TestFix16
    // Full-precision layer
    int16_t *bias2 = new int16_t[nFilters2];
    read(bias2, net->fData, nFilters2, net->fPos);

    int16_t *weight2 = new int16_t[nFilters2*net->Z];
    for(i = 0; i < nFilters2; i++){
	read(&(weight2[i*net->Z]), net->fData, 1, 1, net->Z, net->fPos);
    }
#elif defined(TestFix32)
    // Full-precision layer
    int32_t *bias2 = new int32_t[nFilters2];
    read(bias2, net->fData, nFilters2, net->fPos);

    int32_t *weight2 = new int32_t[nFilters2*net->Z];
    for(i = 0; i < nFilters2; i++){
	read(&(weight2[i*net->Z]), net->fData, 1, 1, net->Z, net->fPos);
    }
#else
    // Full-precision layer
    float *bias2 = new float[nFilters2];
    read(bias2, net->fData, nFilters2, net->fPos);

    float *weight2 = new float[nFilters2*net->Z];
    for(i = 0; i < nFilters2; i++){
	read(&(weight2[i*net->Z]), net->fData, 1, 1, net->Z, net->fPos);
    }
#endif

    LastLayer_ *layer = new LastLayer_(weight1, bias1, alpha1, nFilters1,
				       weight2, bias2, nFilters2, mean, std,
				       gamma, beta);

    net->addLayer(layer);

    net->Z = nFilters2;

}
#ifdef TestFix16
LastLayer_::LastLayer_(uint32_t *weight1_, int16_t *bias1_, int16_t *alpha1_, 
		      int32_t nFilters1_, int16_t *weight2_, int16_t *bias2_,
		      int32_t nFilters2_, int16_t *mean_, int16_t *std_, 
		      int16_t *gamma_, int16_t *beta_){
    
    weight1 = weight1_;
    bias1 = bias1_;
    alpha1 = alpha1_;
    nFilters1 = nFilters1_;
    weight2 = weight2_;
    bias2 = bias2_;
    nFilters2 = nFilters2_;
    mean = mean_;
    std = std_;
    gamma = gamma_;
    beta = beta_;

}
#elif defined(TestFix32)
LastLayer_::LastLayer_(uint32_t *weight1_, int32_t *bias1_, int32_t *alpha1_, 
		      int32_t nFilters1_, int32_t *weight2_, int32_t *bias2_,
		      int32_t nFilters2_, int32_t *mean_, int32_t *std_, 
		      int32_t *gamma_, int32_t *beta_){
    
    weight1 = weight1_;
    bias1 = bias1_;
    alpha1 = alpha1_;
    nFilters1 = nFilters1_;
    weight2 = weight2_;
    bias2 = bias2_;
    nFilters2 = nFilters2_;
    mean = mean_;
    std = std_;
    gamma = gamma_;
    beta = beta_;

}
#else
LastLayer_::LastLayer_(uint32_t *weight1_, float *bias1_, float *alpha1_, 
		      int32_t nFilters1_, float *weight2_, float *bias2_,
		      int32_t nFilters2_, float *mean_, float *std_, 
		      float *gamma_, float *beta_){
    
    weight1 = weight1_;
    bias1 = bias1_;
    alpha1 = alpha1_;
    nFilters1 = nFilters1_;
    weight2 = weight2_;
    bias2 = bias2_;
    nFilters2 = nFilters2_;
    mean = mean_;
    std = std_;
    gamma = gamma_;
    beta = beta_;

}
#endif
LastLayer_::~LastLayer_(){
    delete[] weight1;
    delete[] weight2;
    delete[] bias1;
    delete[] alpha1;
    delete[] bias2;
    delete[] mean;
    delete[] std;
    delete[] gamma;
    delete[] beta;

}

T* LastLayer_::forward(T *input){

    int32_t i, j, k, kk, sum1;
#ifdef TestFix16
    int32_t sum2;
#elif defined(TestFix32)
    int64_t sum2;
#else
    float sum2;
#endif
    T *output;

    if(input->type == T_FLOAT){
	input->binarize();
    }

#ifdef TestFix16
    int32_t *inter = new int32_t[nFilters1];
#elif defined(TestFix32)
    int32_t *inter = new int32_t[nFilters1];
#else
    float *inter = new float[nFilters1];
#endif

    output = new T(T_FLOAT, input->X, input->Y, nFilters2);
/*	for(i = 0; i < input->X; i++){
		for(j = 0; j < input->Y; j++){
			for(k = 0; k < output->Z; k++){
				output->f[(i*output->Y+j)*output->Z+k] = 0;
			}
		}
	}*/

    for(i = 0; i < input->X; i++){
	for(j = 0; j < input->Y; j++){
	    // Calculated the binary layer
	    k = 0;
	    do{
//	    for(k = 0; k < nFilters1; k++){
#ifdef UNFOLDED
		sum1 = 64 - popcount(weight1[k*input->Z + 0]^input->b[(i*input->Y+j)*input->Z +0])\
			-popcount(weight1[k*input->Z + 1]^input->b[(i*input->Y+j)*input->Z + 1])\
			-popcount(weight1[k*input->Z + 2]^input->b[(i*input->Y+j)*input->Z + 2])\
			-popcount(weight1[k*input->Z + 3]^input->b[(i*input->Y+j)*input->Z + 3]);
		sum1 = 2*sum1;
#else
//		sum1 = 0;
		sum1 = 16 * input->Z;
		for(kk = 0; kk < input->Z; kk++){
		    sum1 -= popcount(weight1[k*input->Z+kk]^input->b[(i*input->Y+j)*input->Z+kk]);
		}
		sum1 *= 2;
#endif
		// Store intermediary results
#ifdef TestFix16
		inter[k] = (0.0 > (sum1*alpha1[k]+bias1[k]))? 0.0 : (sum1*alpha1[k] + bias1[k]);
		inter[k] = (((((inter[k]-mean[k])*std[k])>>8)*gamma[k])>>8)+beta[k];
#elif defined(TestFix32)
		inter[k] = (0.0 > (sum1*(int64_t)alpha1[k]+bias1[k]))? 0.0 : (sum1*(int64_t)alpha1[k] + bias1[k]);
		inter[k] = (((((inter[k]-mean[k])*(int64_t)std[k])>>20)*(int64_t)gamma[k])>>20)+beta[k];
#else
		inter[k] = max(0.0, sum1*alpha1[k]+bias1[k]);
		inter[k] = (inter[k]-mean[k])*std[k]*gamma[k]+beta[k];
#endif
		k++;
	    }while(k < nFilters1);

	    // Calculate the full-point layer
	    for(k = 0; k < nFilters2; k++){
		sum2 = 0;

		for(kk = 0; kk < nFilters1; kk++){
	#ifdef TestFix16
		    sum2 += (weight2[k*nFilters1+kk]*inter[kk])>>8;
	#elif defined(TestFix32)
		    sum2 += (int64_t)(weight2[k*nFilters1+kk]*(int64_t)inter[kk])>>20;
	#else
		    sum2 += weight2[k*nFilters1+kk]*inter[kk];
	#endif
		}
	#ifdef TestFix16
		output->f[(i*output->Y+j)*output->Z+k] = ((float)(sum2+bias2[k]))/(float)256;
	#elif defined(TestFix32)
		output->f[(i*output->Y+j)*output->Z+k] = ((float)(sum2+bias2[k]))/(float)1048576;
	#else
		output->f[(i*output->Y+j)*output->Z+k] = sum2+bias2[k];
	#endif
	    }
	}
    }
    delete[] inter;
    delete input;

    return output;
    
}

