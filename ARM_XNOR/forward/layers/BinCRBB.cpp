#include "BinCRBB.h"

////////////////////////////////////////////////////////////////////////////////////////////
#ifdef originalxnor
////////////////////////////////////////////////////////////////////////////////////////////
BinCRBB_::BinCRBB_(uint32_t *weight_, int8_t *signGamma_, int32_t *threshold2_int32_,
        int32_t *threshold1_int32_, int8_t *compare_, int32_t nFilters_,
        int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_,
        int32_t padX_, int32_t padY_){

    weight = weight_;
    signGamma = signGamma_;
    threshold2_int32 = threshold2_int32_;
    threshold1_int32 = threshold1_int32_;
    compare = compare_;
    nFilters = nFilters_;
    kX = kX_;
    kY = kY_;
    padX = padX_;
    padY = padY_;
    dX = dX_;
    dY = dY_;

}

BinCRBB_::~BinCRBB_(){
    delete[] weight;
    delete[] signGamma;
    delete[] threshold2_int32;
    delete[] threshold1_int32;
    delete[] compare;

}

T* BinCRBB_::forward(T* input){

    int32_t i, j, k, ki, kj, kk, x0, y0, outi, outj;
    int32_t kX2, kY2, outX, outY, cons, cons2;
    int32_t leftX, rightX, leftY, rightY, sum;
    int32_t shift, bink;
    uint32_t *filter;
    T *output;
    // Binarize input if necessary
    if(input->type == T_FLOAT){
        input->binarize();
    }

    // Initialize output feature map
    outX = (input->X+2*padX-kX)/dX+1;
    outY = (input->Y+2*padY-kY)/dY+1;
    output = new T(T_BINARY, outX, outY, nFilters);

    for(i = 0; i < outX; i++){
        for(j = 0; j < outY; j++){
            for(k = 0; k < output->Z; k++){
                output->b[(i*output->Y+j)*output->Z+k] = 0;
            }
        }
    }


    // Half size of the filters
    kX2 = kX/2;
    kY2 = kY/2;

    // Starting position
    x0 = kX2-padX;
    y0 = kY2-padY;

    // Position in the output integer
    shift = 0;
    for(k = 0; k < nFilters; k++){
        filter = &(weight[k*kX*kY*input->Z]);
        // Integer to output to
        bink = k / 32;

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
                // Calculate binary convolution
                sum = 0;
                for(ki = leftX; ki < rightX; ki++){
                    for(kj = leftY; kj < rightY; kj++){
                        for(kk = 0; kk < input->Z; kk++){
                            sum += 32-2*popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                    input->b[(ki*input->Y+kj)*input->Z+kk]);
                        }
                    }
                }      

                // ReLU, Batch Normalization and Binarization using only compares
                if(sum > threshold1_int32[k]){
                    if(signGamma[k]*sum > threshold2_int32[k]){
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= 1<<shift;
                    }
                } else {
                    if(compare[k]){         
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= 1<<shift;
                    }

                }

                outj++;
            }

            outi++;

        }

        shift = (shift+1) % 32;

    }
//	printf("\n");
    delete input;

    return output;

}
////////////////////////////////////////////////////////////////////////////////////////////
#elif defined(my32minusPopcount)
////////////////////////////////////////////////////////////////////////////////////////////
BinCRBB_::BinCRBB_(uint32_t *weight_, int8_t *signGamma_positive_,int8_t *signGamma_negative_, int32_t *threshold2_int32_,
        int32_t *threshold1_int32_, int8_t *signBetaStar_, int32_t nFilters_,
        int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_,
        int32_t padX_, int32_t padY_){

    weight = weight_;
    signGamma_positive = signGamma_positive_;
    signGamma_negative = signGamma_negative_;
    threshold2_int32 = threshold2_int32_;
    threshold1_int32 = threshold1_int32_;
    signBetaStar = signBetaStar_;
    nFilters = nFilters_;
    kX = kX_;
    kY = kY_;
    padX = padX_;
    padY = padY_;
    dX = dX_;
    dY = dY_;

}

BinCRBB_::~BinCRBB_(){
    delete[] weight;
    delete[] signGamma_positive;
    delete[] signGamma_negative;
    delete[] threshold2_int32;
    delete[] threshold1_int32;
    delete[] signBetaStar;

}

T* BinCRBB_::forward(T* input){

    int32_t i, j, k, ki, kj, kk, x0, y0, outi, outj;
    int32_t kX2, kY2, outX, outY, cons, cons2;
    int32_t leftX, rightX, leftY, rightY, sum;
    int32_t shift, bink;
    uint32_t *filter;
    T *output;
    // Binarize input if necessary
    if(input->type == T_FLOAT){
        input->binarize();
    }

    // Initialize output feature map
    outX = (input->X+2*padX-kX)/dX+1;
    outY = (input->Y+2*padY-kY)/dY+1;
    output = new T(T_BINARY, outX, outY, nFilters);

    for(i = 0; i < outX; i++){
        for(j = 0; j < outY; j++){
            for(k = 0; k < output->Z; k++){
                output->b[(i*output->Y+j)*output->Z+k] = 0;
            }
        }
    }


    // Half size of the filters
    kX2 = kX/2;
    kY2 = kY/2;

    // Starting position
    x0 = kX2-padX;
    y0 = kY2-padY;

    // Position in the output integer
    shift = 0;
    for(k = 0; k < nFilters; k++){
        filter = &(weight[k*kX*kY*input->Z]);
        // Integer to output to
        bink = k / 32;

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
                // Calculate binary convolution
                sum = 0;
                for(ki = leftX; ki < rightX; ki++){
                    for(kj = leftY; kj < rightY; kj++){
                        for(kk = 0; kk < input->Z; kk++){
                            sum += 16-popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                    input->b[(ki*input->Y+kj)*input->Z+kk]);
                        }
                    }
                }      

                // ReLU, Batch Normalization and Binarization using only compares
                if(sum > threshold1_int32[k]){
                    if(sum > threshold2_int32[k]){
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
			}
		    else{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
		
                    }
                } else {
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;
                }
                outj++;
            }
            outi++;
        }
        shift = (shift+1) % 32;
    }

    delete input;

    return output;

}
////////////////////////////////////////////////////////////////////////////////////////////
#elif defined(myPopcount)
////////////////////////////////////////////////////////////////////////////////////////////
BinCRBB_::BinCRBB_(uint32_t *weight_, 
	int8_t *signGamma_positive_, 
	int8_t *signGamma_negative_,
	int32_t *threshold2_int32_,
        int32_t *threshold1_int32_, 
	int8_t *signBetaStar_, 
	int32_t nFilters_,
        int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_,
        int32_t padX_, int32_t padY_){

    weight = weight_;
    signGamma_positive = signGamma_positive_;
    signGamma_negative = signGamma_negative_;
    threshold2_int32 = threshold2_int32_;
    threshold1_int32 = threshold1_int32_;
    signBetaStar = signBetaStar_;
    nFilters = nFilters_;
    kX = kX_;
    kY = kY_;
    padX = padX_;
    padY = padY_;
    dX = dX_;
    dY = dY_;

}

BinCRBB_::~BinCRBB_(){
    delete[] weight;
    delete[] signGamma_positive;
    delete[] signGamma_negative;
    delete[] threshold2_int32;
    delete[] threshold1_int32;
    delete[] signBetaStar;
}

T* BinCRBB_::forward(T* input){

    int32_t i, j, k, ki, kj, kk, x0, y0, outi, outj;
    int32_t kX2, kY2, outX, outY, cons, cons2;
    int32_t leftX, rightX, leftY, rightY, sum;

    int32_t shift, bink;
    uint32_t *filter;
    T *output;
    // Binarize input if necessary
    if(input->type == T_FLOAT){
        input->binarize();
    }

    // Initialize output feature map
    outX = (input->X+2*padX-kX)/dX+1;
    outY = (input->Y+2*padY-kY)/dY+1;
    output = new T(T_BINARY, outX, outY, nFilters);

    for(i = 0; i < outX; i++){
        for(j = 0; j < outY; j++){
            for(k = 0; k < output->Z; k++){
                output->b[(i*output->Y+j)*output->Z+k] = 0;
            }
        }
    }

    // Half size of the filters
    kX2 = kX/2;
    kY2 = kY/2;

    // Starting position
    x0 = kX2-padX;
    y0 = kY2-padY;

    // Position in the output integer
    shift = 0;
    for(k = 0; k < nFilters; k++){
        filter = &(weight[k*kX*kY*input->Z]);
        // Integer to output to
        bink = k / 32;

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
		int flag1, flag2, flag3, flag11, flag22, flag33, bbb, aaa;
                // Calculate binary convolution

                sum = 0;
                for(ki = leftX; ki < rightX; ki++){
                    for(kj = leftY; kj < rightY; kj++){
                        for(kk = 0; kk < input->Z; kk++){
                            sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                    input->b[(ki*input->Y+kj)*input->Z+kk]);
                        }
                    }
                }      
		sum = sum + (9 - (rightX - leftX)*(rightY - leftY))*16*(input->Z);

                // ReLU, Batch Normalization and Binarization using only compares
                if(sum <= threshold1_int32[k]){
                    if(sum <= threshold2_int32[k])
		    {
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
		    }
		    else
		    {
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
                    }
                }
		else
		{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;
                }
                outj++;
            }
            outi++;
        }
        shift = (shift+1) % 32;
    }

    delete input;

    return output;

}

