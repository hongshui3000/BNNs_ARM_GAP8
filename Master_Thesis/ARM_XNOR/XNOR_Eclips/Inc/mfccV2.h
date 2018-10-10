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

#ifndef __KWS_MFCCV2_H__
#define __KWS_MFCCV2_H__

#include "arm_math.h"
#include "string.h"
#include "util.h"
#include "arm_const_structs.h"
#include "arm_math.h"
#define SAMP_FREQ 16000
#define NUM_FBANK_BINS 24
#define LOW_FREQ 20
#define HIGH_FREQ 4000

#define M_2PI 6.28318530717958

class MFCC{
  private:
    int num_mfcc_bins;
    int frame_length;
    //int mfcc_dec_bits;
    float * frame;
    float * buffer;
    float * mel_energies;
    float * window_func;
    int32_t * fbank_filter_first;
    int32_t * fbank_filter_last;
    float ** mel_fbank;
    float * dct_matrix;
	//const arm_cfft_instance_f32 *S;
    arm_rfft_fast_instance_f32* rfft;
    float * create_dct_matrix(int32_t input_length, int32_t coefficient_count);
    float ** create_mel_fbank();

  public:
    MFCC(int num_mfcc_features, int frame_len);
    ~MFCC();
    void mfcc_compute(const float* data, float* mfcc_out);
};

#endif
