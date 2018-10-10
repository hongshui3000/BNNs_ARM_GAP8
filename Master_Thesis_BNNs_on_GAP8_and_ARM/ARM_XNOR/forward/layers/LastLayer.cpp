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

    //for(i=0;i<nFilters1;i++){
//	    printf("Last bias1[%d] = %f\n", i ,bias1[i]);
//	}
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
    
//    for(i=0;i<nFilters1;i++){
//	    printf("Last gamma[%d] = %f\n", i ,gamma[i]);
//	}
    float *beta = new float[MAX_DEPTH];
    read(beta, net->fData, nFilters1, net->fPos);
//    for(i=0;i<nFilters1;i++){
//	    printf("Last beta[%d] = %f\n", i ,beta[i]);
//	}
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

//    for(i=0;i<nFilters2;i++){
//	    printf("Last bias2[%d] = %f\n", i ,bias2[i]);
//	}
    float *weight2 = new float[nFilters2*net->Z];
    for(i = 0; i < nFilters2; i++){
	read(&(weight2[i*net->Z]), net->fData, 1, 1, net->Z, net->fPos);
    }
#endif
//    for(int kkk=0; kkk<nFilters1; kkk++)
//    {   if(bias1[kkk]>1)
//        printf("Last bias1 = %f\n",bias1[kkk]);}
//    for(int kkk=0; kkk<nFilters1; kkk++)
//    {   if(alpha1[kkk]>1)
//            printf("Last alpha1 = %f\n",alpha1[kkk]);}
//
//    for(int kkk=0; kkk<nFilters1; kkk++)
//    {   if(mean[kkk]>1)
//        printf("Last mean = %f\n",mean[kkk]);}
//    for(int kkk=0; kkk<nFilters1; kkk++)
//    {   if(std[kkk]>1)
//        printf("Last std = %f\n",std[kkk]);}
//    for(int kkk=0; kkk<nFilters1; kkk++)
//    {   if(gamma[kkk]>1)
//        printf("Last gamma = %f\n",gamma[kkk]);}
//    for(int kkk=0; kkk<nFilters1; kkk++)
//    {   if(beta[kkk]>1)
//        printf("Last beta = %f\n",beta[kkk]);}
//
//    for(int kkk=0; kkk<nFilters2; kkk++)
//    {   if(bias2[kkk]>1)
//        printf("Last bias2 = %f\n",bias2[kkk]);}
//    for(int kkk=0; kkk<nFilters2*net->Z; kkk++)
//    {   if(weight2[kkk]>1)
//        printf("Last weight2 = %f\n",weight2[kkk]);}
//
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
    int64_t sum2;
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
	/*for(i = 0; i < input->X; i++){
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
            sum1 = 16 * input->Z;
            for(kk = 0; kk < input->Z; kk++){
                sum1 -= popcount(weight1[k*input->Z+kk]^input->b[(i*input->Y+j)*input->Z+kk]);
            }
            sum1 *= 2;
#endif
		// Store intermediary results
#ifdef  TestFix16
            inter[k] = (0 > (sum1*(int32_t)alpha1[k]+bias1[k]))? 0 : ((int32_t)sum1*(int32_t)alpha1[k] + bias1[k]);
            //printf("alpha1[%d]: %d, bias1[%d]:%d, sum1 = %d\n",k, alpha1[k], kk, bias1[k], sum1);
            inter[k] = (((((int32_t)(inter[k]-mean[k])*(int32_t)std[k])>>FIXSCALE)*(int32_t)gamma[k])>>FIXSCALE)+beta[k];
            //printf("mean[%d]: %d, inter[%d]:%d, sum1 = %d\n",k, mean[k], kk, inter[k], sum1);

#elif defined(TestFix32)
            inter[k] = (0 > (sum1*(int64_t)alpha1[k]+bias1[k]))? 0 : ((int64_t)sum1*(int64_t)alpha1[k] + bias1[k]);
            inter[k] = (((((int64_t)(inter[k]-mean[k])*(int64_t)std[k])>>FIXSCALE)*(int64_t)gamma[k])>>FIXSCALE)+beta[k];
            //printf("mean[%d]: %d, inter[%d]:%d, sum1 = %d\n",k, mean[k], kk, inter[k], sum1);
#else
            inter[k] = max(0.0, sum1*alpha1[k]+bias1[k]);
            inter[k] = (inter[k]-mean[k])*std[k]*gamma[k]+beta[k];
            //printf("mean[%d]: %f, inter[%d]:%f, sum1 = %d\n",k, mean[k], kk, inter[k], sum1);

#endif
            k++;
	    }while(k < nFilters1);

	    // Calculate the full-point layer
	    for(k = 0; k < nFilters2; k++){
            sum2 = 0;
            for(kk = 0; kk < nFilters1; kk++)
            {
                #ifdef TestFix16
                    sum2 += (int64_t)(weight2[k*nFilters1+kk]*(int64_t)inter[kk])>>FIXSCALE;// 2^((7+5) + 16 + 16 - 8 = 36)
                    //printf("weight2[%d]:%d, inter[%d]:%d, sum2 = %d\n",kk, weight2[k*nFilters1+kk], kk, inter[kk], sum2);
                #elif defined(TestFix32)
                    sum2 += (int64_t)(weight2[k*nFilters1+kk]*(int64_t)inter[kk])>>FIXSCALE;// 2^((7+5) + 32 + 32 - 20 = 56)
                #else
                    sum2 += weight2[k*nFilters1+kk]*inter[kk];
                #endif
            }
            #ifdef TestFix16
                output->f[(i*output->Y+j)*output->Z+k] = ((float)(sum2+bias2[k]))/(float)FIXVALUE;
                //printf("sum2 : %d, bias2[%d]:%d\n",sum2, k, bias2[k]);
            #elif defined(TestFix32)
                output->f[(i*output->Y+j)*output->Z+k] = ((float)(sum2+bias2[k]))/(float)FIXVALUE;
                //printf("sum2 : %d, bias2[%d]:%d\n",sum2, k, bias2[k]);
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