////////////////////////////////////////////////////////////////////////////////////////////
#elif defined(myPopcount_methode_2)
////////////////////////////////////////////////////////////////////////////////////////////
BinCRBB_::BinCRBB_(uint32_t *weight_, 
	uint8_t *signGamma_positive_, 
#ifdef with_sign_Gamma_negative
	uint8_t *signGamma_negative_,
#endif
	uint32_t *threshold2_int32_,
        uint32_t *threshold1_int32_, 
	uint8_t *signBetaStar_, 
	int32_t nFilters_,
        int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_,
        int32_t padX_, int32_t padY_){

    weight = weight_;
    signGamma_positive = signGamma_positive_;
#ifdef with_sign_Gamma_negative
    signGamma_negative = signGamma_negative_;
#endif
    threshold2_int32 = threshold2_int32_;
    threshold1_int32 = threshold1_int32_;
    signBetaStar = signBetaStar_;
    nFilters = nFilters_;
    kX = kX_;
    kY = kY_;
    dX = dX_;
    dY = dY_;
    padX = padX_;
    padY = padY_;
}

BinCRBB_::~BinCRBB_(){
    delete[] weight;
    delete[] signGamma_positive;
#ifdef with_sign_Gamma_negative
    delete[] signGamma_negative;
#endif
    delete[] threshold2_int32;
    delete[] threshold1_int32;
    delete[] signBetaStar;
}

T* BinCRBB_::forward(T* input){

    int32_t i, j, k, ki, kj, kk, x0, y0, outi, outj;
    int32_t kX2, kY2, outX, outY, cons, cons2;
    int32_t leftX, rightX, leftY, rightY, sum, sum2;
    int32_t shift, bink;
    uint32_t *filter;
    T *output;

    // Binarize input if necessary
    if(input->type == T_FLOAT){
        input->binarize();
    }

    // Initialize output feature map
    outX = (input->X+2*padX-kX)/dX+1;
    outY = (input->Y+2*padY-kY)/dY+1;
    output = new T(T_BINARY, outX, outY, nFilters);

    for(i = 0; i < outX; i++){
        for(j = 0; j < outY; j++){
            for(k = 0; k < output->Z; k++){
                output->b[(i*output->Y+j)*output->Z+k] = 0;
            }
        }
    }
    // Half size of the filters
    kX2 = kX/2;
    kY2 = kY/2;

    // Starting position
    x0 = kX2-padX;
    y0 = kY2-padY;
    //printf("inputX = : %d\n", input->X);
    //printf("inputY = : %d\n", input->Y);
    // Position in the output integer
    shift = 0;
    //printf("X = %u, Y = %u, Z = %u \n", input->X, input->Y, input->Z);
    if((dX == 2) && (dY == 2))
    {
      if((input->X%2 == 0) && (input->Y%2 == 0))
      {
	for(k = 0; k < nFilters; k++)
	{
		filter = &(weight[k*kX*kY*input->Z]);
		// Integer to output to
		bink = k / 32;
//////////////////////////////corner, 2X2//////////////////////////////
		outi = 0;  // X position in the output
		outj = 0;  // Y position in the output
		sum = 0;
		// Calculate binary convolution
		for(ki = 0; ki < 2; ki++)
		{
			for(kj = 0; kj < 2; kj++)
			{
				for(kk = 0; kk < input->Z; kk++)
				{
					sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-y0)*input->Z+kk]^\
						input->b[(ki*input->Y+kj)*input->Z+kk]);
				}
			}
		}      
		sum += 80*input->Z;
		if(sum <= threshold1_int32[k])
		{
			if(sum <= threshold2_int32[k])
			{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
			}
			else
			{
#ifdef with_sign_Gamma_negative
                       		output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
			}
		}
		else
		{
                       	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;               	
		}//output(0,0) done

/////////////////////////////////////////////////////////////////////////
/////////////////////////////edge 2X3, 3X2///////////////////////////////
//-------------------edge outputY = 0------------------------//
		outi = 1;
		outj = 0;
		for(i = x0+dX; i < input->X-x0; i += dX){
			//leftX = max(0, i-kX2);
			leftX = i-kX2;
			//rightX = min(input->X, i+kX2+1);
			rightX = i+kX2+1;
			sum = 0;
		        // Calculate binary convolution
			for(ki = leftX; ki < rightX; ki++){
				for(kj = 0; kj < 2; kj++){
/*					for(kk = 0; kk < input->Z; kk++){
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-y0)*input->Z+kk]^\
                	         				       input->b[(ki*input->Y+kj)*input->Z+kk]);
                	       		}*/
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-y0)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                	   	}
			}      
			sum += 48*input->Z;
			if(sum <= threshold1_int32[k]){
				if(sum <= threshold2_int32[k])
				{
                	        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
				}
				else
				{
#ifdef with_sign_Gamma_negative
                       		output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
				}
			}
			else
			{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;              	
			}
                	outi++;
		}// output(dX,0) to output(outX,0) done
//-------------------edge outputX = 0------------------------//
            	outi = 0;
		outj = 1;
            	for(j = y0+dY; j < input->Y-y0; j += dY){
			//leftY = max(0, j-kY2);
			leftY = j-kY2;
			//rightY = min(input->Y, j+kY2+1);
			rightY = j+kY2+1;
			sum = 0;
		        // Calculate binary convolution
			for(ki = 0; ki < 2; ki++){
				for(kj = leftY; kj < rightY; kj++){
//					for(kk = 0; kk < input->Z; kk++){
//						sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-j)*input->Z+kk]^\
//                	         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
//                	       		}
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-j)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                		}
			}      
			sum += 48*input->Z;
			if(sum <= threshold1_int32[k]){
				if(sum <= threshold2_int32[k])
				{
                	        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
				}
				else
				{
#ifdef with_sign_Gamma_negative
                       		output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
				}
			}
			else
			{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;              	
			}
                	outj++;
		}// output(0,dY) to output(0,outY) done
