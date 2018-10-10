#include <cstdio>
#include <cmath>
#include <cstdint>

#include <functional>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include "model.h"
#include "tensor.h"
#include "layers/BinConvolution.h"
#include "layers/FirstLayer.h"
#include "layers/LastLayer.h"

#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <stdint.h>
using namespace std;

#include "aquila/global.h"
#include "aquila/source/WaveFile.h"
#include "aquila/source/window/HammingWindow.h"
#include "aquila/transform/Mfcc.h"
#include "aquila/tools/TextPlot.h"
#include "aquila/source/generator/SineGenerator.h"
#include "ZZZZ.dat"

const int FREQ = 16000;
const int WIN_LENGTH = 512;
const int WIN_OVERLAP = 384;
const int IMG_OVERLAP = 200;
#if defined(MFCC_PREPROCESS)||defined(Testaccuracy)
const int MFCC_BINS = 64;
const int IMG_WIDTH = 400;
#endif

#ifdef TestFix16
#ifndef Testaccuracy
//#include "input_int400.dat"
#include "1_1_f16.dat"
#endif
#elif defined(TestFix32)
#ifndef Testaccuracy
//#include "input_fp32_400.dat"
#include "1_1_f32.dat"
#endif
#else
#ifdef MFCC_PREPROCESS
#else
#ifndef Testaccuracy
//#include "input.dat"
#include "1_1_F.dat"
#endif
//	#include "input_240.dat"
#endif
#endif

//with 8 bit signGamma and signBetaStar, which are pre-calculated
#ifdef myPopcount_methode_2
#ifdef data_optimized
#ifdef with_sign_Gamma_negative
#ifdef TestFix32
//#include "model_optimize_fp32.dat"
//#include "model_mymodel_32f.dat"
#include "model_mymodel_32f_07_01.dat"
//#include "model_mymodel_32f_07_05.dat"
//#include "model_mymodel_32f_07_01_shift16.dat"  //the model data in Q7.16 resolution
//#include "model_mymodel_32f_07_01_shift12.dat"  //the model data in Q7.12 resolution
//#include "model_mymodel_32f_07_01_shift10.dat"    //the model data in Q7.10 resolution
//#include "model_mymodel_32f_07_01_shift9.dat"   //the model data in Q7.9  resolution
//#include "model_mymodel_32f_07_01_shift8.dat"
#else
//#include "model_optimize.dat"
//#include "model_mymodel.dat"
//#include "model_mymodel_F_Norm1.dat"
//#include "model_mymodel_F_07_05.dat"
#include "model_mymodel_F_07_01.dat"
#endif
#else
#include "model_optimize_without_sigGammaNeg.dat"
#endif
#endif
#endif

// with vectorized signGamma and signBetaStar
#ifdef Popcount_sign_vectorized
#ifdef with_sign_Gamma_negative
#include "model_optimize_sign_vectorized.dat"
#else
#include "model_optimize_sign_vectorized_without_sigGammaNeg.dat"
#endif
#endif

//original model dataset, no precalculation of threshold and so on.
#if defined(originalxnor)||defined(my32minusPopcount)|| defined(myPopcount)
#include "model.dat"
#endif

