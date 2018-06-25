/**
 ******************************************************************************
 * @file    Templates/Src/main.c
 * @author  MCD Application Team
 * @version V1.0.3
 * @date    06-May-2016
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F4xx_HAL_Examples
 * @{
 */

/** @addtogroup Templates
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
//FATFS USBH_FatFs;
/* Private define ------------------------------------------------------------*/
//#define LTDC_BACKGROUND_LAYER   0
//#define LTDC_FOREGROUND_LAYER   1
/*!< Read cycle counter register */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//USBH_HandleTypeDef hUSBHost;
//AUDIO_ApplicationTypeDef AppliState = APPLICATION_IDLE;
//TS_StateTypeDef  TS_State = {0};
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */

#include <cmath>
#include <cstdint>

#include "model.h"
#include "tensor.h"

#include <cstdio>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
//#include <stdint.h>
#ifdef MFCC_PREPROCESS
#include <string.h>
#include "aquila/global.h"
#include "aquila/source/WaveFile.h"
#include "aquila/source/window/HammingWindow.h"
#include "aquila/transform/Mfcc.h"
//#include "aquila/tools/TextPlot.h"
#include "aquila/source/generator/SineGenerator.h"
using namespace std;
#elif defined(MFCC_V2)
#include "stdlib.h"
#include "mfccV2.h"
#endif

const int FREQ = 16000;
const int WIN_LENGTH = 256;
//const int WIN_OVERLAP = 384;
const int WIN_OVERLAP = 226;
#if defined(MFCC_PREPROCESS)||defined(MFCC_V2)
const int MFCC_BINS = 64;
const int IMG_WIDTH = 200;
float img[MFCC_BINS][IMG_WIDTH];

#else
#endif
const int IMG_OVERLAP = 200;
#include "ZZZZ.dat"

#include "layers/BinConvolution.h"
#include "layers/FirstLayer.h"
#include "layers/LastLayer.h"

//#include "defines.h"


/* Include arm_math.h mathematic functions */
#include "arm_const_structs.h"
#include "arm_math.h"

#ifdef MFCC_V2
/* FFT settings */
//#define SAMPLES                    (512)         /* 256 real party and 256 imaginary parts */
//#define FFT_SIZE                (WIN_LENGTH / 2) /* FFT size is always the same size as we have samples, so 256 in our case */
//float32_t Input[WIN_LENGTH]; /*!< Input buffer is always 2 * FFT_SIZE */
//float32_t Output[FFT_SIZE]; /*!< Output buffer is always FFT_SIZE */
#endif

#ifdef TestFix16
#include "input_fp16.dat"
#else
#ifdef MFCC_PREPROCESS

#elif defined(MFCC_V2)

#else
#include "input.dat"
#endif
#endif

//with 8 bit signGamma and signBetaStar, which are pre-calculated
#ifdef myPopcount_methode_2
#ifdef data_optimized
#ifdef with_sign_Gamma_negative
#ifdef TestFix16
#include "model_optimize_fp16.dat"
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

/* DWT (Data Watchpoint and Trace) registers, only exists on ARM Cortex with a DWT unit */
#define KIN1_DWT_CONTROL             (*((volatile uint32_t*)0xE0001000))
/*!< DWT Control register */
#define KIN1_DWT_CYCCNTENA_BIT       (1UL<<0)
/*!< CYCCNTENA bit in DWT_CONTROL register */
#define KIN1_DWT_CYCCNT              (*((volatile uint32_t*)0xE0001004))
/*!< DWT Cycle Counter register */
#define KIN1_DEMCR                   (*((volatile uint32_t*)0xE000EDFC))
/*!< DEMCR: Debug Exception and Monitor Control Register */
#define KIN1_TRCENA_BIT              (1UL<<24)
/*!< Trace enable bit in DEMCR register */

#define KIN1_InitCycleCounter() \
KIN1_DEMCR |= KIN1_TRCENA_BIT
/*!< TRCENA: Enable trace and debug block DEMCR (Debug Exception and Monitor Control Register */

#define KIN1_ResetCycleCounter() \
KIN1_DWT_CYCCNT = 0
/*!< Reset cycle counter */

#define KIN1_EnableCycleCounter() \
KIN1_DWT_CONTROL |= KIN1_DWT_CYCCNTENA_BIT
/*!< Enable cycle counter */

#define KIN1_DisableCycleCounter() \
KIN1_DWT_CONTROL &= ~KIN1_DWT_CYCCNTENA_BIT
/*!< Disable cycle counter */

#define KIN1_GetCycleCounter() \
KIN1_DWT_CYCCNT

//#define FLASH_ACCESS              (*((volatile uint32_t*)0x40023C10))
/* Flash acces? 0x8000 8000 : 0x8000 0000*/
//#define FLASH_ACCESS2              (*((volatile uint32_t*)0x40023C00))