////////////////////////////////////////////////////////////////////
/////////////////////////////3x3////////////////////////////////////
		sum = 0;
            	outi = 1;
		for(i = x0+dX; i < input->X-x0; i += dX){
		// Range on X where to iterate with the filter
			//leftX = max(0, i-kX2);
			leftX = i-kX2;
            		//rightX = min(input->X, i+kX2+1);
            		rightX = i+kX2+1;
            		outj = 1;
            		for(j = y0+dY; j < input->Y-y0; j += dY){
				// Range on Y where to iterate with the filter
            			//leftY = max(0, j-kY2);
            			leftY = j-kY2;
            		    	//rightY = min(input->Y, j+kY2+1);
            		    	rightY = j+kY2+1;
		            	// Calculate binary convolution
			    	sum = 0;
#ifndef UNFOLDED
				for(ki = leftX; ki < rightX; ki++){
					for(kj = leftY; kj < rightY; kj++){
						/*for(kk = 0; kk < input->Z; kk++){
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                		         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                		       		}*/
                	       			kk = 0;
						do{
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                        	                           input->b[(ki*input->Y+kj)*input->Z+kk]);
							kk++;
						}while(kk<input->Z);
                			}
				}    
#else
				switch(input->Z)
				{
					case 0x1:
					sum += popcount(filter[ 0]^input->b[  leftX   *input->Y+leftY         ])+
					       popcount(filter[ 1]^input->b[  leftX   *input->Y+leftY + 1     ])+
					       popcount(filter[ 2]^input->b[  leftX   *input->Y+leftY + 2     ])+
					       popcount(filter[ 3]^input->b[ (leftX+1)*input->Y+leftY         ])+
					       popcount(filter[ 4]^input->b[ (leftX+1)*input->Y+leftY + 1     ])+
					       popcount(filter[ 5]^input->b[ (leftX+1)*input->Y+leftY + 2     ])+
					       popcount(filter[ 6]^input->b[ (leftX+2)*input->Y+leftY         ])+
					       popcount(filter[ 7]^input->b[ (leftX+2)*input->Y+leftY + 1     ])+
					       popcount(filter[ 8]^input->b[ (leftX+2)*input->Y+leftY + 2     ]);
					break;
					
					case 0x2:
					sum += popcount(filter[ 0]^input->b[ (leftX   *input->Y+leftY    )*2    ])+
					       popcount(filter[ 1]^input->b[ (leftX   *input->Y+leftY    )*2 + 1])+
					       popcount(filter[ 2]^input->b[ (leftX   *input->Y+leftY + 1)*2    ])+
					       popcount(filter[ 3]^input->b[ (leftX   *input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[ 4]^input->b[ (leftX   *input->Y+leftY + 2)*2    ])+
					       popcount(filter[ 5]^input->b[ (leftX   *input->Y+leftY + 2)*2 + 1])+
					       popcount(filter[ 6]^input->b[((leftX+1)*input->Y+leftY    )*2    ])+
					       popcount(filter[ 7]^input->b[((leftX+1)*input->Y+leftY    )*2 + 1])+
					       popcount(filter[ 8]^input->b[((leftX+1)*input->Y+leftY + 1)*2    ])+
					       popcount(filter[ 9]^input->b[((leftX+1)*input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[10]^input->b[((leftX+1)*input->Y+leftY + 2)*2    ])+
					       popcount(filter[11]^input->b[((leftX+1)*input->Y+leftY + 2)*2 + 1])+
					       popcount(filter[12]^input->b[((leftX+2)*input->Y+leftY    )*2    ])+
					       popcount(filter[13]^input->b[((leftX+2)*input->Y+leftY    )*2 + 1])+
					       popcount(filter[14]^input->b[((leftX+2)*input->Y+leftY + 1)*2    ])+
					       popcount(filter[15]^input->b[((leftX+2)*input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[16]^input->b[((leftX+2)*input->Y+leftY + 2)*2    ])+
					       popcount(filter[17]^input->b[((leftX+2)*input->Y+leftY + 2)*2 + 1]);
					break;
				
					case 0x4:
					sum += popcount(filter[ 0]^input->b[ (leftX   *input->Y+leftY    )*4    ])+
					       popcount(filter[ 1]^input->b[ (leftX   *input->Y+leftY    )*4 + 1])+
					       popcount(filter[ 2]^input->b[ (leftX   *input->Y+leftY    )*4 + 2])+
					       popcount(filter[ 3]^input->b[ (leftX   *input->Y+leftY    )*4 + 3])+
					       popcount(filter[ 4]^input->b[ (leftX   *input->Y+leftY + 1)*4    ])+
					       popcount(filter[ 5]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[ 6]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[ 7]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[ 8]^input->b[ (leftX   *input->Y+leftY + 2)*4    ])+
					       popcount(filter[ 9]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[10]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[11]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 3])+
					       popcount(filter[12]^input->b[((leftX+1)*input->Y+leftY    )*4    ])+
					       popcount(filter[13]^input->b[((leftX+1)*input->Y+leftY    )*4 + 1])+
					       popcount(filter[14]^input->b[((leftX+1)*input->Y+leftY    )*4 + 2])+
					       popcount(filter[15]^input->b[((leftX+1)*input->Y+leftY    )*4 + 3])+
					       popcount(filter[16]^input->b[((leftX+1)*input->Y+leftY + 1)*4    ])+
					       popcount(filter[17]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[18]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[19]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[20]^input->b[((leftX+1)*input->Y+leftY + 2)*4    ])+
					       popcount(filter[21]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[22]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[23]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 3])+
					       popcount(filter[24]^input->b[((leftX+2)*input->Y+leftY    )*4    ])+
					       popcount(filter[25]^input->b[((leftX+2)*input->Y+leftY    )*4 + 1])+
					       popcount(filter[26]^input->b[((leftX+2)*input->Y+leftY    )*4 + 2])+
					       popcount(filter[27]^input->b[((leftX+2)*input->Y+leftY    )*4 + 3])+
					       popcount(filter[28]^input->b[((leftX+2)*input->Y+leftY + 1)*4    ])+
					       popcount(filter[29]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[30]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[31]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[32]^input->b[((leftX+2)*input->Y+leftY + 2)*4    ])+
					       popcount(filter[33]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[34]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[35]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 3]);
					break;
				}
#endif
				if(sum <= threshold1_int32[k]){
					if(sum <= threshold2_int32[k])
					{
                	        		output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
					}
					else
					{
#ifdef with_sign_Gamma_negative
                       				output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        			output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
					}
				}
				else
				{
                        		output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;               	
				}
                		outj++;
			}
			outi++;
		}
		shift = (shift+1) % 32;
	}
      }
      else if((input->X%2 == 0) && (input->Y%2 == 1))
      {
	for(k = 0; k < nFilters; k++)
	{
		filter = &(weight[k*kX*kY*input->Z]);
		// Integer to output to
		bink = k / 32;
//////////////////////////////corner, 2X2//////////////////////////////
		outi = 0;  // X position in the output
		outj = 0;  // Y position in the output
		sum = 0;
		// Calculate binary convolution
		for(ki = 0; ki < 2; ki++)
		{
			for(kj = 0; kj < 2; kj++)
			{
				for(kk = 0; kk < input->Z; kk++)
				{
					sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-y0)*input->Z+kk]^\
						input->b[(ki*input->Y+kj)*input->Z+kk]);
				}
			}
		}      
		sum += 80*input->Z;
		if(sum <= threshold1_int32[k])
		{
			if(sum <= threshold2_int32[k])
			{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
			}
			else
			{
#ifdef with_sign_Gamma_negative
                       		output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
			}
		}
		else
		{
                       	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;               	
		}//output(0,0) done

		outi = 0;  // X position in the output
		outj = outY-1;  // Y position in the output
		sum = 0;
		// Calculate binary convolution
		for(ki = 0; ki < 2; ki++)
		{
			for(kj = 0; kj < 2; kj++)
			{
				for(kk = 0; kk < input->Z; kk++)
				{
					sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-y0)*input->Z+kk]^\
						input->b[(ki*input->Y+kj)*input->Z+kk]);
				}
			}
		}      
		sum += 80*input->Z;
		if(sum <= threshold1_int32[k])
		{
			if(sum <= threshold2_int32[k])
			{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
			}
			else
			{
#ifdef with_sign_Gamma_negative
                       		output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
			}
		}
		else
		{
                       	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;               	
		}//output(0,outY) done

/////////////////////////////////////////////////////////////////////////
/////////////////////////////edge 2X3, 3X2///////////////////////////////
//-------------------edge outputY = 0------------------------//
		outi = 1;
		outj = 0;
		for(i = x0+dX; i < input->X-x0; i += dX){
			//leftX = max(0, i-kX2);
			leftX = i-kX2;
			//rightX = min(input->X, i+kX2+1);
			rightX = i+kX2+1;
			sum = 0;
		        // Calculate binary convolution
			for(ki = leftX; ki < rightX; ki++){
				for(kj = 0; kj < 2; kj++){
/*					for(kk = 0; kk < input->Z; kk++){
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-y0)*input->Z+kk]^\
                	         				       input->b[(ki*input->Y+kj)*input->Z+kk]);
                	       		}*/
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-y0)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                	   	}
			}      
			sum += 48*input->Z;
			if(sum <= threshold1_int32[k]){
				if(sum <= threshold2_int32[k])
				{
                	        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
				}
				else
				{
#ifdef with_sign_Gamma_negative
                       		output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
				}
			}
			else
			{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;              	
			}
                	outi++;
		}// output(dX,0) to output(outX,0) done
//-------------------edge outputX = 0------------------------//
            	outi = 0;
		outj = 1;
            	for(j = y0+dY; j < input->Y-y0-dY; j += dY){
			//leftY = max(0, j-kY2);
			leftY = j-kY2;
			//rightY = min(input->Y, j+kY2+1);
			rightY = j+kY2+1;
			sum = 0;
		        // Calculate binary convolution
			for(ki = 0; ki < 2; ki++){
				for(kj = leftY; kj < rightY; kj++){
//					for(kk = 0; kk < input->Z; kk++){
//						sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-j)*input->Z+kk]^\
//                	         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
//                	       		}
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-j)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                		}
			}      
			sum += 48*input->Z;
			if(sum <= threshold1_int32[k]){
				if(sum <= threshold2_int32[k])
				{
                	        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
				}
				else
				{
#ifdef with_sign_Gamma_negative
                       		output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
				}
			}
			else
			{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;              	
			}
                	outj++;
		}// output(0,dY) to output(0,outY) done
//-------------------edge outputY = outY------------------------//
       		outi = 1;
		outj = outY-1;
		j = input->Y-dY;
		leftY = max(0, j-kY2);
		rightY = min(input->Y, j+kY2+1);
            	for(i = x0+dX; i < input->X-x0; i += dX){
			//leftX = max(0, i-kX2);
			leftX = i-kX2;
//			rightX = min(input->X, i+kX2+1);
			rightX = i+kX2+1;
		    	sum = 0;
		        // Calculate binary convolution
			for(ki = leftX; ki < rightX; ki++){
				for(kj = leftY; kj < rightY; kj++){
					/*for(kk = 0; kk < input->Z; kk++){
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                	         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                	       		}*/
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                		}
			}      
			sum += 48*input->Z;
			if(sum <= threshold1_int32[k]){
				if(sum <= threshold2_int32[k])
				{
                	        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
				}
				else
				{
#ifdef with_sign_Gamma_negative
                       			output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        		output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
				}
			}
			else
			{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;              	
			}
                	outi++;
		}// output(outX,1) to output(outX,outY-1) done
