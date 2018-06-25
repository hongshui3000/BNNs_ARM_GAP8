#include "FirstLayer.h"

void FirstLayer(Model *net, int32_t nFilters,
		int32_t kX, int32_t kY, int32_t dX, int32_t dY,
		int32_t padX, int32_t padY, float eps1, float eps2){
	int32_t i;
#ifdef TestFix16
	int16_t *bias = new int16_t[nFilters];
	read(bias, net->fData, nFilters, net->fPos);
	int16_t *weight = new int16_t[nFilters*kX*kY*net->Z];
	for(i = 0; i < nFilters; i++){
		read(&(weight[i*kX*kY*net->Z]), net->fData, kX, kY, net->Z, net->fPos);
	}

	// Batch Normalization 1
	int16_t *mean1 = new int16_t[nFilters];
	int16_t *std1 = new int16_t[nFilters];
	int16_t *gamma1 = new int16_t[nFilters];
	int16_t *beta1 = new int16_t [nFilters];

	read(mean1, net->fData, nFilters, net->fPos);
	read(std1, net->fData, nFilters, net->fPos);
	read(gamma1, net->fData, nFilters, net->fPos);
	read(beta1, net->fData, nFilters, net->fPos);
#else
	float *bias = new float[nFilters];
	read(bias, net->fData, nFilters, net->fPos);
	float *weight = new float[nFilters*kX*kY*net->Z];
	for(i = 0; i < nFilters; i++){
		read(&(weight[i*kX*kY*net->Z]), net->fData, kX, kY, net->Z, net->fPos);
	}

	// Batch Normalization 1
	float *mean1 = new float[nFilters];
	float *std1 = new float[nFilters];
	float *gamma1 = new float[nFilters];
	float *beta1 = new float [nFilters];

	read(mean1, net->fData, nFilters, net->fPos);
	read(std1, net->fData, nFilters, net->fPos);
	read(gamma1, net->fData, nFilters, net->fPos);
	read(beta1, net->fData, nFilters, net->fPos);
#endif

#if defined (data_optimized) || defined(signGamma_signBetaStar_optimized)
#else
	for(i = 0; i < nFilters; i++){
		std1[i] = 1.0/sqrt(std1[i]+eps1);
	}
#endif

#ifdef TestFix16
	// Batch Normalization 2
	int16_t *mean2 = new int16_t[nFilters];
	int16_t *std2 = new int16_t[nFilters];
	int16_t *gamma2 = new int16_t[nFilters];
	int16_t *beta2 = new int16_t [nFilters];

	read(mean2, net->fData, nFilters, net->fPos);
	read(std2, net->fData, nFilters, net->fPos);
	read(gamma2, net->fData, nFilters, net->fPos);
	read(beta2, net->fData, nFilters, net->fPos);
#else
	// Batch Normalization 2
	float *mean2 = new float[nFilters];
	float *std2 = new float[nFilters];
	float *gamma2 = new float[nFilters];
	float *beta2 = new float [nFilters];

	read(mean2, net->fData, nFilters, net->fPos);
	read(std2, net->fData, nFilters, net->fPos);
	read(gamma2, net->fData, nFilters, net->fPos);
	read(beta2, net->fData, nFilters, net->fPos);
#endif
#if defined (data_optimized) || defined(signGamma_signBetaStar_optimized)
#else
	for(i = 0; i < nFilters; i++){
		std2[i] = 1.0/sqrt(std2[i]+eps2);
	}
#endif
	FirstLayer_ *layer = new FirstLayer_(weight, bias, nFilters, kX, kY, dX, dY, padX, padY,
			mean1, std1, gamma1, beta1, mean2, std2, gamma2, beta2);

	net->addLayer(layer);


	net->Z = nFilters;

}

#ifdef TestFix16
FirstLayer_::FirstLayer_(int16_t* weight_, int16_t *bias_, int32_t nFilters_,
		int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, int32_t padX_, int32_t padY_,
		int16_t *mean1_, int16_t *std1_, int16_t *gamma1_, int16_t *beta1_,
		int16_t *mean2_, int16_t *std2_, int16_t *gamma2_, int16_t *beta2_){

	weight = weight_;
	bias = bias_;
	nFilters = nFilters_;
	kX = kX_;
	kY = kY_;
	padX = padX_;
	padY = padY_;
	dX = dX_;
	dY = dY_;
	mean1 = mean1_;
	std1 = std1_;
	gamma1 = gamma1_;
	beta1 = beta1_;
	mean2 = mean2_;
	std2 = std2_;
	gamma2 = gamma2_;
	beta2 = beta2_;

}
#else
FirstLayer_::FirstLayer_(float* weight_, float *bias_, int32_t nFilters_,
		int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, int32_t padX_, int32_t padY_,
		float *mean1_, float *std1_, float *gamma1_, float *beta1_,
		float *mean2_, float *std2_, float *gamma2_, float *beta2_){

	weight = weight_;
	bias = bias_;
	nFilters = nFilters_;
	kX = kX_;
	kY = kY_;
	padX = padX_;
	padY = padY_;
	dX = dX_;
	dY = dY_;
	mean1 = mean1_;
	std1 = std1_;
	gamma1 = gamma1_;
	beta1 = beta1_;
	mean2 = mean2_;
	std2 = std2_;
	gamma2 = gamma2_;
	beta2 = beta2_;

}

