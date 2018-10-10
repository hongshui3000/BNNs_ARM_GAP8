#include <cstdio>
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


const int FREQ = 16000;
const int WIN_LENGTH = 512;
const int WIN_OVERLAP = 384;
//const int WIN_LENGTH = 256;
//const int WIN_OVERLAP = 128;
const int MFCC_BINS = 64;
const int IMG_WIDTH = 400;
const int IMG_OVERLAP = 200;
//#define FIXPOINT16
//#define FIXPOINT32
#define FLOAT
#define TEST_DATASET
#define TRAIN_DATASET

void makeCSV(char *csvPath, float img[MFCC_BINS][IMG_WIDTH]){

  int i, j;
  static int n = 1;
  
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
  printf("imin: %f, imax: %f \n", imin, imax);
  
  float mean_img = 0;
  for(i = 0; i < MFCC_BINS; i++){
    for(j = 0; j < IMG_WIDTH; j++){
      //img[i][j] = (1 - (img[i][j] - imin) / (imax - imin));
	     img[i][j] = (1 - (img[i][j] - imin) / (imax - imin)) * 255.0;
      //printf("img[%d][%d] = %f \n", i,j,img[i][j]);
	    //mean_img +=img[i][j];
    }
  }
  //mean_img = mean_img/(MFCC_BINS*IMG_WIDTH);
  char csvFile[96];
  sprintf(csvFile, "%s%d.csv", csvPath, n);
  FILE *fp = fopen(csvFile, "w");

///fixpoint16///
#ifdef FIXPOINT16
  int32_t fixpoint;
  uint32_t checkvalue;
  int32_t SCALE = 256;
  for(i = 0; i < MFCC_BINS; i++){
    for(j = 0; j < IMG_WIDTH-1; j++){
	    //fixpoint = (int) (SCALE*img[i][j]);
	    fixpoint = (int) img[i][j];
		  checkvalue = fixpoint & 0xFFFF0000;
		  if((checkvalue > 0 ) && (checkvalue != 0xFFFF0000))
		  {
		  	printf("16 bit overflow: %x \n", fixpoint);
		  }
	    fprintf(fp, "%d,", fixpoint);
	    img[i][j] = 0;
    }
    //fixpoint = (int) (SCALE*img[i][j]);
    fixpoint = (int) img[i][j];
    fprintf(fp, "%d\n", fixpoint);
    img[i][j] = 0;
  }

#elif defined(FIXPOINT32)
///fixpoint32///
  int64_t fixpoint;
  uint64_t checkvalue;
  int32_t SCALE = 1048576;
  for(i = 0; i < MFCC_BINS; i++){
    for(j = 0; j < IMG_WIDTH-1; j++){
	    fixpoint = (int64_t) (SCALE*img[i][j]);
	    checkvalue = fixpoint & 0xFFFFFFFF00000000;
	    if((checkvalue > 0 ) && (checkvalue != 0xFFFFFFFF00000000))
	    {
		printf("32 bit overflow: %x \n", (int) (fixpoint>>32));
	    }
	    fprintf(fp, "%d,", fixpoint);
	    img[i][j] = 0;
    }
    fixpoint = (int64_t) (SCALE*img[i][j]);
    fprintf(fp, "%d\n", fixpoint);
    img[i][j] = 0;
  }

#else
///floating point//////////////////

  for(i = 0; i < MFCC_BINS; i++){
    for(j = 0; j < IMG_WIDTH-1; j++){
	    //img[i][j] = img[i][j] - mean_img;
	    fprintf(fp, "%f,", img[i][j]);
	    img[i][j] = 0;
    }
    //img[i][j] = img[i][j] - mean_img;
    fprintf(fp, "%f\n", img[i][j]);
    img[i][j] = 0;
  }
#endif
//////////////////////////////
  fclose(fp);
  
  n++;
  
}

int main(){

  int i, j, k;
  #ifdef TEST_DATASET
  FILE *fileList = fopen("test.txt", "r");
  #elif defined(TRAIN_DATASET)
  FILE *fileList = fopen("train.txt", "r");
  #endif
  if(!fileList)
  {
	printf("Failed to open file\n");
	exit(1);
  }

  char wavName[130], wavPath[130], outPath[130];
  int fileCount, c, winCount, winStep, frame;
  static float img[MFCC_BINS][IMG_WIDTH];
  
  Aquila::HammingWindow hamming(WIN_LENGTH);
  Aquila::Mfcc mfcc(WIN_LENGTH);
  
  fscanf(fileList, "%d", &fileCount);
  printf("%d\n",fileCount);
  for(i = 0; i < fileCount; i++){
    fscanf(fileList, "%s %d", wavName, &c);
    printf("%s, %d\n", wavName, c);
//    sprintf(wavPath, "../../../../../../../scratch/sem17f11/dataset/AudioEventDataset/test/%s", wavName);
//test data
    #ifdef TEST_DATASET
    sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/test/%s", wavName);
    #elif defined(TRAIN_DATASET)
//train data
    sprintf(wavPath, "/scratch/sem17f11/dataset/AudioEventDataset/train/%s", wavName);
    #endif

    ///select output PATH
//fixpoint 16
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutf16/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TrainOutf16/%d_", c + 1);    
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutf16_7/%d_", c + 1);    
//fixpoint 32
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutf32/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TrainOutf32/%d_", c + 1);    
//floating point
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TrainOutF/%d_", c + 1);    
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TrainOutF_Inv/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TrainOutF_Trunc/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TrainOutF_NFilter_40/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TrainOutF_Mean/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TrainOutF_Norm/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TrainOutF_Win256_Overlap128/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TrainOutF_NoNorm/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_Win256_Overlap128/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_Norm/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_NoNorm/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_NFilter_40/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_Log_Mean/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_Inv/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_Trunc/%d_", c + 1);
//    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_Trun_No256/%d_", c + 1);
    sprintf(outPath, "/scratch/sem17f11/dataset/AudioEventDataset/TestOutF_No256/%d_", c + 1);
    Aquila::WaveFile wav(wavPath);
    winStep = WIN_LENGTH - WIN_OVERLAP;
    winCount = (wav.getSamplesCount() - WIN_LENGTH) / winStep;
    
    frame = 0;
    for(j = 0; j < wav.getSamplesCount() - WIN_LENGTH; j += winStep){
	    vector<Aquila::SampleType> win(WIN_LENGTH);
	    for(k = 0; k < WIN_LENGTH; k++){
        win[k] = hamming.sample(k)*wav.sample(j+k);
	    }
      
	    auto spectrum = mfcc.calculate(Aquila::SignalSource(win, FREQ), MFCC_BINS);
	    
	    for(k = 0; k < MFCC_BINS; k++){
        img[k][frame] = spectrum[k];
	    }
      
	    frame++;
	    
	    if(frame == IMG_WIDTH){
        makeCSV(outPath, img);
        j = j - winStep*IMG_OVERLAP;
        frame = 0;
	    }
    }
    
    if(frame > 0){
    	    makeCSV(outPath, img);
    }
  }
  
  fclose(fileList);
 
  return 0;
}
