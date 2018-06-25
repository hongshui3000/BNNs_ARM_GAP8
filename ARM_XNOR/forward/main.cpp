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
#ifdef MFCC_PREPROCESS
const int MFCC_BINS = 64;
const int IMG_WIDTH = 400;
#endif

#ifdef TestFix16
//#include "input_fp16.dat"
#include "input_int400.dat"
#elif defined(TestFix32)
//#include "input_fp16.dat"
#include "input_fp32_400.dat"
//#include "input_fp32.dat"
#else
#ifdef MFCC_PREPROCESS
#else
#include "input.dat"
//	#include "input_240.dat"
#endif
#endif

//with 8 bit signGamma and signBetaStar, which are pre-calculated
#ifdef myPopcount_methode_2
#ifdef data_optimized
#ifdef with_sign_Gamma_negative
#ifdef TestFix16
#include "model_optimize_fp16.dat"
#elif defined(TestFix32)
#include "model_optimize_fp32.dat"
#else
#include "model_optimize.dat"
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

#ifdef MFCC_PREPROCESS
	//MFCC
	int fileCount, c, winCount, winStep, frame;
	static float img[MFCC_BINS][IMG_WIDTH];

	Aquila::HammingWindow hamming(WIN_LENGTH);
	Aquila::Mfcc MFCC(WIN_LENGTH);
	vector<Aquila::SampleType> wave_amp(WIN_LENGTH);

	winStep = WIN_LENGTH - WIN_OVERLAP;
	frame = 0;
	for(j = 0; j < IMG_WIDTH*winStep; j += winStep){
		vector<Aquila::SampleType> win(WIN_LENGTH);
		for(k = 0; k < WIN_LENGTH; k++){
			wave_amp[k] = wav_sample[j+k];
		}
		Aquila::SignalSource wav_samp(wave_amp);
		for(k = 0; k < WIN_LENGTH; k++){
			win[k] = hamming.sample(k)*wav_samp.sample(k);
		}
		auto spectrum = MFCC.calculate(Aquila::SignalSource(win, FREQ), MFCC_BINS);
		for(k = 0; k < MFCC_BINS; k++){
			img[k][frame] = spectrum[k];
		}
		frame++;
		//if(frame == IMG_WIDTH){
		//	j = j - winStep*IMG_OVERLAP;
		//	frame = 0;
		//}
	}

	double imin, imax;

	imin =  1e16;
	imax = -1e16;
	for(i = 0; i < MFCC_BINS; i++){
		for(j = 0; j < IMG_WIDTH; j++){
			if(isinf(img[i][j])){
				img[i][j] = 0;
			}

			imin = (img[i][j] < imin)? img[i][j]: imin;
			imax = (img[i][j] > imax)? img[i][j]: imax;
		}
	}

	for(i = 0; i < MFCC_BINS; i++){
		for(j = 0; j < IMG_WIDTH; j++){
			img[i][j] = (1 - (img[i][j] - imin) / (imax - imin)) * 255.0;
		}
	}
#endif

#ifdef Testaccuracy
	/////////////////Testaccuracy///////////////////
	float mean[64][400];
	float std[64][400];
	ifstream meancsv("mean.csv");
	ifstream stdcsv("std.csv");
	if(!meancsv.is_open())
	{
		printf("Error opening meancsv.csv.\n");
	}

	if(!stdcsv.is_open())
	{
		printf("Error opening stdcsv.csv.\n");
	}

	for(int row = 0; row < 64; row++)
	{
		string line1;
		getline(meancsv, line1);
		if(!meancsv.good())
		{
			break;		
		}
		stringstream iss1(line1);
		for(int col = 0; col< 400; col++)
		{
			string val1;
			getline(iss1, val1, ',');
			if(!iss1.good())
			{
				stringstream convertor1(val1);
				convertor1>>mean[row][col];
				break;
			}
			stringstream convertor1(val1);
			convertor1>>mean[row][col];
		}
	}

	for(int row = 0; row < 64; row++)
	{
		string line2;
		getline(stdcsv, line2);
		if(!stdcsv.good())
		{
			break;		
		}
		stringstream iss2(line2);
		for(int col = 0; col< 400; col++)
		{
			string val2;
			getline(iss2, val2, ',');
			if(!iss2.good())
			{
				stringstream convertor2(val2);
				convertor2>>std[row][col];
				std[row][col] = 1.0/sqrt(std[row][col]+1e-4);
				break;
			}
			stringstream convertor2(val2);
			convertor2>>std[row][col];
			std[row][col] = 1.0/sqrt(std[row][col]+1e-4);
		}
	}
	meancsv.close();
	stdcsv.close();
	//FILE *fileList;
	float top_1;
	static int32_t dataset = 1;
	static float match = 0;
	char wavName[96], wavPath[96], outPath[96];
	int32_t c, index;
	float fileCount = 6430;
	//int32_t data[64][400];
	float data[64][400];
	for(c = 0; c < fileCount; c++)
	{
		for(int row = 0; row < 64; row++)
		{
			for(int col = 0; col< 400; col++)
			{
				data[row][col] = 0.0;
			}
		}
#ifdef TestFix16
		sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/output16/%d/%d.csv", dataset, c+1);
#elif defined(TestFix32)
		sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/output32/%d/%d.csv", dataset, c+1);
#else
		sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/outputF/%d/%d.csv", dataset, c+1);
#endif
		ifstream fileList(wavPath);

		if(!fileList.is_open())
		{
			dataset++;
#ifdef TestFix16
			sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/output16/%d/%d.csv", dataset, c+1);
#elif defined(TestFix32)
			sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/output32/%d/%d.csv", dataset, c+1);
#else
			sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/outputF/%d/%d.csv", dataset, c+1);
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
						data[row][col] = ((data[row][col]-mean[row][col])*std[row][col]);
						break;
					}
					stringstream convertor(val);
					convertor>>data[row][col];
					data[row][col] = ((data[row][col]-mean[row][col])*std[row][col]);
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
						data[row][col] = ((data[row][col]-mean[row][col])*std[row][col]);
						break;
					}
					stringstream convertor(val);
					convertor>>data[row][col];
					data[row][col] = ((data[row][col]-mean[row][col])*std[row][col]);
					printf("Final: %f\n",data[row][col]);
				}
			}
			//printf("Final: %f\n",data[0][0]);
			//printf("Final: %f\n",data[10][100]);
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
			//printf("Input 80 \n");
			// Read input
			input = new T(T_FLOAT, MFCC_BINS, mfccR-mfccL, 1);
			for(i = 0; i < MFCC_BINS; i++){
				for(j = mfccL; j < mfccR; j++){
					input->f[i*input->Y+j-mfccL] = data[i][j];
				}
			}

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

		maximum = predict[0];

		for(i = 1; i < nClasses; i++){
			if(predict[i]>maximum)
			{
				maximum = predict[i];
				index = i+1;
			}
		}				
		printf("maximum index = %d\n", index);
		printf("dataset: %d\n", dataset);
		if(dataset == index)
		{
			match = match + 1;
		}

		top_1 = (float) (match/fileCount)*100;

		//Top-1 accuracy:
		printf("Top-1 accuracy : %f % \n", top_1);
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
		printf("Input 80 \n");
		// Read input
		input = new T(T_FLOAT, MFCC_BINS, mfccR-mfccL, 1);
		for(i = 0; i < MFCC_BINS; i++){
			for(j = mfccL; j < mfccR; j++){
				input->f[i*input->Y+j-mfccL] = mfcc[i][j];
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