#endif
FirstLayer_::~FirstLayer_(){
	delete[] weight;
	delete[] bias;
	delete[] mean1;
	delete[] std1;
	delete[] gamma1;
	delete[] beta1;
	delete[] mean2;
	delete[] std2;
	delete[] gamma2;
	delete[] beta2;

}

T* FirstLayer_::forward(T* __restrict__ input){
    BSP_LED_Toggle(LED1);
	int32_t i, j, k, ki, kj, kk, x0, y0, outi, outj;
	int32_t kX2, kY2, outX, outY;
	int32_t leftX, rightX, leftY, rightY;
	int32_t shift, bink;
	uint32_t aa,bb;
	T *output;
#ifdef TestFix16
	int16_t *filter;
	int32_t sum;
#else
	float *filter, sum;
#endif

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

	kX2 = kX/2;
	kY2 = kY/2;

	x0 = kX2-padX;
	y0 = kY2-padY;

	shift = 0;
	for(k = 0; k < nFilters; k++){
		filter = &(weight[k*kX*kY*input->Z]);
		bink = k / 32;
#ifdef UNFOLDED
		///////////////////////////////////out 2x2////////////////////////////////////
		outi = 0;
		for(i = x0; i < input->X-x0; i += input->X - x0 - dX){
			//Range on X where to iterate with the filter
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
						aa = ((kX2+ki-i)*kY+kY2+kj-j)*input->Z;
						bb = (ki*input->Y+kj)*input->Z;
						for(kk = 0; kk < input->Z; kk++){
#ifdef TestFix16
							sum += (filter[aa+kk]*input->b[bb+kk]);
#else
							sum += (filter[aa+kk]*input->f[bb+kk]);
#endif
						}
					}
				}

#ifdef TestFix16
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = ((((sum - mean1[k])*std1[k])>>8)*gamma1[k]>>8)+beta1[k];
				sum = max(0.0, sum);
				sum = ((((sum - mean2[k])*std2[k])>>8)*gamma2[k]>>8)+beta2[k];
#else
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = (sum - mean1[k])*std1[k]*gamma1[k]+beta1[k];
				sum = max(0.0, sum);
				sum = (sum - mean2[k])*std2[k]*gamma2[k]+beta2[k];
#endif
				if(sum > 0){
					output->b[(outi*output->Y+outj)*output->Z+bink] |= 1<<shift;
				}
				outj = outY -1;
			}
			outi = outX-1;
		}

		/////////////////////////edge 2X3, 3X2 (four edges)////---------edge outputY = 0------------------------
		outi = 1;
		outj = 0;
		for(i = x0+dX; i < input->X-x0-dX; i += dX){
			leftX = max(0, i-kX2);
			rightX = min(input->X, i+kX2+1);
			sum = 0;
			// Calculate binary convolution
			for(ki = leftX; ki < rightX; ki++){
				for(kj = 0; kj < 2; kj++){
					aa = ((kX2+ki-i)*kY+kY2+kj-y0)*input->Z;
					bb = (ki*input->Y+kj)*input->Z;
					for(kk = 0; kk < input->Z; kk++)
					{
#ifdef TestFix16
						sum += (filter[aa+kk]*input->b[bb+kk]);
#else
						sum += (filter[aa+kk]*input->f[bb+kk]);
#endif
					}
				}
			}

#ifdef TestFix16
			// Batch Normalization 1
			sum = sum + bias[k];
			sum = ((((sum - mean1[k])*std1[k])>>8)*gamma1[k]>>8)+beta1[k];
			sum = max(0.0, sum);
			sum = ((((sum - mean2[k])*std2[k])>>8)*gamma2[k]>>8)+beta2[k];
#else
			// Batch Normalization 1
			sum = sum + bias[k];
			sum = (sum - mean1[k])*std1[k]*gamma1[k]+beta1[k];
			sum = max(0.0, sum);
			sum = (sum - mean2[k])*std2[k]*gamma2[k]+beta2[k];
#endif
			if(sum > 0){
				output->b[(outi*output->Y+outj)*output->Z+bink] |= 1<<shift;
			}
			outi++;
		}// output(1,0) to output(outi-1,0) done
		//-------------------edge outputX = 0------------------------//
		outi = 0;
		outj = 1;
		for(j = y0+dY; j < input->Y-y0-dY; j += dY){
			leftY = max(0, j-kY2);
			rightY = min(input->Y, j+kY2+1);
			sum = 0;
			// Calculate binary convolution
			for(ki = 0; ki < 2; ki++){
				for(kj = leftY; kj < rightY; kj++){
					aa = ((kX2+ki-x0)*kY+kY2+kj-j)*input->Z;
					bb = (ki*input->Y+kj)*input->Z;
					for(kk = 0; kk < input->Z; kk++){
#ifdef TestFix16
						sum += (filter[aa+kk]*input->b[bb+kk]);
#else
						sum += (filter[aa+kk]*input->f[bb+kk]);
#endif
					}
				}
			}

#ifdef TestFix16
			// Batch Normalization 1
			sum = sum + bias[k];
			sum = ((((sum - mean1[k])*std1[k])>>8)*gamma1[k]>>8)+beta1[k];
			sum = max(0.0, sum);
			sum = ((((sum - mean2[k])*std2[k])>>8)*gamma2[k]>>8)+beta2[k];
#else
			// Batch Normalization 1
			sum = sum + bias[k];
			sum = (sum - mean1[k])*std1[k]*gamma1[k]+beta1[k];
			sum = max(0.0, sum);
			sum = (sum - mean2[k])*std2[k]*gamma2[k]+beta2[k];
#endif
			if(sum > 0){
				output->b[(outi*output->Y+outj)*output->Z+bink] |= 1<<shift;
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
			leftY = max(0, j-kY2);
			rightY = min(input->Y, j+kY2+1);
			sum = 0;
			// Calculate binary convolution
			for(ki = leftX; ki < rightX; ki++){
				for(kj = leftY; kj < rightY; kj++){
					aa = ((kX2+ki-i)*kY+kY2+kj-j)*input->Z;
					bb = (ki*input->Y+kj)*input->Z;
					for(kk = 0; kk < input->Z; kk++){
#ifdef TestFix16
						sum += (filter[aa+kk]*input->b[bb+kk]);
#else
						sum += (filter[aa+kk]*input->f[bb+kk]);
#endif
					}
				}
			}

#ifdef TestFix16
			// Batch Normalization 1
			sum = sum + bias[k];
			sum = ((((sum - mean1[k])*std1[k])>>8)*gamma1[k]>>8)+beta1[k];
			sum = max(0.0, sum);
			sum = ((((sum - mean2[k])*std2[k])>>8)*gamma2[k]>>8)+beta2[k];
#else
			// Batch Normalization 1
			sum = sum + bias[k];
			sum = (sum - mean1[k])*std1[k]*gamma1[k]+beta1[k];
			sum = max(0.0, sum);
			sum = (sum - mean2[k])*std2[k]*gamma2[k]+beta2[k];
#endif
			if(sum > 0){
				output->b[(outi*output->Y+outj)*output->Z+bink] |= 1<<shift;
			}
			outj++;
		}// output(outX,1) to output(outX,outY-dY) done
		//-------------------edge outputY = outY------------------------//
		outi = 1;
		outj = outY-1;
		j = input->Y-dY;
		leftY = max(0, j-kY2);
		rightY = min(input->Y, j+kY2+1);
		for(i = x0+dX; i < input->X-x0-dX; i += dX){
			leftX = max(0, i-kX2);
			rightX = min(input->X, i+kX2+1);
			sum = 0;
			// Calculate binary convolution
			for(ki = leftX; ki < rightX; ki++){
				for(kj = leftY; kj < rightY; kj++){
					aa = ((kX2+ki-i)*kY+kY2+kj-j)*input->Z;
					bb = (ki*input->Y+kj)*input->Z;
					for(kk = 0; kk < input->Z; kk++){
#ifdef TestFix16
						sum += (filter[aa+kk]*input->b[bb+kk]);
#else
						sum += (filter[aa+kk]*input->f[bb+kk]);
#endif
					}
				}
			}

#ifdef TestFix16
			// Batch Normalization 1
			sum = sum + bias[k];
			sum = ((((sum - mean1[k])*std1[k])>>8)*gamma1[k]>>8)+beta1[k];
			sum = max(0.0, sum);
			sum = ((((sum - mean2[k])*std2[k])>>8)*gamma2[k]>>8)+beta2[k];
#else
			// Batch Normalization 1
			sum = sum + bias[k];
			sum = (sum - mean1[k])*std1[k]*gamma1[k]+beta1[k];
			sum = max(0.0, sum);
			sum = (sum - mean2[k])*std2[k]*gamma2[k]+beta2[k];
#endif
			if(sum > 0){
				output->b[(outi*output->Y+outj)*output->Z+bink] |= 1<<shift;
			}
			outi++;
		}// output(outX,1) to output(outX,outY-1) done
		//  ////////////////////////3x3 (one center)////////////////////////////////
		outi = 1;
		for(i = x0+dX; i < input->X-x0-dX; i += dX){
			// Range on X where to iterate with the filter
			//leftX = max(0, i-kX2);
			//rightX = min(input->X, i+kX2+1);
			leftX = i-kX2;
			rightX = i+kX2+1;
			// Y position in the output
			outj = 1;
			for(j = y0+dY; j < input->Y-y0-dY; j += dY){
				// Range on Y where to iterate with the filter
				//leftY = max(0, j-kY2);
				//rightY = min(input->Y, j+kY2+1);
				leftY = j-kY2;
				rightY = j+kY2+1;
				// Calculate binary convolution
				sum = 0;

#ifdef TestFix16
				sum += (filter[ 0]*input->b[  leftX   *input->Y+leftY         ])>>8+
						(filter[ 1]*input->b[  leftX   *input->Y+leftY + 1     ])>>8+
						(filter[ 2]*input->b[  leftX   *input->Y+leftY + 2     ])>>8+
						(filter[ 3]*input->b[ (leftX+1)*input->Y+leftY         ])>>8+
						(filter[ 4]*input->b[ (leftX+1)*input->Y+leftY + 1     ])>>8+
						(filter[ 5]*input->b[ (leftX+1)*input->Y+leftY + 2     ])>>8+
						(filter[ 6]*input->b[ (leftX+2)*input->Y+leftY         ])>>8+
						(filter[ 7]*input->b[ (leftX+2)*input->Y+leftY + 1     ])>>8+
						(filter[ 8]*input->b[ (leftX+2)*input->Y+leftY + 2     ])>>8;
#else

				sum += (filter[ 0]*input->f[  leftX   *input->Y+leftY         ])+
					(filter[ 1]*input->f[  leftX   *input->Y+leftY + 1     ])+
					(filter[ 2]*input->f[  leftX   *input->Y+leftY + 2     ])+
					(filter[ 3]*input->f[ (leftX+1)*input->Y+leftY         ])+
					(filter[ 4]*input->f[ (leftX+1)*input->Y+leftY + 1     ])+
					(filter[ 5]*input->f[ (leftX+1)*input->Y+leftY + 2     ])+
					(filter[ 6]*input->f[ (leftX+2)*input->Y+leftY         ])+
					(filter[ 7]*input->f[ (leftX+2)*input->Y+leftY + 1     ])+
					(filter[ 8]*input->f[ (leftX+2)*input->Y+leftY + 2     ]);
#endif
#ifdef TestFix16
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = ((((sum - mean1[k])*std1[k])>>8)*gamma1[k]>>8)+beta1[k];
				sum = max(0.0, sum);
				sum = ((((sum - mean2[k])*std2[k])>>8)*gamma2[k]>>8)+beta2[k];
#else
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = (sum - mean1[k])*std1[k]*gamma1[k]+beta1[k];
				sum = max(0.0, sum);
				sum = (sum - mean2[k])*std2[k]*gamma2[k]+beta2[k];
#endif
				if(sum > 0){
					output->b[(outi*output->Y+outj)*output->Z+bink] |= 1<<shift;
				}
				outj++;
			}
			outi++;
		}
#else
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
						aa = ((kX2+ki-i)*kY+kY2+kj-j)*input->Z;
						bb = (ki*input->Y+kj)*input->Z;
						for(kk = 0; kk < input->Z; kk++){
#ifdef TestFix16
							sum += (filter[aa+kk]*input->b[bb+kk])>>8;
#else
							sum += (filter[aa+kk]*input->f[bb+kk]);
#endif
						}
					}
				}

#ifdef TestFix16
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = ((((sum - mean1[k])*std1[k])>>8)*gamma1[k]>>8)+beta1[k];
				sum = max(0.0, sum);
				sum = ((((sum - mean2[k])*std2[k])>>8)*gamma2[k]>>8)+beta2[k];
#else
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = (sum - mean1[k])*std1[k]*gamma1[k]+beta1[k];
				sum = max(0.0, sum);
				sum = (sum - mean2[k])*std2[k]*gamma2[k]+beta2[k];
#endif
				if(sum > 0){
					output->b[(outi*output->Y+outj)*output->Z+bink] |= 1<<shift;
				}
				outj++;
			}
			outi++;
		}



#endif

		shift = (shift+1) % 32;
	}

    delete input;
    BSP_LED_Toggle(LED1);

    return output;

}