////////////////////////////////////////////////////////////////////
/////////////////////////////3x3////////////////////////////////////
		sum = 0;
            	outi = 1;
		for(i = x0+dX; i < input->X-x0; i += dX){
		// Range on X where to iterate with the filter
			//leftX = max(0, i-kX2);
			leftX = i-kX2;
            		//rightX = min(input->X, i+kX2+1);
            		rightX = i+kX2+1;
            		outj = 1;
            		for(j = y0+dY; j < input->Y-y0-dY; j += dY){
				// Range on Y where to iterate with the filter
            			//leftY = max(0, j-kY2);
            			leftY = j-kY2;
            		    	//rightY = min(input->Y, j+kY2+1);
            		    	rightY = j+kY2+1;
		            	// Calculate binary convolution
			    	sum = 0;
#ifndef UNFOLDED
				for(ki = leftX; ki < rightX; ki++){
					for(kj = leftY; kj < rightY; kj++){
						/*for(kk = 0; kk < input->Z; kk++){
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                		         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                		       		}*/
                	       			kk = 0;
						do{
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                        	                           input->b[(ki*input->Y+kj)*input->Z+kk]);
							kk++;
						}while(kk<input->Z);
                			}
				}    
#else
				switch(input->Z)
				{
					case 0x1:
					sum += popcount(filter[ 0]^input->b[  leftX   *input->Y+leftY         ])+
					       popcount(filter[ 1]^input->b[  leftX   *input->Y+leftY + 1     ])+
					       popcount(filter[ 2]^input->b[  leftX   *input->Y+leftY + 2     ])+
					       popcount(filter[ 3]^input->b[ (leftX+1)*input->Y+leftY         ])+
					       popcount(filter[ 4]^input->b[ (leftX+1)*input->Y+leftY + 1     ])+
					       popcount(filter[ 5]^input->b[ (leftX+1)*input->Y+leftY + 2     ])+
					       popcount(filter[ 6]^input->b[ (leftX+2)*input->Y+leftY         ])+
					       popcount(filter[ 7]^input->b[ (leftX+2)*input->Y+leftY + 1     ])+
					       popcount(filter[ 8]^input->b[ (leftX+2)*input->Y+leftY + 2     ]);
					break;
					
					case 0x2:
					sum += popcount(filter[ 0]^input->b[ (leftX   *input->Y+leftY    )*2    ])+
					       popcount(filter[ 1]^input->b[ (leftX   *input->Y+leftY    )*2 + 1])+
					       popcount(filter[ 2]^input->b[ (leftX   *input->Y+leftY + 1)*2    ])+
					       popcount(filter[ 3]^input->b[ (leftX   *input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[ 4]^input->b[ (leftX   *input->Y+leftY + 2)*2    ])+
					       popcount(filter[ 5]^input->b[ (leftX   *input->Y+leftY + 2)*2 + 1])+
					       popcount(filter[ 6]^input->b[((leftX+1)*input->Y+leftY    )*2    ])+
					       popcount(filter[ 7]^input->b[((leftX+1)*input->Y+leftY    )*2 + 1])+
					       popcount(filter[ 8]^input->b[((leftX+1)*input->Y+leftY + 1)*2    ])+
					       popcount(filter[ 9]^input->b[((leftX+1)*input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[10]^input->b[((leftX+1)*input->Y+leftY + 2)*2    ])+
					       popcount(filter[11]^input->b[((leftX+1)*input->Y+leftY + 2)*2 + 1])+
					       popcount(filter[12]^input->b[((leftX+2)*input->Y+leftY    )*2    ])+
					       popcount(filter[13]^input->b[((leftX+2)*input->Y+leftY    )*2 + 1])+
					       popcount(filter[14]^input->b[((leftX+2)*input->Y+leftY + 1)*2    ])+
					       popcount(filter[15]^input->b[((leftX+2)*input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[16]^input->b[((leftX+2)*input->Y+leftY + 2)*2    ])+
					       popcount(filter[17]^input->b[((leftX+2)*input->Y+leftY + 2)*2 + 1]);
					break;
				
					case 0x4:
					sum += popcount(filter[ 0]^input->b[ (leftX   *input->Y+leftY    )*4    ])+
					       popcount(filter[ 1]^input->b[ (leftX   *input->Y+leftY    )*4 + 1])+
					       popcount(filter[ 2]^input->b[ (leftX   *input->Y+leftY    )*4 + 2])+
					       popcount(filter[ 3]^input->b[ (leftX   *input->Y+leftY    )*4 + 3])+
					       popcount(filter[ 4]^input->b[ (leftX   *input->Y+leftY + 1)*4    ])+
					       popcount(filter[ 5]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[ 6]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[ 7]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[ 8]^input->b[ (leftX   *input->Y+leftY + 2)*4    ])+
					       popcount(filter[ 9]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[10]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[11]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 3])+
					       popcount(filter[12]^input->b[((leftX+1)*input->Y+leftY    )*4    ])+
					       popcount(filter[13]^input->b[((leftX+1)*input->Y+leftY    )*4 + 1])+
					       popcount(filter[14]^input->b[((leftX+1)*input->Y+leftY    )*4 + 2])+
					       popcount(filter[15]^input->b[((leftX+1)*input->Y+leftY    )*4 + 3])+
					       popcount(filter[16]^input->b[((leftX+1)*input->Y+leftY + 1)*4    ])+
					       popcount(filter[17]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[18]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[19]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[20]^input->b[((leftX+1)*input->Y+leftY + 2)*4    ])+
					       popcount(filter[21]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[22]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[23]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 3])+
					       popcount(filter[24]^input->b[((leftX+2)*input->Y+leftY    )*4    ])+
					       popcount(filter[25]^input->b[((leftX+2)*input->Y+leftY    )*4 + 1])+
					       popcount(filter[26]^input->b[((leftX+2)*input->Y+leftY    )*4 + 2])+
					       popcount(filter[27]^input->b[((leftX+2)*input->Y+leftY    )*4 + 3])+
					       popcount(filter[28]^input->b[((leftX+2)*input->Y+leftY + 1)*4    ])+
					       popcount(filter[29]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[30]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[31]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[32]^input->b[((leftX+2)*input->Y+leftY + 2)*4    ])+
					       popcount(filter[33]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[34]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[35]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 3]);
					break;
				}
#endif
				if(sum <= threshold1_int32[k]){
					if(sum <= threshold2_int32[k])
					{
                	        		output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
					}
					else
					{
#ifdef with_sign_Gamma_negative
                       				output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        			output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
					}
				}
				else
				{
                        		output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;               	
				}
                		outj++;
			}
			outi++;
		}
		shift = (shift+1) % 32;
	}
      }
    }
    else
    {
	for(k = 0; k < nFilters; k++)
	{
		filter = &(weight[k*kX*kY*input->Z]);
		// Integer to output to
		bink = k / 32;
//////////////////////////////corner, 2X2 (four corners)//////////////////////////////
		outi = 0;
		for(i = x0; i < input->X-x0; i += input->X - x0 - dX){
		// Range on X where to iterate with the filter
			leftX = max(0, i-kX2);
            		rightX = min(input->X, i+kX2+1);
            		// Y position in the output
            		outj = 0;
            		for(j = y0; j < input->Y-y0; j += input->Y - y0 - dY){
				// Range on Y where to iterate with the filter
            			leftY = max(0, j-kY2);
            		    	rightY = min(input->Y, j+kY2+1);
		            	// Calculate binary convolution
			    	sum = 0;
				for(ki = leftX; ki < rightX; ki++){
					for(kj = leftY; kj < rightY; kj++){
						/*for(kk = 0; kk < input->Z; kk++){
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                		         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                		       		}*/
                	       			kk = 0;
						do{
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                                	                   input->b[(ki*input->Y+kj)*input->Z+kk]);
							kk++;
						}while(kk<input->Z);
                			}
				}     
				sum += 80*input->Z; 
				if(sum <= threshold1_int32[k]){
					if(sum <= threshold2_int32[k])
					{
                	        		output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
					}
					else
					{
#ifdef with_sign_Gamma_negative
                       				output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        			output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
					}
				}
				else
				{
                        		output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;              	
				}
                		outj = outY -1;
			}
			outi = outX-1;
		}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////edge 2X3, 3X2 (four edges)//////////////////////////////////////
//-------------------edge outputY = 0------------------------//
       		outi = 1;
		outj = 0;
		for(i = x0+dX; i < input->X-x0-dX; i += dX){
			//leftX = max(0, i-kX2);
			leftX = i-kX2;
			//rightX = min(input->X, i+kX2+1);
			rightX = i+kX2+1;
			sum = 0;
		        // Calculate binary convolution
			for(ki = leftX; ki < rightX; ki++){
				for(kj = 0; kj < 2; kj++){
					/*for(kk = 0; kk < input->Z; kk++)
					{
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-y0)*input->Z+kk]^\
                	         				       input->b[(ki*input->Y+kj)*input->Z+kk]);
                	       		}*/
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-y0)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                	   	}
			}      
			sum += 48*input->Z;
			if(sum <= threshold1_int32[k]){
				if(sum <= threshold2_int32[k])
				{
                	        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
				}
				else
				{
#ifdef with_sign_Gamma_negative
                       			output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        		output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
				}
			}
			else
			{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;              	
			}
                	outi++;
		}// output(1,0) to output(outi-1,0) done
//-------------------edge outputX = 0------------------------//
       		outi = 0;
		outj = 1;
            	for(j = y0+dY; j < input->Y-y0-dY; j += dY){
			//leftY = max(0, j-kY2);
			leftY = j-kY2;
			//rightY = min(input->Y, j+kY2+1);
			rightY = j+kY2+1;
		    	sum = 0;
		        // Calculate binary convolution
			for(ki = 0; ki < 2; ki++){
				for(kj = leftY; kj < rightY; kj++){
					/*for(kk = 0; kk < input->Z; kk++){
						sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-j)*input->Z+kk]^\
                	         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                	       		}*/
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-j)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                		}
			}      
			sum += 48*input->Z;
			if(sum <= threshold1_int32[k]){
				if(sum <= threshold2_int32[k])
				{
                	        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
				}
				else
				{
#ifdef with_sign_Gamma_negative
                       			output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        		output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
				}
			}
			else
			{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;              	
			}
                	outj++;
		}// output(0,dY) to output(0,outY-dY) done
