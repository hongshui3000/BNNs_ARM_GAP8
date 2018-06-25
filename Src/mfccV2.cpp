/*
 * Copyright (C) 2018 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Description: MFCC feature extraction to match with TensorFlow MFCC Op
 */

#include <string.h>
#include <stdint.h>
#include "mfccV2.h"
#include "float.h"

MFCC::MFCC(int num_mfcc_features, int frame_len) //:
		//num_mfcc_features(num_mfcc_features), frame_len(frame_len)
		{

	num_mfcc_bins = num_mfcc_features;
	frame_length = frame_len;
	frame = new float[frame_len];
	buffer = new float[frame_len];
	mel_energies = new float[NUM_FBANK_BINS];

	//create window function
	window_func = new float[frame_len];
	for (int i = 0; i < frame_len; i++) {
		window_func[i] = 0.53836
				- 0.46164 * cos(M_2PI * ((float) i) / (frame_len));
	}
	//create mel filterbank
	fbank_filter_first = new int32_t[NUM_FBANK_BINS];
	fbank_filter_last = new int32_t[NUM_FBANK_BINS];

	mel_fbank = create_mel_fbank();

	//create DCT matrix
	dct_matrix = create_dct_matrix(NUM_FBANK_BINS, num_mfcc_features);

	//initialize FFT
	//S = &arm_cfft_sR_f32_len256;
	rfft = new arm_rfft_fast_instance_f32;
	arm_rfft_fast_init_f32(rfft, frame_len);

}

MFCC::~MFCC() {
	delete[] frame;
	delete[] buffer;
	delete[] mel_energies;
	delete[] window_func;
	delete[] fbank_filter_first;
	delete[] fbank_filter_last;
	delete[] dct_matrix;
	//delete S;
	delete rfft;
	for (int i = 0; i < NUM_FBANK_BINS; i++)
		delete mel_fbank[i];
	delete mel_fbank;
}

float * MFCC::create_dct_matrix(int32_t input_length,
		int32_t coefficient_count) {
	int32_t k, n;
	float * M = new float[input_length * coefficient_count];
	float normalizer1, normalizer2;
	arm_sqrt_f32(1.0 / (float) input_length, &normalizer1);
	arm_sqrt_f32(2.0 / (float) input_length, &normalizer2);

	for (n = 0; n < input_length; n++) {
		k = 0;
		M[k * input_length + n] = normalizer1
				* cos((((float) M_PI) * (n + 0.5) * k) / input_length);
	}

	for (k = 1; k < coefficient_count; k++) {
		for (n = 0; n < input_length; n++) {
			M[k * input_length + n] = normalizer2
					* cos((((float) M_PI) * (n + 0.5) * k) / input_length);
		}
	}
	return M;
}

float ** MFCC::create_mel_fbank() {

	int32_t bin, i;
	int32_t num_fft_bins = frame_length;
	//float fft_bin_width = ((float) SAMP_FREQ) / num_fft_bins;
	float mel_freq_delta = 200;
	//float *this_bin = new float[num_fft_bins];
	float ** mel_fbank = new float*[NUM_FBANK_BINS];
	int j;
	for (bin = 0; bin < NUM_FBANK_BINS; bin++) {
		float left_mel = bin * mel_freq_delta / 2.0;
		float center_mel = left_mel + mel_freq_delta / 2.0;
		float right_mel = left_mel + mel_freq_delta;
		float low_freq = 700.0 * (expf(left_mel / 1127.0) - 1.0);//meltoLinear
		float center_freq = 700.0 * (expf(center_mel / 1127.0) - 1.0);//meltoLinear
		float high_freq = 700.0 * (expf(right_mel / 1127.0) - 1.0);	//meltoLinear
		int minPos = (num_fft_bins * low_freq / SAMP_FREQ);
		int maxPos = (num_fft_bins * high_freq / SAMP_FREQ);
		// limit maxPos not to write out of bounds of vector storage
		maxPos = (maxPos < num_fft_bins - 1) ? maxPos : (num_fft_bins - 1);
		mel_fbank[bin] = new float[maxPos - minPos + 1];
		j = 0;
		for (int k = minPos; k <= maxPos; ++k) {
			float currentFreq = (float) (k * SAMP_FREQ) / num_fft_bins;
			if (currentFreq < center_freq && currentFreq > low_freq) {
				// in the triangle on the ascending slope
				mel_fbank[bin][j++] = (currentFreq - low_freq)
						/ (center_freq - low_freq);
			} else if (currentFreq > center_freq && currentFreq < high_freq) {
				// in the triangle on the descending slope
				mel_fbank[bin][j++] = (high_freq - currentFreq)
						/ (high_freq - center_freq);
			} else{
				mel_fbank[bin][j++] = 0;
			}
		}
		fbank_filter_first[bin] = minPos;
		fbank_filter_last[bin] = maxPos;
	}
	return mel_fbank;
}

void MFCC::mfcc_compute(const float32_t * audio_data, float32_t* mfcc_out) {

	int32_t i, j, bin;

//TensorFlow way of normalizing .wav data to (-1,1)
	for (i = 0; i < frame_length; i++) {
		//frame[i] = (float)audio_data[i]/(1<<15);
		frame[i] = (float) audio_data[i];

	}
//Fill up remaining with zeros
//memset(&frame[frame_length], 0, sizeof(float) * (frame_length - frame_length));

	for (i = 0; i < frame_length; i++) {
		frame[i] *= window_func[i];
	}
	/* Initialize the CFFT/CIFFT module, intFlag = 0, doBitReverse = 1 */
	//Compute FFT
	arm_rfft_fast_f32(rfft, frame, buffer, 0);
	//arm_cfft_f32(S, frame, 0, 1);
	/* Process the data through the Complex Magniture Module for calculating the magnitude at each bin */
	//arm_cmplx_mag_f32(frame, buffer, frame_length / 2);
	//Convert to power spectrum
	//frame is stored as [real0, realN/2-1, real1, im1, real2, im2, ...]
	int32_t half_dim = frame_length / 2;
	 float first_energy = buffer[0] * buffer[0], last_energy = buffer[1]
	 * buffer[1];  // handle this special case
	 for (i = 1; i < half_dim; i++) {
	 float real = buffer[i * 2], im = buffer[i * 2 + 1];
	 buffer[i] = real * real + im * im;
	 }
	 buffer[0] = first_energy;
	 buffer[half_dim] = last_energy;

	float sqrt_data;
	//Apply mel filterbanks
	for (bin = 0; bin < NUM_FBANK_BINS; bin++) {
		j = 0;
		float mel_energy = 0;
		int32_t first_index = fbank_filter_first[bin];
		int32_t last_index = fbank_filter_last[bin];
		for (i = first_index; i <= last_index; ++i) {
			arm_sqrt_f32(buffer[i], &sqrt_data);
			mel_energy += (sqrt_data) * mel_fbank[bin][j++];
		}
		mel_energies[bin] = mel_energy;

		//avoid log of zero
		if (mel_energy <= 0.0)
			mel_energies[bin] = FLT_MIN;
	}

//Take log
//	for (bin = 0; bin < NUM_FBANK_BINS; bin++)
//		mel_energies[bin] = logf(mel_energies[bin]);

//Take DCT. Uses matrix mul.
	float32_t sum;
	for (i = 0; i < num_mfcc_bins; i++) {
		sum = 0;
		for (j = 0; j < NUM_FBANK_BINS; j++) {
			sum += dct_matrix[i * NUM_FBANK_BINS + j] * mel_energies[j];
		}

		mfcc_out[i] = sum;
	}

}
