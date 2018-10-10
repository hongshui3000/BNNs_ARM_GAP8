#include "BinConvolution.h"
//#define data_optimized

#ifdef originalxnor

void BinConvolution(Model *net, int32_t nFilters, 
		    int32_t kX, int32_t kY, int32_t dX, int32_t dY, 
		    int32_t padX, int32_t padY, int32_t bOutput, float eps){
    int32_t i, binZ;
    // Convolutional parameters
    float bias[nFilters];
    read(bias, net->fData, nFilters, net->fPos);
    for(i=0;i<nFilters;i++){
	    printf("bias[%d] = %f\n", i ,bias[i]);
	}
    float alpha[nFilters];
    read(alpha, net->fData, nFilters, net->fPos);

    binZ = net->Z/32;

    uint32_t *weight = new uint32_t[nFilters*kX*kY*binZ];
    for(i = 0; i < nFilters; i++){
	read(&weight[i*kX*kY*binZ], net->bData, kX, kY, binZ, net->bPos);
    }
    if(bOutput){	
	// Batch Normalization
	float mean[MAX_DEPTH];
	read(mean, net->fData, nFilters, net->fPos);

	float std[MAX_DEPTH];
	read(std, net->fData, nFilters, net->fPos);
	
	float gamma[MAX_DEPTH];
	read(gamma, net->fData, nFilters, net->fPos);
	
	float beta[MAX_DEPTH];
	read(beta, net->fData, nFilters, net->fPos);
	// Calculate binarization treshhold
	int8_t *signGamma = new int8_t[nFilters];
	int32_t *threshold2_int32 = new int32_t[nFilters];
	int32_t *threshold1_int32 = new int32_t[nFilters];
	int8_t *compare = new int8_t[nFilters];
	float threshold2_float, threshold1_float;
	for(i = 0; i < nFilters; i++){
	    signGamma[i] = sign(gamma[i]);
            threshold2_float =  (-beta[i]*sqrt(std[i]+eps)/(signGamma[i]*gamma[i])
                    +signGamma[i]*(mean[i]-bias[i]))/(alpha[i]);

	    threshold2_int32[i] = int32_t (threshold2_float);
	    // What to compare the input with in the ReLU layer instead of 0
  	    threshold1_float = -bias[i]/alpha[i];
	    threshold1_int32[i] = threshold1_float;

	    // The full-preision result of the comparison between the treshhold
	    // and the default output of the ReLU layer
	     compare[i] = (sign(gamma[i])*threshold1_float > threshold2_float) ? 1: 0;
	}
	
        BinCRBB_ *layer = new BinCRBB_(weight,  signGamma, threshold2_int32, threshold1_int32, compare,
                                       nFilters, kX, kY, dX, dY, padX, padY);
	net->addLayer(layer);

    } else {
	BinConvolution_ *layer = new BinConvolution_(weight, bias, alpha, nFilters, 
						     kX, kY, dX, dY, padX, padY);

	net->addLayer(layer);
    }

    net->Z = nFilters;

}
///////////////////////////////////////////////////////////////////////////////
#elif defined(my32minusPopcount)
///////////////////////////////////////////////////////////////////////////////
void BinConvolution(Model *net, int32_t nFilters, 
		    int32_t kX, int32_t kY, int32_t dX, int32_t dY, 
		    int32_t padX, int32_t padY, int32_t bOutput, float eps){
    int32_t i, binZ;
    // Convolutional parameters
    float bias[nFilters];
    read(bias, net->fData, nFilters, net->fPos);

    float alpha[nFilters];
    read(alpha, net->fData, nFilters, net->fPos);

    binZ = net->Z/32;

    uint32_t *weight = new uint32_t[nFilters*kX*kY*binZ];
    for(i = 0; i < nFilters; i++){
	read(&weight[i*kX*kY*binZ], net->bData, kX, kY, binZ, net->bPos);
    }
    if(bOutput){	
	// Batch Normalization
	float mean[MAX_DEPTH];
	read(mean, net->fData, nFilters, net->fPos);

	float std[MAX_DEPTH];
	read(std, net->fData, nFilters, net->fPos);
	
	float gamma[MAX_DEPTH];
	read(gamma, net->fData, nFilters, net->fPos);
	
	float beta[MAX_DEPTH];
	read(beta, net->fData, nFilters, net->fPos);
	// Calculate binarization treshhold
	int8_t *signGamma_positive = new int8_t[nFilters];
	int8_t *signGamma_negative = new int8_t[nFilters];
	int32_t *threshold2_int32 = new int32_t[nFilters];
	int32_t *threshold1_int32 = new int32_t[nFilters];
	int8_t *signBetaStar = new int8_t[nFilters];
	float threshold2_float, threshold1_float;
	float BetaStar;
	for(i = 0; i < nFilters; i++){
	    signGamma_positive[i] = (1+sign(gamma[i]))/2;
	    signGamma_negative[i] = (1-sign(gamma[i]))/2;
	   BetaStar = (beta[i]*sqrt(std[i]+eps)) - (mean[i]*gamma[i]);
	   threshold2_float = (-BetaStar - bias[i]*gamma[i])/(2*alpha[i]*sign(gamma[i])*gamma[i]);
	   threshold2_int32[i] = int32_t (threshold2_float);
	    // What to compare the input with in the ReLU layer instead of 0
  	    threshold1_float = -bias[i]/(2*alpha[i]);
	    threshold1_int32[i] = threshold1_float;

	    // The full-preision result of the comparison between the treshhold
	    // and the default output of the ReLU layer
	    signBetaStar[i] = (sign(BetaStar) > 0)? 1: 0;

	}
	
	//BinCRBB_ *layer = new BinCRBB_(weight, signGamma, treshhold, relu, compare, 
	//			       nFilters, kX, kY, dX, dY, padX, padY);
        BinCRBB_ *layer = new BinCRBB_(weight,  signGamma_positive, signGamma_negative, threshold2_int32, threshold1_int32, signBetaStar,
                                       nFilters, kX, kY, dX, dY, padX, padY);
	net->addLayer(layer);

    } else {
	BinConvolution_ *layer = new BinConvolution_(weight, bias, alpha, nFilters, 
						     kX, kY, dX, dY, padX, padY);

	net->addLayer(layer);
    }

    net->Z = nFilters;

}
///////////////////////////////////////////////////////////////////////////////
#elif defined(myPopcount)
///////////////////////////////////////////////////////////////////////////////
void BinConvolution(Model *net, int32_t nFilters, 
		    int32_t kX, int32_t kY, int32_t dX, int32_t dY, 
		    int32_t padX, int32_t padY, int32_t bOutput, float eps){
    int32_t i, binZ;
    // Convolutional parameters
    float bias[nFilters];
    read(bias, net->fData, nFilters, net->fPos);

    float alpha[nFilters];
    read(alpha, net->fData, nFilters, net->fPos);

    binZ = net->Z/32;

    uint32_t *weight = new uint32_t[nFilters*kX*kY*binZ];
    for(i = 0; i < nFilters; i++){
	read(&weight[i*kX*kY*binZ], net->bData, kX, kY, binZ, net->bPos);
    }
    if(bOutput){	
	// Batch Normalization
	float mean[MAX_DEPTH];
	read(mean, net->fData, nFilters, net->fPos);

	float std[MAX_DEPTH];
	read(std, net->fData, nFilters, net->fPos);
	
	float gamma[MAX_DEPTH];
	read(gamma, net->fData, nFilters, net->fPos);
	
	float beta[MAX_DEPTH];
	read(beta, net->fData, nFilters, net->fPos);

	// Calculate binarization treshhold
	int8_t *signGamma_positive = new int8_t[nFilters];
	int8_t *signGamma_negative = new int8_t[nFilters];
	int32_t *threshold2_int32 = new int32_t[nFilters];
	int32_t *threshold1_int32 = new int32_t[nFilters];
	int8_t *signBetaStar = new int8_t[nFilters];
	float threshold2_float, threshold1_float;
	float BetaStar;
	for(i = 0; i < nFilters; i++){
	    signGamma_positive[i] = (1+sign(gamma[i]))/2;
	    signGamma_negative[i] = (1-sign(gamma[i]))/2;
 	    threshold1_float = bias[i]/(2*alpha[i]) + 16*9*binZ;
	    threshold1_int32[i] = int32_t (threshold1_float);
	   BetaStar = (beta[i]*sqrt(std[i]+eps))- (gamma[i]*mean[i]);
           threshold2_float = (threshold1_float*gamma[i] + BetaStar/(2*alpha[i]))/(gamma[i]);
	   threshold2_int32[i] = int32_t (threshold2_float);

	    // The full-preision result of the comparison between the treshhold
	    // and the default output of the ReLU layer
	    signBetaStar[i] = (sign(BetaStar) > 0)? 1: 0;
	}

        BinCRBB_ *layer = new BinCRBB_(weight,  signGamma_positive, signGamma_negative, threshold2_int32, threshold1_int32, signBetaStar,
                                       nFilters, kX, kY, dX, dY, padX, padY);
	net->addLayer(layer);

    } else {
	BinConvolution_ *layer = new BinConvolution_(weight, bias, alpha, nFilters, 
						     kX, kY, dX, dY, padX, padY);

	net->addLayer(layer);
    }

    net->Z = nFilters;

}