//-------------------edge outputX = outX------------------------//
       		outi = outX-1;
		outj = 1;
		i = input->X-dX;
		leftX = max(0, i-kX2);
		rightX = min(input->X, i+kX2+1);
            	for(j = y0+dY; j < input->Y-y0-dY; j += dY){
			//leftY = max(0, j-kY2);
			leftY = j-kY2;
			//rightY = min(input->Y, j+kY2+1);
			rightY = j+kY2+1;
		    	sum = 0;
		        // Calculate binary convolution
			for(ki = leftX; ki < rightX; ki++){
				for(kj = leftY; kj < rightY; kj++){
					/*for(kk = 0; kk < input->Z; kk++){
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                	         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                	       		}*/
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                		}
			}      
			sum += 48*input->Z;
			if(sum <= threshold1_int32[k]){
				if(sum <= threshold2_int32[k])
				{
                	        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
				}
				else
				{
#ifdef with_sign_Gamma_negative
                       			output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        		output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
				}
			}
			else
			{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;              	
			}
                	outj++;
		}// output(outX,1) to output(outX,outY-1) done
//-------------------edge outputY = outY------------------------//
       		outi = 1;
		outj = outY-1;
		j = input->Y-dY;
		leftY = max(0, j-kY2);
		rightY = min(input->Y, j+kY2+1);
            	for(i = x0+dX; i < input->X-x0-dX; i += dX){
			//leftX = max(0, i-kX2);
			leftX = i-kX2;
//			rightX = min(input->X, i+kX2+1);
			rightX = i+kX2+1;
		    	sum = 0;
		        // Calculate binary convolution
			for(ki = leftX; ki < rightX; ki++){
				for(kj = leftY; kj < rightY; kj++){
					/*for(kk = 0; kk < input->Z; kk++){
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                	         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                	       		}*/
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                		}
			}      
			sum += 48*input->Z;
			if(sum <= threshold1_int32[k]){
				if(sum <= threshold2_int32[k])
				{
                	        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
				}
				else
				{
#ifdef with_sign_Gamma_negative
                       			output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        		output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
				}
			}
			else
			{
                        	output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;              	
			}
                	outi++;
		}// output(outX,1) to output(outX,outY-1) done
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////3x3 (one center)////////////////////////////////
            	outi = 1;
		for(i = x0+dX; i < input->X-x0-dX; i += dX){
		// Range on X where to iterate with the filter
//			leftX = max(0, i-kX2);
			leftX = i-kX2;
//            		rightX = min(input->X, i+kX2+1);
            		rightX = i+kX2+1;
            		// Y position in the output
            		outj = 1;
            		for(j = y0+dY; j < input->Y-y0-dY; j += dY){
				// Range on Y where to iterate with the filter
//            			leftY = max(0, j-kY2);
            			leftY = j-kY2;
//            		    	rightY = min(input->Y, j+kY2+1);
            		    	rightY = j+kY2+1;
		            	// Calculate binary convolution
			    	sum = 0;
#ifndef UNFOLDED
				for(ki = leftX; ki < rightX; ki++){
					for(kj = leftY; kj < rightY; kj++){
						/*for(kk = 0; kk < input->Z; kk++){
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                		         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                		       		}*/
                	       			kk = 0;
						do{
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                        	                           input->b[(ki*input->Y+kj)*input->Z+kk]);
							kk++;
						}while(kk<input->Z);
                			}
				}      
#else
				switch(input->Z)
				{
					case 0x1:
					sum += popcount(filter[ 0]^input->b[  leftX   *input->Y+leftY         ])+
					       popcount(filter[ 1]^input->b[  leftX   *input->Y+leftY + 1     ])+
					       popcount(filter[ 2]^input->b[  leftX   *input->Y+leftY + 2     ])+
					       popcount(filter[ 3]^input->b[ (leftX+1)*input->Y+leftY         ])+
					       popcount(filter[ 4]^input->b[ (leftX+1)*input->Y+leftY + 1     ])+
					       popcount(filter[ 5]^input->b[ (leftX+1)*input->Y+leftY + 2     ])+
					       popcount(filter[ 6]^input->b[ (leftX+2)*input->Y+leftY         ])+
					       popcount(filter[ 7]^input->b[ (leftX+2)*input->Y+leftY + 1     ])+
					       popcount(filter[ 8]^input->b[ (leftX+2)*input->Y+leftY + 2     ]);
					break;

					case 0x2: 
					sum += popcount(filter[ 0]^input->b[ (leftX   *input->Y+leftY    )*2    ])+
					       popcount(filter[ 1]^input->b[ (leftX   *input->Y+leftY    )*2 + 1])+
					       popcount(filter[ 2]^input->b[ (leftX   *input->Y+leftY + 1)*2    ])+
					       popcount(filter[ 3]^input->b[ (leftX   *input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[ 4]^input->b[ (leftX   *input->Y+leftY + 2)*2    ])+
					       popcount(filter[ 5]^input->b[ (leftX   *input->Y+leftY + 2)*2 + 1])+
					       popcount(filter[ 6]^input->b[((leftX+1)*input->Y+leftY    )*2    ])+
					       popcount(filter[ 7]^input->b[((leftX+1)*input->Y+leftY    )*2 + 1])+
					       popcount(filter[ 8]^input->b[((leftX+1)*input->Y+leftY + 1)*2    ])+
					       popcount(filter[ 9]^input->b[((leftX+1)*input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[10]^input->b[((leftX+1)*input->Y+leftY + 2)*2    ])+
					       popcount(filter[11]^input->b[((leftX+1)*input->Y+leftY + 2)*2 + 1])+
					       popcount(filter[12]^input->b[((leftX+2)*input->Y+leftY    )*2    ])+
					       popcount(filter[13]^input->b[((leftX+2)*input->Y+leftY    )*2 + 1])+
					       popcount(filter[14]^input->b[((leftX+2)*input->Y+leftY + 1)*2    ])+
					       popcount(filter[15]^input->b[((leftX+2)*input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[16]^input->b[((leftX+2)*input->Y+leftY + 2)*2    ])+
					       popcount(filter[17]^input->b[((leftX+2)*input->Y+leftY + 2)*2 + 1]);
					break;

					case 0x4: 
					sum += popcount(filter[ 0]^input->b[ (leftX   *input->Y+leftY    )*4    ])+
					       popcount(filter[ 1]^input->b[ (leftX   *input->Y+leftY    )*4 + 1])+
					       popcount(filter[ 2]^input->b[ (leftX   *input->Y+leftY    )*4 + 2])+
					       popcount(filter[ 3]^input->b[ (leftX   *input->Y+leftY    )*4 + 3])+
					       popcount(filter[ 4]^input->b[ (leftX   *input->Y+leftY + 1)*4    ])+
					       popcount(filter[ 5]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[ 6]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[ 7]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[ 8]^input->b[ (leftX   *input->Y+leftY + 2)*4    ])+
					       popcount(filter[ 9]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[10]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[11]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 3])+
					       popcount(filter[12]^input->b[((leftX+1)*input->Y+leftY    )*4    ])+
					       popcount(filter[13]^input->b[((leftX+1)*input->Y+leftY    )*4 + 1])+
					       popcount(filter[14]^input->b[((leftX+1)*input->Y+leftY    )*4 + 2])+
					       popcount(filter[15]^input->b[((leftX+1)*input->Y+leftY    )*4 + 3])+
					       popcount(filter[16]^input->b[((leftX+1)*input->Y+leftY + 1)*4    ])+
					       popcount(filter[17]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[18]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[19]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[20]^input->b[((leftX+1)*input->Y+leftY + 2)*4    ])+
					       popcount(filter[21]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[22]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[23]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 3])+
					       popcount(filter[24]^input->b[((leftX+2)*input->Y+leftY    )*4    ])+
					       popcount(filter[25]^input->b[((leftX+2)*input->Y+leftY    )*4 + 1])+
					       popcount(filter[26]^input->b[((leftX+2)*input->Y+leftY    )*4 + 2])+
					       popcount(filter[27]^input->b[((leftX+2)*input->Y+leftY    )*4 + 3])+
					       popcount(filter[28]^input->b[((leftX+2)*input->Y+leftY + 1)*4    ])+
					       popcount(filter[29]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[30]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[31]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[32]^input->b[((leftX+2)*input->Y+leftY + 2)*4    ])+
					       popcount(filter[33]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[34]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[35]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 3]);
					break;
				}
#endif
				if(sum <= threshold1_int32[k]){
					if(sum <= threshold2_int32[k])
					{
                	        		output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_positive[k]<<shift;
					}
					else
					{
#ifdef with_sign_Gamma_negative
                       				output->b[(outi*output->Y+outj)*output->Z+bink] |= signGamma_negative[k]<<shift; 
#else
                        		output->b[(outi*output->Y+outj)*output->Z+bink] |= ((1-signGamma_positive[k])>>1)<<shift;
#endif
					}
				}
				else
				{
                        		output->b[(outi*output->Y+outj)*output->Z+bink] |= signBetaStar[k]<<shift;              	
				}
                		outj++;
			}
			outi++;
		}
		shift = (shift+1) % 32;
	}
    }
    delete input;
    return output;

}