int main(){
	using namespace std;
	int32_t i, j, k;
	T *input, *output;

	const int nClasses = 28;
	double predict[nClasses], maximum;
	int number_eachClass[nClasses];
	int match_eachClass[nClasses];
	int top5_eachClass[nClasses];
#ifdef myPopcount_methode_2
	Model net(1, b32Data, b8Data, fData);
#elif defined(Popcount_sign_vectorized)
	Model net(1, b32Data, bSignData, fData);
#else
	Model net(1, bData, fData);
#endif
	// Preallocate memory for the convolutional layers

	FirstLayer(&net, 32, 3, 3, 1, 1, 1, 1, 1e-5, 1e-4);
	BinConvolution(&net, 64, 3, 3, 2, 2, 1, 1, true, 1e-4);
	BinConvolution(&net, 128, 3, 3, 1, 1, 1, 1, true, 1e-4);
	BinConvolution(&net, 128, 3, 3, 2, 2, 1, 1, true, 1e-4);
	BinConvolution(&net, 128, 3, 3, 1, 1, 1, 1, true, 1e-4);
	LastLayer(&net, 128, nClasses, 1e-3);

	double predict_temp = 0;

	for(i = 0; i < nClasses; i++){
		predict[i] = 0;
	}
	for(i = 0; i < nClasses; i++){
		number_eachClass[i] = 0;
	}	
	for(i = 0; i < nClasses; i++){
		match_eachClass[i] = 0;
	}
	for(i = 0; i < nClasses; i++){
		top5_eachClass[i] = 0;
	}

#ifdef Testaccuracy
	//FILE *fileList;
	float top_1, top_5[5],top_5_acc;
	static int32_t dataset = 1;
	static float match = 0, match_5 = 0;
	char wavName[120], wavPath[120], outPath[120];
	int32_t c, index, index_5[5];
	int fileCount = 6430;
	//int fileCount = 5218;
	//int fileCount = 1;

#ifdef TestFix16
	uint16_t data[64][400];
#elif defined(TestFix32)
	int32_t data[64][400];
#else
	float data[64][400];
#endif
	for(c = 0; c < fileCount; c++)
	{
		for(int row = 0; row < 64; row++)
		{
			for(int col = 0; col< 400; col++)
			{
				#if defined (TestFix16)|| defined (TestFix32)
				data[row][col] = 0;
				#else
				data[row][col] = 0.0;
				#endif
			}
		}
#ifdef TestFix16
		#ifdef TestFix16_7
		sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutf16_7/%d_%d.csv", dataset, c+1);
		#elif defined(TestFix16_8)||defined(TestFix16_9)
		sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutf16/%d_%d.csv", dataset, c+1);
		#else
		//sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutf32/%d_%d.csv", dataset, c+1);
		sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_Trun_No256/%d_%d.csv", dataset, c+1);
		#endif
#elif defined(TestFix32)
		//sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_No256/%d_%d.csv", dataset, c+1);
		sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutf32/%d_%d.csv", dataset, c+1);
		//sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutf16/%d_%d.csv", dataset, c+1);
		//sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_Trun_No256/%d_%d.csv", dataset, c+1);
#else
		sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF/%d_%d.csv", dataset, c+1);
		//sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_Trun_No256/%d_%d.csv", dataset, c+1);
#endif
		ifstream fileList(wavPath);

		if(!fileList.is_open())
		{
			dataset++;
#ifdef TestFix16
			#ifdef TestFix16_7
			sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutf16_7/%d_%d.csv", dataset, c+1);
			#elif defined(TestFix16_8)||defined(TestFix16_9)
			sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutf16/%d_%d.csv", dataset, c+1);
			#else
			//sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutf32/%d_%d.csv", dataset, c+1); //model with Qn.20 format
			sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_Trun_No256/%d_%d.csv", dataset, c+1);
			#endif
#elif defined(TestFix32)
			//sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_No256/%d_%d.csv", dataset, c+1);
			sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutf32/%d_%d.csv", dataset, c+1);
			//sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutf16/%d_%d.csv", dataset, c+1);
			//sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_Trun_No256/%d_%d.csv", dataset, c+1);
#else
			sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF/%d_%d.csv", dataset,c+1);
			//sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_Trun_No256/%d_%d.csv", dataset, c+1);
#endif

			ifstream fileList(wavPath);

			if(!fileList.is_open())
			{
				printf("Error opening file.\n");
			}
			printf("%s\n",wavPath);

			for(int row = 0; row < 64; row++)
			{
				string line3;
				getline(fileList, line3);
				if(!fileList.good())
				{
					break;		
				}
				stringstream iss3(line3);
				for(int col = 0; col< 400; col++)
				{
					string val;
					getline(iss3, val, ',');
					if(!iss3.good())
					{
						stringstream convertor(val);
						convertor>>data[row][col];
						data[row][col] = data[row][col];
					}
					stringstream convertor(val);
					convertor>>data[row][col];
					data[row][col] = data[row][col];
				}
			}

			fileList.close();
		}
		else
		{
			//open input csv correct
			printf("%s\n",wavPath);
			for(int row = 0; row < 64; row++)
			{
				string line4;
				getline(fileList, line4);
				if(!fileList.good())
				{
					break;		
				}
				stringstream iss4(line4);
				for(int col = 0; col< 400; col++)
				{
					string val;
					getline(iss4, val, ',');
					if(!iss4.good())
					{
						stringstream convertor(val);
						convertor>>data[row][col];
						data[row][col] = data[row][col];
						break;
					}
					stringstream convertor(val);
					convertor>>data[row][col];
					data[row][col] = data[row][col];
					//printf("Final: %d\n",data[row][col]);
				}
			}
			//printf("Final: %f\n",data[0][0]);
			//printf("Final: %d\n",data[47][0]);
			printf("Final: %d\n",data[47][1]);
			fileList.close();
		}
		// Size of the subsection
		const int32_t size = 80;
		int32_t pos, mfccL, mfccR, invalidL, invalidR;
		for(pos = 0; pos < IMG_WIDTH; pos += size){
			// Pad input from the left and right
			if(pos == 0){
				mfccL = 0;
				invalidL = 0;
			} else {
				mfccL = pos-12;
				invalidL = 3;
			}

			if(pos+size >= IMG_WIDTH){
				mfccR = IMG_WIDTH;
				invalidR = 0;
			} else {
				mfccR = pos+size+8;
				invalidR = 2;
			}
			// Read input
			#if defined(TestFix16)||defined(TestFix32)
			input = new T(T_FIXPOINT, MFCC_BINS, mfccR-mfccL, 1);
			#else
			input = new T(T_FLOAT, MFCC_BINS, mfccR-mfccL, 1);
			#endif
			for(i = 0; i < MFCC_BINS; i++){
				for(j = mfccL; j < mfccR; j++){
			#if defined(TestFix16)||defined(TestFix32)
					input->fixp[i*input->Y+j-mfccL] = data[i][j];
			#else
					input->f[i*input->Y+j-mfccL] = data[i][j];
			#endif
				}
			}
			//printf("input : final %d\n",(int32_t)input->fixp[47*88]);
			//printf("input : final %d\n",input->fixp[47*88+1]);
			// Forward pass
			output = net.forward(input);
			// Accumulate the result
			for(k = 0; k < output->Z; k++){
				predict_temp = 0;
				for(i = 0; i < output->X; i++){
					for(j = invalidL; j < output->Y-invalidR; j++){
						//		    predict[k] += output->f[(i*output->Y+j)*output->Z+k];
						predict_temp += output->f[(i*output->Y+j)*output->Z+k];		    
					}
				}
				predict[k] += predict_temp;
			}

			delete output;
		}

		// AveragePool
		for(i = 0; i < nClasses; i++){
			predict[i] = predict[i]/(16*100);
		}

		// LogSoftMax
		float sum = 0;

		for(i = 0; i < nClasses; i++){
			sum += expf(predict[i]);
		}

		sum = 1/sum;

		for(i = 0; i < nClasses; i++){
			predict[i] = logf(sum*expf(predict[i]));
		}

		// Print result
//		for(i = 0; i < nClasses; i++){
//
//			printf("%8.4f\n", predict[i]);
//		}
		
		for(i = 0; i < 5; i++){
			top_5[i] = -1000.2;
			index_5[i] = 0;
		}
		maximum = -1000.2;
		for(i = 0; i < nClasses; i++){
			if(predict[i]>maximum)
			{
				maximum = predict[i];
				index = i;
			}
			if(predict[i]>top_5[4])
			{
				if(predict[i]>top_5[3])
				{
					if(predict[i]>top_5[2])
					{
						if(predict[i]>top_5[1])
						{
							if(predict[i]>top_5[0])
							{
								top_5[4] = top_5[3];
								top_5[3] = top_5[2];
								top_5[2] = top_5[1];
								top_5[1] = top_5[0];
								top_5[0] = predict[i];
								index_5[4] = index_5[3];
								index_5[3] = index_5[2];
								index_5[2] = index_5[1];
								index_5[1] = index_5[0];
								index_5[0] = i;
							}
							else{
							top_5[4] = top_5[3];
							top_5[3] = top_5[2];
							top_5[2] = top_5[1];
							top_5[1] = predict[i];
							index_5[4] = index_5[3];
							index_5[3] = index_5[2];
							index_5[2] = index_5[1];
							index_5[1] = i;
							}
						}else{
						top_5[4] = top_5[3];
						top_5[3] = top_5[2];
						top_5[2] = predict[i];
						index_5[4] = index_5[3];
						index_5[3] = index_5[2];
						index_5[2] = i;
						}
					}else{
					top_5[4] = top_5[3];
					top_5[3] = predict[i];
					index_5[4] = index_5[3];
					index_5[3] = i;
					}
				}else{
				top_5[4] = predict[i];
				index_5[4] = i;
				}
			}
		}				
		printf("maximum index = %d\n", index+1);
		printf("dataset: %d\n", dataset);
		if(dataset == index+1)
		{
			match = match + 1;
			match_eachClass[dataset-1] = match_eachClass[dataset-1] + 1;
		}
		number_eachClass[dataset-1] = number_eachClass[dataset-1] + 1;
		for(i = 0; i < 5; i++){
			printf("top[%d]: %d\n", i+1,index_5[i]+1);
		}
		if(index_5[0]+1 == dataset){
			top5_eachClass[dataset-1] = top5_eachClass[dataset-1] + 1;
			match_5 = match_5 + 1;
		}else if(index_5[1]+1 == dataset){
			top5_eachClass[dataset-1] = top5_eachClass[dataset-1] + 1;
			match_5 = match_5 + 1;
		}else if(index_5[2]+1 == dataset){
			top5_eachClass[dataset-1] = top5_eachClass[dataset-1] + 1;
			match_5 = match_5 + 1;
		}else if(index_5[3]+1 == dataset){
			top5_eachClass[dataset-1] = top5_eachClass[dataset-1] + 1;
			match_5 = match_5 + 1;
		}else if(index_5[4]+1 == dataset){
			top5_eachClass[dataset-1] = top5_eachClass[dataset-1] + 1;
			match_5 = match_5 + 1;
			}
		top_1 = (float) (match/fileCount)*100;
		top_5_acc = (float) (match_5/fileCount)*100;
		for(i = 0; i < nClasses; i++){

			printf("Class %d, Top1:%8.4f %, %d/%d , Top5:%8.4f %, %d/%d\n", i, (float)match_eachClass[i]*100/(float)number_eachClass[i],\
			 															match_eachClass[i], 
			 															number_eachClass[i],
			 															(float)top5_eachClass[i]*100/(float)number_eachClass[i], 
			 															top5_eachClass[i], 
			 															number_eachClass[i]);
		}
		//Top-1 accuracy:
		printf("Overall Top-1 accuracy : %f %, %d/%d \n", top_1,(int) match,(int) fileCount);
		printf("Overall Top-5 accuracy : %f %, %d/%d \n", top_5_acc,(int) match_5,(int) fileCount);


#ifdef originalxnor
		printf("originalxnor \n");
#elif defined(my32minusPopcount)
		printf("my32minusPopcount \n");
#elif defined(myPopcount)
		printf("myPopcount \n");
#elif defined(myPopcount_methode_2)
		printf("myPopcount_methode_2 \n");
#ifdef data_optimized
#ifdef TestFix16
		printf("data_optimized with TestFix16 \n");
#elif defined(TestFix32)
		printf("data_optimized with TestFix32 \n");
#else
		printf("data_optimized \n");
#endif
#endif
#ifdef with_sign_Gamma_negative
		printf("with_sign_Gamma_negative \n");
#else
		printf("withOUT_sign_Gamma_negative \n");
#endif
#ifdef UNFOLDED
		printf("UNFOLDED \n");
#endif
#elif defined(Popcount_sign_vectorized)
#ifdef signGamma_signBetaStar_optimized
		printf("signGamma_signBetaStar_optimized \n");
#endif
#ifdef with_sign_Gamma_negative
		printf("with_sign_Gamma_negative \n");
#else
		printf("withOUT_sign_Gamma_negative \n");
#endif
#ifdef UNFOLDED
		printf("UNFOLDED \n");
#endif
#ifdef MFCC_PREPROCESS
		printf("MFCC on board preprocess\n");
#endif
#endif

		/*sort(predict, predict+28);
		  reverse(predict, predict+28);
		  printf("-----------------------ordered-------------------------\n");

		// Print ordered result
		for(i = 0; i < nClasses; i++){

		printf("%8.4f\n", predict[i]);
		}*/

	}


	/////////////////////////////////////////////

#else


#if defined(TestFix16)||defined(TestFix32)
	// Size of the subsection
	const int32_t size = 100;
	int32_t pos, mfccL, mfccR, invalidL, invalidR;
	for(pos = 0; pos < IMG_WIDTH; pos += size){
		// Pad input from the left and right
		if(pos == 0){
			mfccL = 0;
			invalidL = 0;
		} else {
			mfccL = pos-12;
			invalidL = 3;
		}

		if(pos+size >= IMG_WIDTH){
			mfccR = IMG_WIDTH;
			invalidR = 0;
		} else {
			mfccR = pos+size+8;
			invalidR = 2;
		}
		printf("Input 100 \n");
		// Read input
		input = new T(T_FIXPOINT, MFCC_BINS, mfccR-mfccL, 1);
		for(i = 0; i < MFCC_BINS; i++){
			for(j = mfccL; j < mfccR; j++){
				input->fixp[i*input->Y+j-mfccL] = mfcc[i][j];
			}
		}
#else
		// Size of the subsection
		const int32_t size = 100;
		int32_t pos, mfccL, mfccR, invalidL, invalidR;
		for(pos = 0; pos < IMG_WIDTH; pos += size){
			// Pad input from the left and right
			if(pos == 0){
				mfccL = 0;
				invalidL = 0;
			} else {
				mfccL = pos-12;
				invalidL = 3;
			}

			if(pos+size >= IMG_WIDTH){
				mfccR = IMG_WIDTH;
				invalidR = 0;
			} else {
				mfccR = pos+size+8;
				invalidR = 2;
			}
			printf("Input 100 \n");
			// Read input
			input = new T(T_FLOAT, MFCC_BINS, mfccR-mfccL, 1);
			for(i = 0; i < MFCC_BINS; i++){
				for(j = mfccL; j < mfccR; j++){
#ifdef MFCC_PREPROCESS
					input->f[i*input->Y+j-mfccL] = img[i][j];
#else
					input->f[i*input->Y+j-mfccL] = mfcc[i][j];
#endif
				}
			}
#endif

			// Forward pass

			output = net.forward(input);


			// Accumulate the result
			for(k = 0; k < output->Z; k++){
				predict_temp = 0;
				for(i = 0; i < output->X; i++){
					for(j = invalidL; j < output->Y-invalidR; j++){
						//		    predict[k] += output->f[(i*output->Y+j)*output->Z+k];
						predict_temp += output->f[(i*output->Y+j)*output->Z+k];		    
					}
				}
				predict[k] += predict_temp;
			}

			delete output;
		}

		// AveragePool
		for(i = 0; i < nClasses; i++){
			predict[i] = predict[i]/(16*100);
		}

		// LogSoftMax
		float sum = 0;

		for(i = 0; i < nClasses; i++){
			sum += expf(predict[i]);
		}

		sum = 1/sum;

		for(i = 0; i < nClasses; i++){
			predict[i] = logf(sum*expf(predict[i]));
		}

		// Print result
		for(i = 0; i < nClasses; i++){

			printf("%8.4f\n", predict[i]);
		}
#ifdef originalxnor
		printf("originalxnor \n");
#elif defined(my32minusPopcount)
		printf("my32minusPopcount \n");
#elif defined(myPopcount)
		printf("myPopcount \n");
#elif defined(myPopcount_methode_2)
		printf("myPopcount_methode_2 \n");
#ifdef data_optimized
#ifdef TestFix16
		printf("data_optimized with TestFix16 \n");
#elif defined(TestFix32)
		printf("data_optimized with TestFix32 \n");
#else
		printf("data_optimized \n");
#endif
#endif
#ifdef with_sign_Gamma_negative
		printf("with_sign_Gamma_negative \n");
#else
		printf("withOUT_sign_Gamma_negative \n");
#endif
#ifdef UNFOLDED
		printf("UNFOLDED \n");
#endif
#elif defined(Popcount_sign_vectorized)
#ifdef signGamma_signBetaStar_optimized
		printf("signGamma_signBetaStar_optimized \n");
#endif
#ifdef with_sign_Gamma_negative
		printf("with_sign_Gamma_negative \n");
#else
		printf("withOUT_sign_Gamma_negative \n");
#endif
#ifdef UNFOLDED
		printf("UNFOLDED \n");
#endif
#ifdef MFCC_PREPROCESS
		printf("MFCC on board preprocess\n");
#endif
#endif
		//delete input;
#endif

		return 0;

	}