///////////////////////////////////////////////////////////////////////////////
#elif defined(myPopcount_methode_2)
///////////////////////////////////////////////////////////////////////////////
void BinConvolution(Model *net, int32_t nFilters, 
		    int32_t kX, int32_t kY, int32_t dX, int32_t dY, 
		    int32_t padX, int32_t padY, int32_t bOutput, float eps){
    int32_t i, binZ;
    // Convolutional parameters
    binZ = net->Z/32;

    uint32_t *weight = new uint32_t[nFilters*kX*kY*binZ];
    for(i = 0; i < nFilters; i++){
	read(&weight[i*kX*kY*binZ], net->b32Data, kX, kY, binZ, net->b32Pos);
    }
    if(bOutput){
        uint8_t sign_posi[MAX_DEPTH];
        read(sign_posi, net->b8Data, nFilters, net->b8Pos);
	#ifdef with_sign_Gamma_negative
        uint8_t sign_nega[MAX_DEPTH];
        read(sign_nega, net->b8Data, nFilters, net->b8Pos);
	#endif
        uint32_t th1[MAX_DEPTH];
        read(th1, net->b32Data, nFilters, net->b32Pos);
        uint32_t th2[MAX_DEPTH];
	read(th2, net->b32Data, nFilters, net->b32Pos);
	uint8_t sign_BeSt[MAX_DEPTH];
	read(sign_BeSt, net->b8Data, nFilters, net->b8Pos);

	// Calculate binarization treshhold
	uint8_t *signGamma_positive = new uint8_t[nFilters];
#ifdef with_sign_Gamma_negative
	uint8_t *signGamma_negative = new uint8_t[nFilters];
#endif
	uint32_t *threshold2_int32 = new uint32_t[nFilters];
	uint32_t *threshold1_int32 = new uint32_t[nFilters];
	uint8_t *signBetaStar = new uint8_t[nFilters];

	for(i = 0; i < nFilters; i++){
		signGamma_positive[i] = sign_posi[i];
		#ifdef with_sign_Gamma_negative
		signGamma_negative[i] = sign_nega[i];
		#endif
		threshold1_int32[i] = th1[i];
		threshold2_int32[i] = th2[i];
		signBetaStar[i] = sign_BeSt[i];

	}
#ifdef with_sign_Gamma_negative
        BinCRBB_ *layer = new BinCRBB_(weight, signGamma_positive, signGamma_negative, threshold2_int32, threshold1_int32, signBetaStar,
                                       nFilters, kX, kY, dX, dY, padX, padY);
#else
        BinCRBB_ *layer = new BinCRBB_(weight, signGamma_positive, threshold2_int32, threshold1_int32, signBetaStar,
                                       nFilters, kX, kY, dX, dY, padX, padY);
#endif
	net->addLayer(layer);

    } 
	else {
//	BinConvolution_ *layer = new BinConvolution_(weight, bias, alpha, nFilters, 
//						     kX, kY, dX, dY, padX, padY);
//
//	net->addLayer(layer);
    }

    net->Z = nFilters;

}