////////////////////////////////////////////////////////////////////////////////////////////
#elif defined(Popcount_sign_vectorized)
////////////////////////////////////////////////////////////////////////////////////////////
BinCRBB_::BinCRBB_(uint32_t *weight_, 
	uint32_t *signGamma_positive_, 
#ifdef with_sign_Gamma_negative
	uint32_t *signGamma_negative_,
#endif
	uint32_t *threshold2_int32_,
        uint32_t *threshold1_int32_, 
	uint32_t *signBetaStar_, 
	int32_t nFilters_,
        int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_,
        int32_t padX_, int32_t padY_){

    weight = weight_;
    signGamma_positive = signGamma_positive_;
#ifdef with_sign_Gamma_negative
    signGamma_negative = signGamma_negative_;
#endif
    threshold2_int32 = threshold2_int32_;
    threshold1_int32 = threshold1_int32_;
    signBetaStar = signBetaStar_;
    nFilters = nFilters_;
    kX = kX_;
    kY = kY_;
    padX = padX_;
    padY = padY_;
    dX = dX_;
    dY = dY_;

}

BinCRBB_::~BinCRBB_(){
    delete[] weight;
    delete[] signGamma_positive;
#ifdef with_sign_Gamma_negative
    delete[] signGamma_negative;
#endif
    delete[] threshold2_int32;
    delete[] threshold1_int32;
    delete[] signBetaStar;
}

T* BinCRBB_::forward(T* input){

    int32_t i, j, k, ki, kj, kk, x0, y0, outi, outj,outk;
    int32_t kX2, kY2, outX, outY, cons, cons2, outZ;
    int32_t leftX, rightX, leftY, rightY, sum;

    int32_t shift, bink;
    uint32_t *filter;
    T *output;
    // Binarize input if necessary
    if(input->type == T_FLOAT){
        input->binarize();
    }

    // Initialize output feature map
    outX = (input->X+2*padX-kX)/dX+1;
    outY = (input->Y+2*padY-kY)/dY+1;
    output = new T(T_BINARY, outX, outY, nFilters);
    outZ = nFilters/32;
	uint32_t *comp1 = new uint32_t[outX*outY*outZ];
	uint32_t *comp2 = new uint32_t[outX*outY*outZ];
    for(i = 0; i < outX; i++){
        for(j = 0; j < outY; j++){
            for(k = 0; k < output->Z; k++){
                output->b[(i*output->Y+j)*output->Z+k] = 0;
		comp1[(i*outY+j)*outZ+k] = 0;
		comp2[(i*outY+j)*outZ+k] = 0;
            }
        }
    }

    // Half size of the filters
    kX2 = kX/2;
    kY2 = kY/2;

    // Starting position
    x0 = kX2-padX;
    y0 = kY2-padY;
    // Position in the output integer
    shift = 0;
    if((dX == 2) && (dY == 2))
    {
	for(k = 0; k < nFilters; k++)
	{
		filter = &(weight[k*kX*kY*input->Z]);
		// Integer to output to
		bink = k / 32;
//////////////////////////////corner, 2X2//////////////////////////////
		outi = 0;  // X position in the output
		outj = 0;  // Y position in the output
		sum = 0;
		// Calculate binary convolution
		for(ki = 0; ki < 2; ki++)
		{
			for(kj = 0; kj < 2; kj++)
			{
				/*for(kk = 0; kk < input->Z; kk++)
				{
					sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-y0)*input->Z+kk]^\
						input->b[(ki*input->Y+kj)*input->Z+kk]);
				}*/
                	       	kk = 0;
				do{
					sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-y0)*input->Z+kk]^\
                                                input->b[(ki*input->Y+kj)*input->Z+kk]);
					kk++;
				}while(kk<input->Z);
			}
		}      
		sum += 80*input->Z;
		comp1[(outi*outY+outj)*outZ+bink] |= (sum <= threshold1_int32[k])<<shift;
		comp2[(outi*outY+outj)*outZ+bink] |= (sum <= threshold2_int32[k])<<shift;

/////////////////////////////////////////////////////////////////////////
/////////////////////////////edge 2X3, 3X2///////////////////////////////
//-------------------edge outputY = 0------------------------//
		outi = 1;
		outj = 0;
		for(i = x0+dX; i < input->X-x0; i += dX){
			//leftX = max(0, i-kX2);
			leftX = i-kX2;
			//rightX = min(input->X, i+kX2+1);
			rightX = i+kX2+1;
			sum = 0;
		        // Calculate binary convolution
			for(ki = leftX; ki < rightX; ki++){
				for(kj = 0; kj < 2; kj++){
					/*for(kk = 0; kk < input->Z; kk++){
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-y0)*input->Z+kk]^\
                	         				       input->b[(ki*input->Y+kj)*input->Z+kk]);
                	       		}*/
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-y0)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                	   	}
			}      
			sum += 48*input->Z;
			comp1[(outi*outY+outj)*outZ+bink] |= (sum <= threshold1_int32[k])<<shift;
			comp2[(outi*outY+outj)*outZ+bink] |= (sum <= threshold2_int32[k])<<shift;
			outi++;
		}

//-------------------edge outputX = 0------------------------//
            	outi = 0;
		outj = 1;
            	for(j = y0+dY; j < input->Y-y0; j += dY){
			//leftY = max(0, j-kY2);
			leftY = j-kY2;
			//rightY = min(input->Y, j+kY2+1);
			rightY = j+kY2+1;
			sum = 0;
		        // Calculate binary convolution
			for(ki = 0; ki < 2; ki++){
				for(kj = leftY; kj < rightY; kj++){
					/*for(kk = 0; kk < input->Z; kk++){
						sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-j)*input->Z+kk]^\
                	         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                	       		}*/
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-j)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                		}
			}      
			sum += 48*input->Z;
			comp1[(outi*outY+outj)*outZ+bink] |= (sum <= threshold1_int32[k])<<shift;
			comp2[(outi*outY+outj)*outZ+bink] |= (sum <= threshold2_int32[k])<<shift;
                	outj++;
		}// output(0,dY) to output(0,outY) done