//#define FLAAA       (1UL<<3)

//#define GetFlash() \
FLASH_ACCESS2 |= FLAAA

int main(void) {

	HAL_Init();



	/* Configure the system clock to 180 MHz */
	SystemClock_Config();

	/* Add your application code here
	 */
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED2);
	BSP_LED_Init(LED1);

	// Init LCD
	//TM_LCD_Init();
	/* Initialize ADC, PA0 is used */
//	TM_ADC_Init(ADC1, TM_ADC_Channel_0);
	BSP_LCD_Reset();

#ifdef LCD_USED
	BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	BSP_LCD_SelectLayer(0);
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(0, 0, 800, 480);
	//BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	//BSP_LCD_FillRect(0, 240, 800, 240);
	BSP_LCD_SetBackColor(LCD_COLOR_DARKGRAY);
	BSP_LCD_DisplayStringAtLine(1, (uint8_t *)"Classification:");
	BSP_LCD_SetFont(&Font16);
#endif

#if defined(STM32F429_DISCOVERY)
	// Rotate LCD to landscape mode
	TM_LCD_SetOrientation(2);
#endif
	uint32_t cycles; /* number of cycles */

	int32_t j, k;
	uint16_t i;

#ifdef MFCC_PREPROCESS
	{	int fileCount, c, winCount, winStep, frame;

		Aquila::HammingWindow hamming(WIN_LENGTH);
		Aquila::Mfcc MFCC(WIN_LENGTH);
		vector<Aquila::SampleType> wave_amp(WIN_LENGTH);

		winStep = WIN_LENGTH - WIN_OVERLAP;
		frame = 0;
		vector<Aquila::SampleType> win(WIN_LENGTH);
		for(j = 0; j < IMG_WIDTH*winStep; j += winStep) {
			for(k = 0; k < WIN_LENGTH; k++) {
				wave_amp[k] = wav_sample[j+k];
			}
			Aquila::SignalSource wav_samp(wave_amp);
			for(k = 0; k < WIN_LENGTH; k++) {
				win[k] = hamming.sample(k)*wav_samp.sample(k);
			}
			KIN1_InitCycleCounter(); /* enable DWT hardware */
			KIN1_ResetCycleCounter(); /* reset cycle counter */
			KIN1_EnableCycleCounter(); /* start counting */
			auto spectrum = MFCC.calculate(Aquila::SignalSource(win, FREQ), MFCC_BINS);
			cycles = KIN1_GetCycleCounter(); /* get cycle counter */
			KIN1_DisableCycleCounter(); /* disable counting if not used any more */
			for(k = 0; k < MFCC_BINS; k++) {
				img[k][frame] = spectrum[k];
			}
			frame++;
//   if(frame == IMG_WIDTH){
//   j = j - winStep*IMG_OVERLAP;
//   frame = 0;
//   }
//	cycles = KIN1_GetCycleCounter(); /* get cycle counter */
//	KIN1_DisableCycleCounter(); /* disable counting if not used any more */
		}

		//double imin, imax;
		float imin, imax;

		imin = 1e16;
		imax = -1e16;
//  KIN1_InitCycleCounter(); /* enable DWT hardware */
//  KIN1_ResetCycleCounter(); /* reset cycle counter */
//  KIN1_EnableCycleCounter(); /* start counting */
		for(i = 0; i < MFCC_BINS; i++) {
			for(j = 0; j < IMG_WIDTH; j++) {
				if(isinf(img[i][j])) {
					img[i][j] = 0;
				}

				imin = (img[i][j] < imin)? img[i][j]: imin;
				imax = (img[i][j] > imax)? img[i][j]: imax;
			}
		}

		for(i = 0; i < MFCC_BINS; i++) {
			for(j = 0; j < IMG_WIDTH; j++) {
				img[i][j] = (1 - (img[i][j] - imin) / (imax - imin)) * 255.0;
			}
		}

	}
