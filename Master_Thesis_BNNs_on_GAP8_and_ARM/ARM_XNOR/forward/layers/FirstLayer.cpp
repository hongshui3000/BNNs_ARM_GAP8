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

#elif defined(TestFix32)
	int32_t *bias = new int32_t[nFilters];
	read(bias, net->fData, nFilters, net->fPos);
	int32_t *weight = new int32_t[nFilters*kX*kY*net->Z];
	for(i = 0; i < nFilters; i++){
		read(&(weight[i*kX*kY*net->Z]), net->fData, kX, kY, net->Z, net->fPos);
	}
	// Batch Normalization 1
	int32_t *mean1 = new int32_t[nFilters];
	int32_t *std1 = new int32_t[nFilters];
	int32_t *gamma1 = new int32_t[nFilters];
	int32_t *beta1 = new int32_t [nFilters];
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

#elif defined(TestFix32)
    // Batch Normalization 2
	int32_t *mean2 = new int32_t[nFilters];
	int32_t *std2 = new int32_t[nFilters];
	int32_t *gamma2 = new int32_t[nFilters];
	int32_t *beta2 = new int32_t [nFilters];
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

	for(int kkk=0; kkk<nFilters; kkk++)
    {   if(bias[kkk]>1 || bias[kkk]<-1)
		printf("bias = %f\n",bias[kkk]);}
	for(int kkk=0; kkk<nFilters*kX*kY*net->Z; kkk++)
    {   if(weight[kkk]>1 || weight[kkk]<-1)
		printf("weight = %f\n",weight[kkk]);}
	for(int kkk=0; kkk<nFilters; kkk++)
    {   if(mean1[kkk]>1 || mean1[kkk]<-1)
		printf("mean1 = %f\n",mean1[kkk]);}
	for(int kkk=0; kkk<nFilters; kkk++)
    {   if(std1[kkk]>1 || std1[kkk]<-1)
		printf("std1 = %f\n",std1[kkk]);}
	for(int kkk=0; kkk<nFilters; kkk++)
    {   if(gamma1[kkk]>1 || gamma1[kkk]<-1)
		printf("gamma1 = %f\n",gamma1[kkk]);}
	for(int kkk=0; kkk<nFilters; kkk++)
    {   if(beta1[kkk]>1 || beta1[kkk]<-1)
		printf("beta1 = %f\n",beta1[kkk]);}
	for(int kkk=0; kkk<nFilters; kkk++)
    {   if(mean2[kkk]>1 || mean2[kkk]<-1)
		printf("mean2 = %f\n",mean2[kkk]);}
	for(int kkk=0; kkk<nFilters; kkk++)
    {   if(std2[kkk]>1 || std2[kkk]<-1)
		printf("std2 = %f\n",std2[kkk]);}
	for(int kkk=0; kkk<nFilters; kkk++)
    {   if(gamma2[kkk]>1 || gamma2[kkk]<-1)
		printf("gamma2 = %f\n",gamma2[kkk]);}
	for(int kkk=0; kkk<nFilters; kkk++)
    {   if(beta2[kkk]>1 || beta2[kkk]<-1)
		printf("beta2 = %f\n",beta2[kkk]);}
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

//#ifdef data_optimized
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
	#elif defined(TestFix32)