////////////////////////////////////////////////////////////////////
/////////////////////////////3x3////////////////////////////////////
		sum = 0;
            	outi = 1;
		for(i = x0+dX; i < input->X-x0; i += dX){
		// Range on X where to iterate with the filter
			//leftX = max(0, i-kX2);
			leftX = i-kX2;
            		//rightX = min(input->X, i+kX2+1);
            		rightX = i+kX2+1;
            		outj = 1;
            		for(j = y0+dY; j < input->Y-y0; j += dY){
				// Range on Y where to iterate with the filter
            			//leftY = max(0, j-kY2);
            			leftY = j-kY2;
            		    	//rightY = min(input->Y, j+kY2+1);
            		    	rightY = j+kY2+1;
		            	// Calculate binary convolution
			    	sum = 0;
#ifndef UNFOLDED
				for(ki = leftX; ki < rightX; ki++){
					for(kj = leftY; kj < rightY; kj++){
						/*for(kk = 0; kk < input->Z; kk++){
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                		         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                		       		}*/
                	       			kk = 0;
						do{
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                        	                           input->b[(ki*input->Y+kj)*input->Z+kk]);
							kk++;
						}while(kk<input->Z);
                			}
				}    
#else
				switch(input->Z)
				{
					case 0x1:
					sum += popcount(filter[ 0]^input->b[  leftX   *input->Y+leftY         ])+
					       popcount(filter[ 1]^input->b[  leftX   *input->Y+leftY + 1     ])+
					       popcount(filter[ 2]^input->b[  leftX   *input->Y+leftY + 2     ])+
					       popcount(filter[ 3]^input->b[ (leftX+1)*input->Y+leftY         ])+
					       popcount(filter[ 4]^input->b[ (leftX+1)*input->Y+leftY + 1     ])+
					       popcount(filter[ 5]^input->b[ (leftX+1)*input->Y+leftY + 2     ])+
					       popcount(filter[ 6]^input->b[ (leftX+2)*input->Y+leftY         ])+
					       popcount(filter[ 7]^input->b[ (leftX+2)*input->Y+leftY + 1     ])+
					       popcount(filter[ 8]^input->b[ (leftX+2)*input->Y+leftY + 2     ]);
					break;
					
					case 0x2:
					sum += popcount(filter[ 0]^input->b[ (leftX   *input->Y+leftY    )*2    ])+
					       popcount(filter[ 1]^input->b[ (leftX   *input->Y+leftY    )*2 + 1])+
					       popcount(filter[ 2]^input->b[ (leftX   *input->Y+leftY + 1)*2    ])+
					       popcount(filter[ 3]^input->b[ (leftX   *input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[ 4]^input->b[ (leftX   *input->Y+leftY + 2)*2    ])+
					       popcount(filter[ 5]^input->b[ (leftX   *input->Y+leftY + 2)*2 + 1])+
					       popcount(filter[ 6]^input->b[((leftX+1)*input->Y+leftY    )*2    ])+
					       popcount(filter[ 7]^input->b[((leftX+1)*input->Y+leftY    )*2 + 1])+
					       popcount(filter[ 8]^input->b[((leftX+1)*input->Y+leftY + 1)*2    ])+
					       popcount(filter[ 9]^input->b[((leftX+1)*input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[10]^input->b[((leftX+1)*input->Y+leftY + 2)*2    ])+
					       popcount(filter[11]^input->b[((leftX+1)*input->Y+leftY + 2)*2 + 1])+
					       popcount(filter[12]^input->b[((leftX+2)*input->Y+leftY    )*2    ])+
					       popcount(filter[13]^input->b[((leftX+2)*input->Y+leftY    )*2 + 1])+
					       popcount(filter[14]^input->b[((leftX+2)*input->Y+leftY + 1)*2    ])+
					       popcount(filter[15]^input->b[((leftX+2)*input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[16]^input->b[((leftX+2)*input->Y+leftY + 2)*2    ])+
					       popcount(filter[17]^input->b[((leftX+2)*input->Y+leftY + 2)*2 + 1]);
					break;
				
					case 0x4:
					sum += popcount(filter[ 0]^input->b[ (leftX   *input->Y+leftY    )*4    ])+
					       popcount(filter[ 1]^input->b[ (leftX   *input->Y+leftY    )*4 + 1])+
					       popcount(filter[ 2]^input->b[ (leftX   *input->Y+leftY    )*4 + 2])+
					       popcount(filter[ 3]^input->b[ (leftX   *input->Y+leftY    )*4 + 3])+
					       popcount(filter[ 4]^input->b[ (leftX   *input->Y+leftY + 1)*4    ])+
					       popcount(filter[ 5]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[ 6]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[ 7]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[ 8]^input->b[ (leftX   *input->Y+leftY + 2)*4    ])+
					       popcount(filter[ 9]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[10]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[11]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 3])+
					       popcount(filter[12]^input->b[((leftX+1)*input->Y+leftY    )*4    ])+
					       popcount(filter[13]^input->b[((leftX+1)*input->Y+leftY    )*4 + 1])+
					       popcount(filter[14]^input->b[((leftX+1)*input->Y+leftY    )*4 + 2])+
					       popcount(filter[15]^input->b[((leftX+1)*input->Y+leftY    )*4 + 3])+
					       popcount(filter[16]^input->b[((leftX+1)*input->Y+leftY + 1)*4    ])+
					       popcount(filter[17]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[18]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[19]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[20]^input->b[((leftX+1)*input->Y+leftY + 2)*4    ])+
					       popcount(filter[21]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[22]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[23]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 3])+
					       popcount(filter[24]^input->b[((leftX+2)*input->Y+leftY    )*4    ])+
					       popcount(filter[25]^input->b[((leftX+2)*input->Y+leftY    )*4 + 1])+
					       popcount(filter[26]^input->b[((leftX+2)*input->Y+leftY    )*4 + 2])+
					       popcount(filter[27]^input->b[((leftX+2)*input->Y+leftY    )*4 + 3])+
					       popcount(filter[28]^input->b[((leftX+2)*input->Y+leftY + 1)*4    ])+
					       popcount(filter[29]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[30]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[31]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[32]^input->b[((leftX+2)*input->Y+leftY + 2)*4    ])+
					       popcount(filter[33]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[34]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[35]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 3]);
					break;
				}
#endif
				comp1[(outi*outY+outj)*outZ+bink] |= (sum <= threshold1_int32[k])<<shift;
				comp2[(outi*outY+outj)*outZ+bink] |= (sum <= threshold2_int32[k])<<shift;
                		outj++;
			}
			outi++;
		}
		shift = (shift+1) % 32;
	}
    }
    else
    {
	for(k = 0; k < nFilters; k++)
	{
		filter = &(weight[k*kX*kY*input->Z]);
		// Integer to output to
		bink = k / 32;
//////////////////////////////corner, 2X2 (four corners)//////////////////////////////
		outi = 0;
		for(i = x0; i < input->X-x0; i += input->X - x0 - dX){
		// Range on X where to iterate with the filter
			leftX = max(0, i-kX2);
            		rightX = min(input->X, i+kX2+1);
            		// Y position in the output
            		outj = 0;
            		for(j = y0; j < input->Y-y0; j += input->Y - y0 - dY){
				// Range on Y where to iterate with the filter
            			leftY = max(0, j-kY2);
            		    	rightY = min(input->Y, j+kY2+1);
		            	// Calculate binary convolution
			    	sum = 0;
				for(ki = leftX; ki < rightX; ki++){
					for(kj = leftY; kj < rightY; kj++){
						/*for(kk = 0; kk < input->Z; kk++){
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                		         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                		       		}*/
                	       			kk = 0;
						do{
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                        	                           input->b[(ki*input->Y+kj)*input->Z+kk]);
							kk++;
						}while(kk<input->Z);
                			}
				}     
				sum += 80*input->Z; 

				comp1[(outi*outY+outj)*outZ+bink] |= (sum <= threshold1_int32[k])<<shift;
				comp2[(outi*outY+outj)*outZ+bink] |= (sum <= threshold2_int32[k])<<shift;
                		outj = outY -1;
			}
			outi = outX-1;
		}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////edge 2X3, 3X2 (four edges)//////////////////////////////////////
//-------------------edge outputY = 0------------------------//
       		outi = 1;
		outj = 0;
		for(i = x0+dX; i < input->X-x0-dX; i += dX){
			//leftX = max(0, i-kX2);
			leftX = i-kX2;
			//rightX = min(input->X, i+kX2+1);
			rightX = i+kX2+1;
			sum = 0;
		        // Calculate binary convolution
			for(ki = leftX; ki < rightX; ki++){
				for(kj = 0; kj < 2; kj++){
					/*for(kk = 0; kk < input->Z; kk++)
					{
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-y0)*input->Z+kk]^\
                	         				       input->b[(ki*input->Y+kj)*input->Z+kk]);
                	       		}*/
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-y0)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                	   	}
			}      
			sum += 48*input->Z;
			comp1[(outi*outY+outj)*outZ+bink] |= (sum <= threshold1_int32[k])<<shift;
			comp2[(outi*outY+outj)*outZ+bink] |= (sum <= threshold2_int32[k])<<shift;
                	outi++;
		}// output(1,0) to output(outi-1,0) done
//-------------------edge outputX = 0------------------------//
       		outi = 0;
		outj = 1;
            	for(j = y0+dY; j < input->Y-y0-dY; j += dY){
			//leftY = max(0, j-kY2);
			leftY = j-kY2;
			//rightY = min(input->Y, j+kY2+1);
			rightY = j+kY2+1;
		    	sum = 0;
		        // Calculate binary convolution
			for(ki = 0; ki < 2; ki++){
				for(kj = leftY; kj < rightY; kj++){
					/*for(kk = 0; kk < input->Z; kk++){
						sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-j)*input->Z+kk]^\
                	         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                	       		}*/
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-x0)*kY+kY2+kj-j)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                		}
			}      
			sum += 48*input->Z;
			comp1[(outi*outY+outj)*outZ+bink] |= (sum <= threshold1_int32[k])<<shift;
			comp2[(outi*outY+outj)*outZ+bink] |= (sum <= threshold2_int32[k])<<shift;
                	outj++;
		}// output(0,dY) to output(0,outY-dY) done
//-------------------edge outputX = outX------------------------//
       		outi = outX-1;
		outj = 1;
		i = input->X-dX;
		leftX = max(0, i-kX2);
		rightX = min(input->X, i+kX2+1);
            	for(j = y0+dY; j < input->Y-y0-dY; j += dY){
			//leftY = max(0, j-kY2);
			leftY = j-kY2;
			//rightY = min(input->Y, j+kY2+1);
			rightY = j+kY2+1;
		    	sum = 0;
		        // Calculate binary convolution
			for(ki = leftX; ki < rightX; ki++){
				for(kj = leftY; kj < rightY; kj++){
					/*for(kk = 0; kk < input->Z; kk++){
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                	         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                	       		}*/
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                		}
			}      
			sum += 48*input->Z;

			comp1[(outi*outY+outj)*outZ+bink] |= (sum <= threshold1_int32[k])<<shift;
			comp2[(outi*outY+outj)*outZ+bink] |= (sum <= threshold2_int32[k])<<shift;
                	outj++;
		}// output(outX,1) to output(outX,outY-dY) done