#elif defined(MFCC_V2)
	//const arm_cfft_instance_f32 *S; /* ARM CFFT module */
	int winCount, winStep,width;
	float* frame = new float[WIN_LENGTH];
	winStep = WIN_LENGTH - WIN_OVERLAP;

	KIN1_InitCycleCounter(); /* enable DWT hardware */
	KIN1_ResetCycleCounter(); /* reset cycle counter */
	KIN1_EnableCycleCounter(); /* start counting */

	MFCC* MFCC_init = new MFCC(MFCC_BINS, WIN_LENGTH);
	float32_t* img_1d = new float[MFCC_BINS];
	width = 0;
	for (int j = 0; j < IMG_WIDTH * winStep; j += winStep) {
		/*Read input to L1*/
		for (i = 0; i < WIN_LENGTH; i++) {
			frame[i] = wav_sample[j + i];
		}

		MFCC_init->mfcc_compute(frame, img_1d);
		for (k = 0; k < MFCC_BINS; k++) {
			img[k][width] = img_1d[k];
		}
		width++;

	}

	float imin, imax;

	imin = 1e16;
	imax = -1e16;

	for(i = 0; i < MFCC_BINS; i++) {
		for(j = 0; j < IMG_WIDTH; j++) {
			if(isinf(img[i][j])) {
				img[i][j] = 0;
			}

			imin = (img[i][j] < imin)? img[i][j]: imin;
			imax = (img[i][j] > imax)? img[i][j]: imax;
		}
	}

	for(i = 0; i < MFCC_BINS; i++) {
		for(j = 0; j < IMG_WIDTH; j++) {
			img[i][j] = (1 - (img[i][j] - imin) / (imax - imin)) * 255.0;
		}
	}
	//delete []frame;
	delete []img_1d;
	delete MFCC_init;
	cycles = KIN1_GetCycleCounter(); /* get cycle counter */
	KIN1_DisableCycleCounter(); /* disable counting if not used any more */

	/* Initialize the CFFT/CIFFT module, intFlag = 0, doBitReverse = 1 */
//	arm_cfft_f32(S, frame, 0, 1);
	/* Process the data through the Complex Magniture Module for calculating the magnitude at each bin */
//	arm_cmplx_mag_f32(frame, Output, FFT_SIZE);
	/* Process the data through the CFFT/CIFFT module */
	//arm_cfft_f32(&S, Input);
#endif

	T *input, *output;

	const int nClasses = 28;
	double predict[nClasses];

#ifdef myPopcount_methode_2
	Model net(1, b32Data, b8Data, fData);
#elif defined(Popcount_sign_vectorized)
	Model net(1, b32Data, bSignData, fData);
#else
	Model net(1, bData, fData);
#endif

	//uint32_t *Test_var = (uint32_t*) 0x20001000;
	//*Test_var = 16;
	//uint32_t buf __attribute__((at(0x2000100C))) = 10;

	FirstLayer(&net, 32, 3, 3, 1, 1, 1, 1, 1e-5, 1e-4);
	BinConvolution(&net, 64, 3, 3, 2, 2, 1, 1, true, 1e-4);
	BinConvolution(&net, 128, 3, 3, 1, 1, 1, 1, true, 1e-4);
	BinConvolution(&net, 128, 3, 3, 2, 2, 1, 1, true, 1e-4);
	BinConvolution(&net, 128, 3, 3, 1, 1, 1, 1, true, 1e-4);
	LastLayer(&net, 128, nClasses, 1e-3);

	// Size of the subsection
#ifdef TestFix16
	const int32_t size = 80;
#else
	const int32_t size = 100;