///////////////////////////////////////////////////////////////////////////////
#elif defined(Popcount_sign_vectorized)
///////////////////////////////////////////////////////////////////////////////
void BinConvolution(Model *net, int32_t nFilters, 
		    int32_t kX, int32_t kY, int32_t dX, int32_t dY, 
		    int32_t padX, int32_t padY, int32_t bOutput, float eps){
    int32_t i, binZ,outZ;
    outZ = nFilters/32;
    // Convolutional parameters
    binZ = net->Z/32;
    uint32_t *weight = new uint32_t[nFilters*kX*kY*binZ];
    for(i = 0; i < nFilters; i++){
	read(&weight[i*kX*kY*binZ], net->b32Data, kX, kY, binZ, net->b32Pos);
    }
    if(bOutput){
        uint32_t sign_posi[MAX_DEPTH];
        read(sign_posi, net->bSignData, outZ, net->bSignPos);
	#ifdef with_sign_Gamma_negative
        uint32_t sign_nega[MAX_DEPTH];
        read(sign_nega, net->bSignData, outZ, net->bSignPos);
	#endif
        uint32_t th1[MAX_DEPTH];
        read(th1, net->b32Data, nFilters, net->b32Pos);
        uint32_t th2[MAX_DEPTH];
	read(th2, net->b32Data, nFilters, net->b32Pos);
	uint32_t sign_BeSt[MAX_DEPTH];
	read(sign_BeSt, net->bSignData, outZ, net->bSignPos);



	uint32_t *signGamma_positive = new uint32_t[outZ];
	#ifdef with_sign_Gamma_negative
	uint32_t *signGamma_negative = new uint32_t[outZ];
	#endif
	uint32_t *threshold2_int32 = new uint32_t[nFilters];
	uint32_t *threshold1_int32 = new uint32_t[nFilters];
	uint32_t *signBetaStar = new uint32_t[outZ];
	for(i = 0; i < outZ; i++){
		signGamma_positive[i] = sign_posi[i];
	#ifdef with_sign_Gamma_negative
		signGamma_negative[i] = sign_nega[i];
	#endif
		signBetaStar[i] = sign_BeSt[i];
	}
	for(i = 0; i < nFilters; i++){
		threshold1_int32[i] = th1[i];
		threshold2_int32[i] = th2[i];
	}
#ifdef with_sign_Gamma_negative
        BinCRBB_ *layer = new BinCRBB_(weight, signGamma_positive, signGamma_negative, threshold2_int32, threshold1_int32, signBetaStar,
                                       nFilters, kX, kY, dX, dY, padX, padY);
#else
        BinCRBB_ *layer = new BinCRBB_(weight, signGamma_positive, threshold2_int32, threshold1_int32, signBetaStar,
                                       nFilters, kX, kY, dX, dY, padX, padY);
#endif
	net->addLayer(layer);

    } 
	else {
//	BinConvolution_ *layer = new BinConvolution_(weight, bias, alpha, nFilters, 
//						     kX, kY, dX, dY, padX, padY);

//	net->addLayer(layer);
    }

    net->Z = nFilters;

}