//-------------------edge outputY = outY------------------------//
       		outi = 1;
		outj = outY-1;
		j = input->Y-dY;
		leftY = max(0, j-kY2);
		rightY = min(input->Y, j+kY2+1);
            	for(i = x0+dX; i < input->X-x0-dX; i += dX){
			//leftX = max(0, i-kX2);
			leftX = i-kX2;
//			rightX = min(input->X, i+kX2+1);
			rightX = i+kX2+1;
		    	sum = 0;
		        // Calculate binary convolution
			for(ki = leftX; ki < rightX; ki++){
				for(kj = leftY; kj < rightY; kj++){
					/*for(kk = 0; kk < input->Z; kk++){
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                	         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                	       		}*/
                	       		kk = 0;
					do{
						sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                                                   input->b[(ki*input->Y+kj)*input->Z+kk]);
						kk++;
					}while(kk<input->Z);
                		}
			}      
			sum += 48*input->Z;
			comp1[(outi*outY+outj)*outZ+bink] |= (sum <= threshold1_int32[k])<<shift;
			comp2[(outi*outY+outj)*outZ+bink] |= (sum <= threshold2_int32[k])<<shift;
                	outi++;
		}// output(outX,1) to output(outX,outY-1) done
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////3x3 (one center)////////////////////////////////
            	outi = 1;
		for(i = x0+dX; i < input->X-x0-dX; i += dX){
		// Range on X where to iterate with the filter
//			leftX = max(0, i-kX2);
			leftX = i-kX2;
//            		rightX = min(input->X, i+kX2+1);
            		rightX = i+kX2+1;
            		// Y position in the output
            		outj = 1;
            		for(j = y0+dY; j < input->Y-y0-dY; j += dY){
				// Range on Y where to iterate with the filter
//            			leftY = max(0, j-kY2);
            			leftY = j-kY2;
//            		    	rightY = min(input->Y, j+kY2+1);
            		    	rightY = j+kY2+1;
		            	// Calculate binary convolution
			    	sum = 0;
#ifndef UNFOLDED
				for(ki = leftX; ki < rightX; ki++){
					for(kj = leftY; kj < rightY; kj++){
						/*for(kk = 0; kk < input->Z; kk++){
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                		         				     input->b[(ki*input->Y+kj)*input->Z+kk]);
                		       		}*/
                	       			kk = 0;
						do{
							sum += popcount(filter[((kX2+ki-i)*kY+kY2+kj-j)*input->Z+kk]^\
                                        	                           input->b[(ki*input->Y+kj)*input->Z+kk]);
							kk++;
						}while(kk<input->Z);
                			}
				}      
#else
				switch(input->Z)
				{
					case 0x1:
					sum += popcount(filter[ 0]^input->b[  leftX   *input->Y+leftY         ])+
					       popcount(filter[ 1]^input->b[  leftX   *input->Y+leftY + 1     ])+
					       popcount(filter[ 2]^input->b[  leftX   *input->Y+leftY + 2     ])+
					       popcount(filter[ 3]^input->b[ (leftX+1)*input->Y+leftY         ])+
					       popcount(filter[ 4]^input->b[ (leftX+1)*input->Y+leftY + 1     ])+
					       popcount(filter[ 5]^input->b[ (leftX+1)*input->Y+leftY + 2     ])+
					       popcount(filter[ 6]^input->b[ (leftX+2)*input->Y+leftY         ])+
					       popcount(filter[ 7]^input->b[ (leftX+2)*input->Y+leftY + 1     ])+
					       popcount(filter[ 8]^input->b[ (leftX+2)*input->Y+leftY + 2     ]);
					break;

					case 0x2: 
					sum += popcount(filter[ 0]^input->b[ (leftX   *input->Y+leftY    )*2    ])+
					       popcount(filter[ 1]^input->b[ (leftX   *input->Y+leftY    )*2 + 1])+
					       popcount(filter[ 2]^input->b[ (leftX   *input->Y+leftY + 1)*2    ])+
					       popcount(filter[ 3]^input->b[ (leftX   *input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[ 4]^input->b[ (leftX   *input->Y+leftY + 2)*2    ])+
					       popcount(filter[ 5]^input->b[ (leftX   *input->Y+leftY + 2)*2 + 1])+
					       popcount(filter[ 6]^input->b[((leftX+1)*input->Y+leftY    )*2    ])+
					       popcount(filter[ 7]^input->b[((leftX+1)*input->Y+leftY    )*2 + 1])+
					       popcount(filter[ 8]^input->b[((leftX+1)*input->Y+leftY + 1)*2    ])+
					       popcount(filter[ 9]^input->b[((leftX+1)*input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[10]^input->b[((leftX+1)*input->Y+leftY + 2)*2    ])+
					       popcount(filter[11]^input->b[((leftX+1)*input->Y+leftY + 2)*2 + 1])+
					       popcount(filter[12]^input->b[((leftX+2)*input->Y+leftY    )*2    ])+
					       popcount(filter[13]^input->b[((leftX+2)*input->Y+leftY    )*2 + 1])+
					       popcount(filter[14]^input->b[((leftX+2)*input->Y+leftY + 1)*2    ])+
					       popcount(filter[15]^input->b[((leftX+2)*input->Y+leftY + 1)*2 + 1])+
					       popcount(filter[16]^input->b[((leftX+2)*input->Y+leftY + 2)*2    ])+
					       popcount(filter[17]^input->b[((leftX+2)*input->Y+leftY + 2)*2 + 1]);
					break;

					case 0x4: 
					sum += popcount(filter[ 0]^input->b[ (leftX   *input->Y+leftY    )*4    ])+
					       popcount(filter[ 1]^input->b[ (leftX   *input->Y+leftY    )*4 + 1])+
					       popcount(filter[ 2]^input->b[ (leftX   *input->Y+leftY    )*4 + 2])+
					       popcount(filter[ 3]^input->b[ (leftX   *input->Y+leftY    )*4 + 3])+
					       popcount(filter[ 4]^input->b[ (leftX   *input->Y+leftY + 1)*4    ])+
					       popcount(filter[ 5]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[ 6]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[ 7]^input->b[ (leftX   *input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[ 8]^input->b[ (leftX   *input->Y+leftY + 2)*4    ])+
					       popcount(filter[ 9]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[10]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[11]^input->b[ (leftX   *input->Y+leftY + 2)*4 + 3])+
					       popcount(filter[12]^input->b[((leftX+1)*input->Y+leftY    )*4    ])+
					       popcount(filter[13]^input->b[((leftX+1)*input->Y+leftY    )*4 + 1])+
					       popcount(filter[14]^input->b[((leftX+1)*input->Y+leftY    )*4 + 2])+
					       popcount(filter[15]^input->b[((leftX+1)*input->Y+leftY    )*4 + 3])+
					       popcount(filter[16]^input->b[((leftX+1)*input->Y+leftY + 1)*4    ])+
					       popcount(filter[17]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[18]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[19]^input->b[((leftX+1)*input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[20]^input->b[((leftX+1)*input->Y+leftY + 2)*4    ])+
					       popcount(filter[21]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[22]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[23]^input->b[((leftX+1)*input->Y+leftY + 2)*4 + 3])+
					       popcount(filter[24]^input->b[((leftX+2)*input->Y+leftY    )*4    ])+
					       popcount(filter[25]^input->b[((leftX+2)*input->Y+leftY    )*4 + 1])+
					       popcount(filter[26]^input->b[((leftX+2)*input->Y+leftY    )*4 + 2])+
					       popcount(filter[27]^input->b[((leftX+2)*input->Y+leftY    )*4 + 3])+
					       popcount(filter[28]^input->b[((leftX+2)*input->Y+leftY + 1)*4    ])+
					       popcount(filter[29]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 1])+
					       popcount(filter[30]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 2])+
					       popcount(filter[31]^input->b[((leftX+2)*input->Y+leftY + 1)*4 + 3])+
					       popcount(filter[32]^input->b[((leftX+2)*input->Y+leftY + 2)*4    ])+
					       popcount(filter[33]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 1])+
					       popcount(filter[34]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 2])+
					       popcount(filter[35]^input->b[((leftX+2)*input->Y+leftY + 2)*4 + 3]);
					break;
				}
#endif
				comp1[(outi*outY+outj)*outZ+bink] |= (sum <= threshold1_int32[k])<<shift;
				comp2[(outi*outY+outj)*outZ+bink] |= (sum <= threshold2_int32[k])<<shift;
                		outj++;
			}
			outi++;
		}
		shift = (shift+1) % 32;
	}
    }

//assign output for 32 channels in on time
    for(outi = 0; outi < outX; outi++){
        for(outj = 0; outj < outY; outj++){
            for(outk = 0; outk < outZ; outk++){
#ifdef with_sign_Gamma_negative
		/*output->b[(outi*output->Y+outj)*output->Z+outk] = \
					(comp1[(outi*outY+outj)*outZ+outk] & comp2[(outi*outY+outj)*outZ+outk] & signGamma_positive[outk])\
					|(comp1[(outi*outY+outj)*outZ+outk] & (~comp2[(outi*outY+outj)*outZ+outk]) & signGamma_negative[outk])\
					|((~comp1[(outi*outY+outj)*outZ+outk])&signBetaStar[outk]);*/
		//optimized one
		output->b[(outi*output->Y+outj)*output->Z+outk] = \
					comp1[(outi*outY+outj)*outZ+outk] & ((comp2[(outi*outY+outj)*outZ+outk] & signGamma_positive[outk])\
					|((~comp2[(outi*outY+outj)*outZ+outk]) & signGamma_negative[outk]))\
					|((~comp1[(outi*outY+outj)*outZ+outk])&signBetaStar[outk]);
#else
		/*output->b[(outi*output->Y+outj)*output->Z+outk] = \
					(comp1[(outi*outY+outj)*outZ+outk] & comp2[(outi*outY+outj)*outZ+outk] & signGamma_positive[outk])\
					|(comp1[(outi*outY+outj)*outZ+outk] & (~comp2[(outi*outY+outj)*outZ+outk]) & (~signGamma_positive[outk]))\
					|((~comp1[(outi*outY+outj)*outZ+outk])&signBetaStar[outk]);*/
		//optimized one
		output->b[(outi*output->Y+outj)*output->Z+outk] = \
					comp1[(outi*outY+outj)*outZ+outk] & ((comp2[(outi*outY+outj)*outZ+outk] & signGamma_positive[outk])\
					|(~(comp2[(outi*outY+outj)*outZ+outk] | signGamma_positive[outk])))\
					|((~comp1[(outi*outY+outj)*outZ+outk])&signBetaStar[outk]);
#endif
            }
        }

    }
    delete input;
    delete comp1;
    delete comp2;
    return output;

}
#endif