#endif
	int32_t pos, mfccL, mfccR, invalidL, invalidR;
	float sum, predict_temp = 0;

	while (1) {

		for (i = 0; i < nClasses; i++) {
			predict[i] = 0;
		}

#ifdef TestFix16
		// Size of the subsection
		for(pos = 0; pos < IMG_WIDTH; pos += size) {
			// Pad input from the left and right
			if(pos == 0) {
				mfccL = 0;
				invalidL = 0;
			} else {
				mfccL = pos-12;
				invalidL = 3;
			}

			if(pos+size >= IMG_WIDTH) {
				mfccR = IMG_WIDTH;
				invalidR = 0;
			} else {
				mfccR = pos+size+8;
				invalidR = 2;
			}

			// Read input
			input = new T(T_BINARY, MFCC_BINS, mfccR-mfccL, 1);
			for(i = 0; i < MFCC_BINS; i++) {
				for(j = mfccL; j < mfccR; j++) {
					input->b[i*input->Y+j-mfccL] = mfcc[i][j];
				}
			}
#else

		for (pos = 0; pos < IMG_WIDTH; pos += size) {
			// Pad input from the left and right
			if (pos == 0) {
				mfccL = 0;
				invalidL = 0;
			} else {
				mfccL = pos - 12;
				invalidL = 3;
			}

			if (pos + size >= IMG_WIDTH) {
				mfccR = IMG_WIDTH;
				invalidR = 0;
			} else {
				mfccR = pos + size + 8;
				invalidR = 2;
			}

			// Read input
			input = new T(T_FLOAT, MFCC_BINS, mfccR - mfccL, 1);
			for (i = 0; i < MFCC_BINS; i++) {
				for (j = mfccL; j < mfccR; j++) {
#ifdef MFCC_PREPROCESS
					input->f[i*input->Y+j-mfccL] = img[i][j];
#elif defined(MFCC_V2)
					input->f[i * input->Y + j - mfccL] = img[i][j];
#else
					input->f[i*input->Y+j-mfccL] = mfcc[i][j];
#endif
				}
			}
#endif
			// Forward pass

			output = net.forward(input);

			// Accumulate the result

			for (k = 0; k < output->Z; k++) {
				//predict_temp = 0;
				for (i = 0; i < output->X; i++) {
					for (j = invalidL; j < output->Y - invalidR; j++) {
						predict[k] += output->f[(i * output->Y + j) * output->Z
								+ k];
						//predict_temp = predict_temp + output->f[(i*output->Y+j)*output->Z+k];
					}
				}
				//predict[k] += predict_temp;
			}

			delete output;
		}

		// AveragePool
		for (i = 0; i < nClasses; i++) {
			predict[i] = predict[i] / (16 * 100);
		}

		// LogSoftMax
		sum = 0;
		for (i = 0; i < nClasses; i++) {
			sum += expf(predict[i]);
		}

		sum = 1 / sum;

		for (i = 0; i < nClasses; i++) {
			predict[i] = logf(sum * expf(predict[i]));
		}
#ifdef LCD_USED
		char class1[32]= {};
		char class2[32]= {};
		char class3[32]= {};
		char class4[32]= {};
		char class5[32]= {};
		char class6[32]= {};
		char class7[32]= {};
		char class8[32]= {};
		char class9[32]= {};
		sprintf(class1, "Class 1: %8.4f", predict[0]);
		sprintf(class2, "Class 2: %8.4f", predict[1]);
		sprintf(class3, "Class 3: %8.4f", predict[2]);
		sprintf(class4, "Class 4: %8.4f", predict[3]);
		sprintf(class5, "Class 5: %8.4f", predict[4]);
		sprintf(class6, "Class 6: %8.4f", predict[5]);
		sprintf(class7, "Class 7: %8.4f", predict[6]);
		sprintf(class8, "Class 8: %8.4f", predict[7]);
		sprintf(class9, "Class 9: %8.4f", predict[8]);
		BSP_LCD_DisplayStringAtLine(2, (uint8_t *) class1);
		BSP_LCD_DisplayStringAtLine(3, (uint8_t *) class2);
		BSP_LCD_DisplayStringAtLine(4, (uint8_t *) class3);
		BSP_LCD_DisplayStringAtLine(5, (uint8_t *) class4);
		BSP_LCD_DisplayStringAtLine(6, (uint8_t *) class5);
		BSP_LCD_DisplayStringAtLine(7, (uint8_t *) class6);
		BSP_LCD_DisplayStringAtLine(8, (uint8_t *) class7);
		BSP_LCD_DisplayStringAtLine(9, (uint8_t *) class8);
		BSP_LCD_DisplayStringAtLine(10, (uint8_t *) class9);
#endif
		cycles = KIN1_GetCycleCounter(); /* get cycle counter */
		KIN1_DisableCycleCounter(); /* disable counting if not used any more */

//    BSP_LED_Toggle(LED1);
//    BSP_LED_Toggle(LED2);
//    BSP_LED_Toggle(LED3);
//    HAL_Delay(1000);
//    BSP_LED_Toggle(LED1);
//    BSP_LED_Toggle(LED2);
//    BSP_LED_Toggle(LED3);
#ifdef LCD_USED
//    BSP_LCD_Init();
//    BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
//    BSP_LCD_SelectLayer(0);
//    BSP_LCD_Clear(LCD_COLOR_WHITE);
		BSP_LCD_SetFont(&Font16);
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		BSP_LCD_FillRect(0, 0, 800, 480);
		BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
		//  BSP_LCD_FillRect(0, 240, 800, 240);
		//  BSP_LCD_SetBackColor(LCD_COLOR_DARKGRAY);
		BSP_LCD_DisplayStringAtLine(1, (uint8_t *)"Classification:");
		BSP_LCD_SetFont(&Font16);
#endif
	}

}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 180000000
 *            HCLK(Hz)                       = 180000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 8000000
 *            PLL_M                          = 8
 *            PLL_N                          = 360
 *            PLL_P                          = 2
 *            PLL_Q                          = 7
 *            PLL_R                          = 6
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 5
 * @param  None
 * @retval None
 */
static void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE()
	;

	/* The voltage scaling allows optimizing the power consumption when the device is
	 clocked below the maximum system frequency, to update the voltage scaling value
	 regarding system frequency refer to product datasheet.  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
#if defined(USE_STM32469I_DISCO_REVA)
	RCC_OscInitStruct.PLL.PLLM = 25;
#else
	RCC_OscInitStruct.PLL.PLLM = 8;
#endif /* USE_STM32469I_DISCO_REVA */
	RCC_OscInitStruct.PLL.PLLN = 360;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	RCC_OscInitStruct.PLL.PLLR = 6;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/* Enable the OverDrive to reach the 180 Mhz Frequency */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
		Error_Handler();
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
static void Error_Handler(void) {
	/* User may add here some code to deal with this error */
	while (1) {
	}
}

#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