#endif


BinConvolution_::BinConvolution_(uint32_t* weight_, float *bias_, float *alpha_,
				 int32_t nFilters_, int32_t kX_, int32_t kY_, 
				 int32_t dX_, int32_t dY_,
				 int32_t padX_, int32_t padY_){
    
    weight = weight_;
    bias = bias_;
    alpha = alpha_;
    nFilters = nFilters_;
    kX = kX_;
    kY = kY_;
    padX = padX_;
    padY = padY_;
    dX = dX_;
    dY = dY_;
    printf("BinC deleted");
}

BinConvolution_::~BinConvolution_(){
    
    delete[] bias;
    delete[] alpha;
    delete[] weight;
}

T* BinConvolution_::forward(T *input){

    int32_t i, j, k, ki, kj, kk, x0, y0, outi, outj;
    int32_t kX2, kY2, outX, outY;
    int32_t leftX, rightX, leftY, rightY, sum;
    uint32_t *filter;
    T *output;

    // Binarize if necessary
    if(input->type == T_FLOAT){
	input->binarize();
    }

    // Initialize output feature map
    outX = (input->X+2*padX-kX)/dX+1;
    outY = (input->Y+2*padY-kY)/dY+1;
    output = new T(T_FLOAT, outX, outY, nFilters);

    // Half size of the feature map
    kX2 = kX/2;
    kY2 = kY/2;

    // Starting position
    x0 = kX2-padX;
    y0 = kY2-padY;

    for(k = 0; k < nFilters; k++){
	filter = &(weight[k*kX*kY*input->Z]);

	// X position in the output
	outi = 0;
	for(i = x0; i < input->X-x0; i += dX){
	    // Range on X where to iterate with the filter
	    leftX = max(0, i-kX2);
	    rightX = min(input->X, i+kX2+1);

	    // Y position in the output
	    outj = 0;
	    for(j = y0; j < input->Y-y0; j += dY){
		// Range on Y where to iterate with the filter
		leftY = max(0, j-kY2);
		rightY = min(input->Y, j+kY2+1);
		
		// Calculate convolution
		sum = 0;
		for(ki = leftX; ki < rightX; ki++){
		    for(kj = leftY; kj < rightY; kj++){
			for(kk = 0; kk < input->Z; kk++){
			    sum += 32-2*popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^
						 input->b[(ki*input->Y+kj)*input->Z+kk]);
			}
		    }
		}

		// Output
		output->f[(outi*output->Y+outj)*output->Z+k] = sum*alpha[k] + bias[k];

		outj++;
	    }
	    
	    outi++;
	}
    }

    delete input;

    return output;
    
}