FirstLayer_::FirstLayer_(int32_t* weight_, int32_t *bias_, int32_t nFilters_,
		int32_t kX_, int32_t kY_, int32_t dX_, int32_t dY_, int32_t padX_, int32_t padY_,
		int32_t *mean1_, int32_t *std1_, int32_t *gamma1_, int32_t *beta1_,
		int32_t *mean2_, int32_t *std2_, int32_t *gamma2_, int32_t *beta2_){

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
//#endif
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

T* FirstLayer_::forward(T *input){

	int32_t i, j, k, ki, kj, kk, x0, y0, outi, outj;
	int32_t kX2, kY2, outX, outY;
	int32_t leftX, rightX, leftY, rightY;
	int32_t shift, bink;
	uint32_t aa,bb;
	T *output;
#ifdef TestFix16
	int16_t *filter;
	int64_t sum;
#elif defined(TestFix32)
	int32_t *filter;
	int64_t sum;
	int64_t sum2;
#else
	float *filter, sum, sum2;
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
		///////////////////////////////////out 2x2////////////////////////////////////
#ifdef UNFOLDED
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
				//printf("\n");
				for(ki = leftX; ki < rightX; ki++){
					for(kj = leftY; kj < rightY; kj++){
						aa = ((kX2+ki-i)*kY+kY2+kj-j)*input->Z;
						bb = (ki*input->Y+kj)*input->Z;
						for(kk = 0; kk < input->Z; kk++){
	#ifdef TestFix16
							sum += ((int64_t)filter[aa+kk]*(int64_t)input->fixp[bb+kk]);
						//	printf("sum %d = filter: %d * input : %d\n",((int32_t)filter[aa+kk]*(int32_t)input->fixp[bb+kk])>>8, filter[aa+kk], input->fixp[bb+kk]);
	#elif defined(TestFix32)
							sum += ((int64_t)filter[aa+kk]*(int64_t)input->fixp[bb+kk]);
	#else
							sum += (filter[aa+kk]*input->f[bb+kk]);
	#endif
						}
					}
				}
	#if defined(TestFix32)||defined(TestFix16)
				sum = sum >> INPUTSCALE;
	#endif
				//printf("input %d * -2 =  %d\n", (int32_t)input->fixp[47*88+1], (int32_t)input->fixp[47*88+1]*(int32_t)(-2));

	#ifdef TestFix16
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
				//printf("bias1 : %d\n",beta1[k]);
				sum = (0 > sum)? 0 : sum;
				//sum = max(0, sum);
				sum = (((((sum - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
	#elif defined(TestFix32)
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
				sum = (0 > sum)? 0 : sum;
				//sum = max(0, sum);
				sum = (((((sum - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
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
						sum += ((int64_t)filter[aa+kk]*(int64_t)input->fixp[bb+kk]);
	#elif defined(TestFix32)
						sum += ((int64_t)filter[aa+kk]*(int64_t)input->fixp[bb+kk]);
	#else
						sum += (filter[aa+kk]*input->f[bb+kk]);
	#endif
					}
				}
			}
	#if defined(TestFix32)||defined(TestFix16)
				sum = sum >> INPUTSCALE;
	#endif
	#ifdef TestFix16
			// Batch Normalization 1
			sum = sum + bias[k];
			sum = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
			sum = (0 > sum)? 0 : sum;
			//sum = max(0, sum);
			sum = (((((sum - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
	#elif defined(TestFix32)
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
				sum = (0 > sum)? 0 : sum;
				//sum = max(0, sum);
				sum = (((((sum - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
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
						sum += ((int64_t)filter[aa+kk]*(int64_t)input->fixp[bb+kk]);
	#elif defined(TestFix32)
						sum += ((int64_t)filter[aa+kk]*(int64_t)input->fixp[bb+kk]);
	#else
						sum += (filter[aa+kk]*input->f[bb+kk]);
	#endif
					}
				}
			}
	#if defined(TestFix32)||defined(TestFix16)
				sum = sum >> INPUTSCALE;
	#endif
	#ifdef TestFix16
			// Batch Normalization 1
			sum = sum + bias[k];
			sum = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
			sum = (0 > sum)? 0 : sum;
			//sum = max(0, sum);
			sum = (((((sum - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
	#elif defined(TestFix32)
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
				sum = (0 > sum)? 0 : sum;
				//sum = max(0, sum);
				sum = (((((sum - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
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
						sum += ((int64_t)filter[aa+kk]*(int64_t)input->fixp[bb+kk]);
	#elif defined(TestFix32)
						sum += ((int64_t)filter[aa+kk]*(int64_t)input->fixp[bb+kk]);
	#else
						sum += (filter[aa+kk]*input->f[bb+kk]);
	#endif
					}
				}
			}
	#if defined(TestFix32)||defined(TestFix16)
				sum = sum >> INPUTSCALE;
	#endif
	#ifdef TestFix16
			// Batch Normalization 1
			sum = sum + bias[k];
			sum = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
			sum = (0 > sum)? 0 : sum;
			//sum = max(0, sum);
			sum = (((((sum - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
	#elif defined(TestFix32)
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
				sum = (0 > sum)? 0 : sum;
				//sum = max(0, sum);
				sum = (((((sum - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
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
						sum += ((int64_t)filter[aa+kk]*(int64_t)input->fixp[bb+kk]);
	#elif defined(TestFix32)
						sum += ((int64_t)filter[aa+kk]*(int64_t)input->fixp[bb+kk]);
	#else
						sum += (filter[aa+kk]*input->f[bb+kk]);
	#endif
					}
				}
			}
	#if defined(TestFix32)||defined(TestFix16)
				sum = sum >> INPUTSCALE;
	#endif
	#ifdef TestFix16
			// Batch Normalization 1
			sum = sum + bias[k];
			sum = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
			sum = (0 > sum)? 0 : sum;
			//sum = max(0, sum);
			sum = (((((sum - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
	#elif defined(TestFix32)
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
				sum = (0 > sum)? 0 : sum;
				//sum = max(0, sum);
				sum = (((((sum - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
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
				sum +=         ((((int64_t) filter[ 0]*(int64_t)input->fixp[  leftX   *input->Y+leftY ]))+
						(((int64_t)filter[ 1]*(int64_t)input->fixp[  leftX   *input->Y+leftY + 1     ]))+
						(((int64_t)filter[ 2]*(int64_t)input->fixp[  leftX   *input->Y+leftY + 2     ]))+
						(((int64_t)filter[ 3]*(int64_t)input->fixp[ (leftX+1)*input->Y+leftY         ]))+
						(((int64_t)filter[ 4]*(int64_t)input->fixp[ (leftX+1)*input->Y+leftY + 1     ]))+
						(((int64_t)filter[ 5]*(int64_t)input->fixp[ (leftX+1)*input->Y+leftY + 2     ]))+
						(((int64_t)filter[ 6]*(int64_t)input->fixp[ (leftX+2)*input->Y+leftY         ]))+
						(((int64_t)filter[ 7]*(int64_t)input->fixp[ (leftX+2)*input->Y+leftY + 1     ]))+
						(((int64_t)filter[ 8]*(int64_t)input->fixp[ (leftX+2)*input->Y+leftY + 2     ])))>>INPUTSCALE;

	#elif defined(TestFix32)
				sum +=         ((((int64_t) filter[ 0]*(int64_t)input->fixp[  leftX   *input->Y+leftY ]))+
						(((int64_t)filter[ 1]*(int64_t)input->fixp[  leftX   *input->Y+leftY + 1     ]))+
						(((int64_t)filter[ 2]*(int64_t)input->fixp[  leftX   *input->Y+leftY + 2     ]))+
						(((int64_t)filter[ 3]*(int64_t)input->fixp[ (leftX+1)*input->Y+leftY         ]))+
						(((int64_t)filter[ 4]*(int64_t)input->fixp[ (leftX+1)*input->Y+leftY + 1     ]))+
						(((int64_t)filter[ 5]*(int64_t)input->fixp[ (leftX+1)*input->Y+leftY + 2     ]))+
						(((int64_t)filter[ 6]*(int64_t)input->fixp[ (leftX+2)*input->Y+leftY         ]))+
						(((int64_t)filter[ 7]*(int64_t)input->fixp[ (leftX+2)*input->Y+leftY + 1     ]))+
						(((int64_t)filter[ 8]*(int64_t)input->fixp[ (leftX+2)*input->Y+leftY + 2     ])))>>INPUTSCALE;
	#else

				sum +=  (filter[ 0]*input->f[  leftX   *input->Y+leftY         ])+
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
				sum = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
				sum = (0 > sum)? 0 : sum;
				//sum = max(0, sum);
				sum = (((((sum - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
	#elif defined(TestFix32)
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
				sum = (0 > sum)? 0 : sum;
				//sum = max(0, sum);
				sum = (((((sum - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
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
				//printf("unrolling\n");
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
						for(kk = 0; kk < input->Z; kk++)
						{
	#ifdef TestFix16
							sum += ((int64_t)filter[aa+kk]*(int64_t)input->fixp[bb+kk]); //2^((8 + 6) + (8 + 7) + 4 = 33)
							//printf("sum %d = filter: %d * input : %d\n",sum, filter[aa+kk], input->fixp[bb+kk]);
	#elif defined(TestFix32)
							sum += ((int32_t)filter[aa+kk]*(uint8_t)input->fixp[bb+kk]); //2^((8 + 20) + (8 + 20) + 4 = 60)
	#else
							sum += (filter[aa+kk]*input->f[bb+kk]);
	#endif
						}
					}
				}
	#if defined(TestFix32)||defined(TestFix16)
				sum = sum >> INPUTSCALE;
	#endif
	#ifdef TestFix16
				// Batch Normalization 1
				sum = sum + bias[k];
				sum = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
				sum = (0 > sum)? 0 : sum;
				//sum = max(0, sum);
				sum = (((((sum - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
	#elif defined(TestFix32)
				// Batch Normalization 1
				sum = sum + bias[k];
				sum2 = (((((sum - mean1[k])*std1[k])>>FIXSCALE)*gamma1[k])>>FIXSCALE)+beta1[k];
				sum2 = (0 > sum2)? 0 : sum2;
				//sum = max(0, sum);
				sum2 = (((((sum2 - mean2[k])*std2[k])>>FIXSCALE)*gamma2[k])>>FIXSCALE)+beta2[k];
	#else
				// Batch Normalization 1
				//printf("sum before: %f", sum);
				sum = sum + bias[k];
				sum = (sum - mean1[k])*std1[k]*gamma1[k]+beta1[k];
				sum = max(0.0, sum);
				sum = (sum - mean2[k])*std2[k]*gamma2[k]+beta2[k];

	#endif
				//printf(", after sum = %d, so \n", sum);
				if(sum2 > 0){
					output->b[(outi*output->Y+outj)*output->Z+bink] |= 1<<shift;
				}
				outj++;
			}
			//printf("sum = : %f\n", sum);
			outi++;
			//printf("output = : %x\n", output->b[(outi*output->Y+outj)*output->Z+bink]);
		}
/*	#ifdef TestFix16
				printf("fp16: mean1 = %d\n",mean1[k]);
				printf("fp16: gamma =         %d\n",gamma1[k]);
				printf("fp16: std1 =                %d\n",std1[k]);
				printf("fp16: sum =                      %d\n",sum);
	#elif defined(TestFix32)
				printf("fp32: mean1 = %d\n",mean1[k]);
				printf("fp32: gamma =         %d\n",gamma1[k]);
				printf("fp32: std1 =                %d\n",std1[k]);
				printf("fp32: sum =                      %d\n",sum);
	#else
				printf("Float: mean1 = %f\n",mean1[k]);
				printf("Float: gamma1 =       %f\n",gamma1[k]);
				printf("Float: std1 =               %f\n",std1[k]);
				printf("Float: sum =                      %f\n",sum);

	#endif*/
#endif
		shift = (shift+1) % 32;
	}

	delete input;

	return output;

}
