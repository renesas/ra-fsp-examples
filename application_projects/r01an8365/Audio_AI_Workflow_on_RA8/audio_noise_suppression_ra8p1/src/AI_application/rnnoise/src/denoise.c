/* Copyright (c) 2018 Gregor Richards
 * Copyright (c) 2017 Mozilla */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pmu_ethosu.h"
#include "model.h"
#include "model_io_data.h"
#include "arm_math.h"
#include "hal_data.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "kiss_fft.h"
#include "common.h"
#include <math.h>
#include "rnnoise.h"
#include "pitch.h"
#include "arch.h"
#include "rnn.h"
#include "rnn_data.h"
#include "SEGGER_SYSVIEW.h"
#include "common_utils.h"
#include "common_app.h"
#include "kernel_library_int.h"

#ifdef __arm__
// force using floating point in biquad, and for log10 and sqrt
#define double float
#define log10 log10f
#define sqrt sqrtf
#endif

#define WINDOW_SIZE (2*FRAME_SIZE)
#define FREQ_SIZE (FRAME_SIZE + 1)

#define PITCH_MIN_PERIOD 60U
#define PITCH_MAX_PERIOD 768U
#define PITCH_FRAME_SIZE 960U
#define PITCH_BUF_SIZE (PITCH_MAX_PERIOD+PITCH_FRAME_SIZE)

#define SQUARE(x) ((x)*(x))

#define NB_BANDS 22

#define CEPS_MEM 8
#define NB_DELTA_CEPS 6

#define NB_FEATURES (NB_BANDS+3*NB_DELTA_CEPS+2)


#ifndef TRAINING
#define TRAINING 0
#endif


/* The built-in model, used if no file is given as input */
extern const struct RNNModel rnnoise_model_orig;


static const opus_int16 eband5ms[] = {
/*0  200 400 600 800  1k 1.2 1.4 1.6  2k 2.4 2.8 3.2  4k 4.8 5.6 6.8  8k 9.6 12k 15.6 20k*/
  0,  1,  2,  3,  4,  5,  6,  7,  8, 10, 12, 14, 16, 20, 24, 28, 34, 40, 48, 60, 78, 100
};


typedef struct {
  int init;
  kiss_fft_state *kfft;
  float full_window[WINDOW_SIZE];
  float dct_table[NB_BANDS*NB_BANDS];
} CommonState;

struct DenoiseState {
  float analysis_mem[FRAME_SIZE];
  float cepstral_mem[CEPS_MEM][NB_BANDS];
  int memid;
  float synthesis_mem[FRAME_SIZE];
  float pitch_buf[PITCH_BUF_SIZE];
  float pitch_enh_buf[PITCH_BUF_SIZE];
  float last_gain;
  int last_period;
  float mem_hp_x[2];
  float lastg[NB_BANDS];
  RNNState rnn;
};

//DenoiseState st;
CommonState common;
/* No need to optimize with arm cmsis dsp */
static void compute_band_energy(float *bandE, const kiss_fft_cpx *X) {
  int i;
  float sum[NB_BANDS] = {0};
  for (i=0;i<NB_BANDS-1;i++)
  {
    int j;
    int band_size;
    band_size = (eband5ms[i+1]-eband5ms[i])<<FRAME_SIZE_SHIFT;
    for (j=0;j<band_size;j++) {
      float tmp;
      float frac = (float)j/(float)band_size;
      tmp = SQUARE(X[(eband5ms[i]<<FRAME_SIZE_SHIFT) + j].r);
      tmp += SQUARE(X[(eband5ms[i]<<FRAME_SIZE_SHIFT) + j].i);
      sum[i] += (1-frac)*tmp;
      sum[i+1] += frac*tmp;
    }
  }
  sum[0] *= 2;
  sum[NB_BANDS-1] *= 2;
  for (i=0;i<NB_BANDS;i++)
  {
    bandE[i] = sum[i];
  }
}

static void compute_band_corr(float *bandE, const kiss_fft_cpx *X, const kiss_fft_cpx *P) {
  int i;
  float sum[NB_BANDS] = {0};
  for (i=0;i<NB_BANDS-1;i++)
  {
    int j;
    int band_size;
    band_size = (eband5ms[i+1]-eband5ms[i])<<FRAME_SIZE_SHIFT;
    for (j=0;j<band_size;j++) {
      float tmp;
      float frac = (float)j/(float)band_size;
      tmp = X[(eband5ms[i]<<FRAME_SIZE_SHIFT) + j].r * P[(eband5ms[i]<<FRAME_SIZE_SHIFT) + j].r;
      tmp += X[(eband5ms[i]<<FRAME_SIZE_SHIFT) + j].i * P[(eband5ms[i]<<FRAME_SIZE_SHIFT) + j].i;
      sum[i] += (1-frac)*tmp;
      sum[i+1] += frac*tmp;
    }
  }
  sum[0] *= 2;
  sum[NB_BANDS-1] *= 2;
  for (i=0;i<NB_BANDS;i++)
  {
    bandE[i] = sum[i];
  }
}

static void interp_band_gain(float *g, const float *bandE) {
  int i,j;
  memset(g, 0, FREQ_SIZE);
  for (i=0;i<NB_BANDS-1;i++)
  {
    int band_size;
    band_size = (eband5ms[i+1]-eband5ms[i])<<FRAME_SIZE_SHIFT;
    for (j=0;j<band_size;j++) {
      float frac = (float)j/(float)band_size;
      g[(eband5ms[i]<<FRAME_SIZE_SHIFT) + j] = (1-frac)*bandE[i] + frac*bandE[i+1];
    }
  }
}

static void check_init() {
  uint32_t i;
  if (common.init) return;
  common.kfft = rnn_fft_alloc_twiddles(2*FRAME_SIZE, NULL, NULL, NULL, 0);
  for (i=0;i<FRAME_SIZE;i++)
  {
      float w = sinf(0.5f * M_PI *
                     sinf(0.5f * M_PI * ((float)i + 0.5f) / FRAME_SIZE) *
                     sinf(0.5f * M_PI * ((float)i + 0.5f) / FRAME_SIZE));
      common.full_window[i] = w;
      common.full_window[WINDOW_SIZE-1-i]= w;
  }
  for (i=0;i<NB_BANDS;i++) {
    uint32_t j;
    for (j=0;j<NB_BANDS;j++) {
      common.dct_table[i*NB_BANDS + j] = (float)cos((i+.5)*j*M_PI/NB_BANDS);
      if (j==0) common.dct_table[i*NB_BANDS + j] *= sqrt(.5);
    }
  }
  common.init = 1;
}

static void dct(float *out, const float *in) {
  int i;
  check_init();
  for (i=0;i<NB_BANDS;i++) {
    int j;
    float sum = 0;
    for (j=0;j<NB_BANDS;j++) {
      sum += in[j] * common.dct_table[j*NB_BANDS + i];
    }
    out[i] = sum*sqrt(2./22);
  }
}

#if 0
static void idct(float *out, const float *in) {
  int i;
  check_init();
  for (i=0;i<NB_BANDS;i++) {
    int j;
    float sum = 0;
    for (j=0;j<NB_BANDS;j++) {
      sum += in[j] * common.dct_table[i*NB_BANDS + j];
    }
    out[i] = sum*sqrt(2./22);
  }
}
#endif

static void forward_transform(kiss_fft_cpx *out, const float *in) {
  uint32_t i;
  kiss_fft_cpx x[WINDOW_SIZE];
  kiss_fft_cpx y[WINDOW_SIZE];
  check_init();
  for (i=0;i<WINDOW_SIZE;i++) {
    x[i].r = in[i];
    x[i].i = 0;
  }
  rnn_fft(common.kfft, x, y, 0);
  for (i=0;i<FREQ_SIZE;i++) {
    out[i] = y[i];
  }
}

static void inverse_transform(float *out, const kiss_fft_cpx *in) {
  uint32_t i;
  kiss_fft_cpx x[WINDOW_SIZE];
  kiss_fft_cpx y[WINDOW_SIZE];
  check_init();
  for (i=0;i<FREQ_SIZE;i++) {
    x[i] = in[i];
  }
  for (;i<WINDOW_SIZE;i++) {
    x[i].r = x[WINDOW_SIZE - i].r;
    x[i].i = -x[WINDOW_SIZE - i].i;
  }
  rnn_fft(common.kfft, x, y, 0);
  /* output in reverse order for IFFT. */
  out[0] = WINDOW_SIZE*y[0].r;
  for (i=1;i<WINDOW_SIZE;i++) {
    out[i] = WINDOW_SIZE*y[WINDOW_SIZE - i].r;
  }
}

static void apply_window(float *x) {

    check_init();
    arm_mult_f32(x,common.full_window,x, WINDOW_SIZE);
}

int rnnoise_get_size() {
  return sizeof(DenoiseState);
}

int rnnoise_get_frame_size() {
  return FRAME_SIZE;
}

int rnnoise_init(DenoiseState *st, RNNModel *model) {
  memset(st, 0, sizeof(*st));
#ifdef RUHMI_USED
  st->rnn.vad_gru_state = calloc(sizeof(int8_t), model_vad_gru_prev_state_int8_SIZE);
  st->rnn.noise_gru_state = calloc(sizeof(int8_t), model_noise_gru_prev_state_int8_SIZE);
  st->rnn.denoise_gru_state = calloc(sizeof(int8_t), model_denoise_gru_prev_state_int8_SIZE);
  memset(st->rnn.vad_gru_state, (-1), model_vad_gru_prev_state_int8_SIZE);
  memset(st->rnn.noise_gru_state, (-128), model_noise_gru_prev_state_int8_SIZE);
  memset(st->rnn.denoise_gru_state, (-1), model_denoise_gru_prev_state_int8_SIZE);
#else
  if (model)
    st->rnn.model = model;
  else
    st->rnn.model = &rnnoise_model_orig;
  st->rnn.vad_gru_state = calloc(sizeof(float), st->rnn.model->vad_gru_size);
  st->rnn.noise_gru_state = calloc(sizeof(float), st->rnn.model->noise_gru_size);
  st->rnn.denoise_gru_state = calloc(sizeof(float), st->rnn.model->denoise_gru_size);
#endif
  return 0;
}

DenoiseState *rnnoise_create(RNNModel *model) {
  DenoiseState *st;
  st = malloc(rnnoise_get_size());
  rnnoise_init(st, model);
  return st;
}

void rnnoise_destroy(DenoiseState *st) {
  free(st->rnn.vad_gru_state);
  free(st->rnn.noise_gru_state);
  free(st->rnn.denoise_gru_state);
  free(st);
}

#if TRAINING
int lowpass = FREQ_SIZE;
int band_lp = NB_BANDS;
#endif

static void frame_analysis(DenoiseState *st, kiss_fft_cpx *X, float *Ex, const float *in) {
  uint32_t i;

  float x[WINDOW_SIZE];
  RNN_COPY(x, st->analysis_mem, FRAME_SIZE);
  for (i=0;i<FRAME_SIZE;i++) x[FRAME_SIZE + i] = in[i];
  RNN_COPY(st->analysis_mem, in, FRAME_SIZE);


  apply_window(x);

  /* keep forward transform with kissfft */
  forward_transform(X, x);
#if TRAINING
  for (i=lowpass;i<FREQ_SIZE;i++)
    X[i].r = X[i].i = 0;
#endif
  compute_band_energy(Ex, X);
}

#ifdef RUHMI_USED

#define FEATURE_SIZE 42
#define Q_MIN (-128)
#define Q_MAX (127)


static inline int8_t clamp_int8(int x)
{
    if (x < Q_MIN) return Q_MIN;
    if (x > Q_MAX) return Q_MAX;
    return (int8_t)x;
}

void quantize_features_int8(const float *features,
                            int8_t *out_int8)
{
    const float scale = 0.22150060534477234f;
    const int8_t zero_point = 14;

    for (int i = 0; i < FEATURE_SIZE; i++) {
        float f = features[i] / scale;
        int q = (int)(round(f) + zero_point);
        out_int8[i] = clamp_int8(q);
    }
}


void dequantize_gain(const int8_t *gain_q,
                     float *gain_f)
{
    const float scale = 0.00390625f;
    const int zero_point = -128;

    for (int i = 0; i < NB_BANDS; i++) {
        gain_f[i] = ((float)gain_q[i] - zero_point) * scale;

        /* safety clamp */
        if (gain_f[i] < 0.f) gain_f[i] = 0.f;
        if (gain_f[i] > 1.f) gain_f[i] = 1.f;
    }
}


uint32_t GetCurrentTimeTicks() {
    static bool is_initialized = false;
    if (!is_initialized) {
        ARM_PMU_Enable();
        DCB->DEMCR |= DCB_DEMCR_TRCENA_Msk;
        ARM_PMU_CYCCNT_Reset();
        ARM_PMU_CNTR_Enable(PMU_CNTENSET_CCNTR_ENABLE_Msk);
        is_initialized = true;
    }
    return ARM_PMU_Get_CCNTR();
}


void run_rnn(RNNState *rnn, float *gains, float *vad, const int8_t *input)
{
//    static uint32_t cpuCyclesBegin = 0;
    const float gain_scale = 0.00390625f;
    const float gain_comp = 1.f;
    const float gain_floor = 0.f;

    uint32_t cyc_capture_frame_cnt = 0;

    uint32_t i;

    memcpy(GetModelInputPtr_main_input_int8(), input, model_main_input_int8_SIZE);

    memcpy(GetModelInputPtr_denoise_gru_prev_state_int8(), rnn->denoise_gru_state, model_denoise_gru_prev_state_int8_SIZE);

    memcpy(GetModelInputPtr_noise_gru_prev_state_int8(), rnn->noise_gru_state, model_noise_gru_prev_state_int8_SIZE);

    memcpy(GetModelInputPtr_vad_gru_prev_state_int8(), rnn->vad_gru_state, model_vad_gru_prev_state_int8_SIZE);

    ETHOSU_PMU_CYCCNT_Reset(&g_ethosu0);
    ETHOSU_PMU_EVCNTR_ALL_Reset(&g_ethosu0);

    uint32_t cpuCyclesBegin = GetCurrentTimeTicks();
    RunModel(false);
    uint32_t cpuCycles = GetCurrentTimeTicks() - cpuCyclesBegin;

    if (g_capture_npu_cyc)
    {
        cyc_capture_frame_cnt++;

        if (cyc_capture_frame_cnt >= CYCLE_CAPTURE_FRAMES)
        {
            uint32_t npu_idle = ETHOSU_PMU_Get_EVCNTR(&g_ethosu0, 0);
            uint32_t npu_active = ETHOSU_PMU_Get_EVCNTR(&g_ethosu0, 1);
            uint32_t npu_axi0_en = ETHOSU_PMU_Get_EVCNTR(&g_ethosu0, 2);
            uint32_t npu_axi1_en = ETHOSU_PMU_Get_EVCNTR(&g_ethosu0, 3);

            APP_PRINT("\r\nInference took %d cycles\r\n", cpuCycles);

            APP_PRINT("\r\nNPU: npu_idle: %d\r\n", npu_idle * 2);
            APP_PRINT("NPU: npu_active: %d\r\n", npu_active * 2);
            APP_PRINT("NPU: npu_axi0_en: %d\r\n", npu_axi0_en * 2);
            APP_PRINT("NPU: npu_axi1_en: %d\r\n", npu_axi1_en * 2);
            APP_PRINT("\r\nCPU part: %d\r\n", (cpuCycles - (npu_active * 2)));

            /* Reset for next capture request */
            cyc_capture_frame_cnt   = 0;
            npu_idle                = 0;
            npu_active              = 0;
            npu_axi0_en             = 0;
            npu_axi1_en             = 0;
            g_capture_npu_cyc       = false;
            g_npu_cyc_ready         = true;
        }
    }

    int8_t *gain_q = GetModelOutputPtr_Identity_1_int8_70152();

    for (i = 0; i < NB_BANDS; i++)
    {
        gains[i] = ((float)gain_q[i] - (-128)) * gain_scale;

        /* optional can remove */
        gains[i] *= gain_comp;

        if (gains[i] < gain_floor) gains[i] = 0.0f;
        if (gains[i] > 1.0f) gains[i] = 1.0f;
    }

    int8_t *vad_q = GetModelOutputPtr_Identity_4_int8_70156();

    *vad = ((float)vad_q[0] - (-128)) * gain_scale;

    if (*vad < 0.0f) *vad = 0.0f;
    if (*vad > 1.0f) *vad = 1.0f;

    memcpy(rnn->denoise_gru_state, GetModelOutputPtr_Identity_int8_70148(), model_denoise_gru_prev_state_int8_SIZE);

    memcpy(rnn->noise_gru_state, GetModelOutputPtr_Identity_2_int8_70118(), model_noise_gru_prev_state_int8_SIZE);

    memcpy(rnn->vad_gru_state, GetModelOutputPtr_Identity_3_int8_70089(), model_vad_gru_prev_state_int8_SIZE);
}
#endif

static int compute_frame_features(DenoiseState *st, kiss_fft_cpx *X, kiss_fft_cpx *P,
                                  float *Ex, float *Ep, float *Exp, float *features, const float *in) {
  int i;
  float E = 0;
  float *ceps_0, *ceps_1, *ceps_2;
  float spec_variability = 0;
  float Ly[NB_BANDS];
  float p[WINDOW_SIZE];
  float pitch_buf[PITCH_BUF_SIZE>>1];
  int pitch_index;
  float gain;
  float *(pre[1]);
  float tmp[NB_BANDS];
  float follow, logMax;
  frame_analysis(st, X, Ex, in);
  RNN_MOVE(st->pitch_buf, &st->pitch_buf[FRAME_SIZE], PITCH_BUF_SIZE-FRAME_SIZE);
  RNN_COPY(&st->pitch_buf[PITCH_BUF_SIZE-FRAME_SIZE], in, FRAME_SIZE);
  pre[0] = &st->pitch_buf[0];
  rnn_pitch_downsample(pre, pitch_buf, PITCH_BUF_SIZE, 1);
  rnn_pitch_search(pitch_buf+(PITCH_MAX_PERIOD>>1), pitch_buf, PITCH_FRAME_SIZE,
               PITCH_MAX_PERIOD-3*PITCH_MIN_PERIOD, &pitch_index);
  pitch_index = PITCH_MAX_PERIOD-pitch_index;

  gain = rnn_remove_doubling(pitch_buf, PITCH_MAX_PERIOD, PITCH_MIN_PERIOD,
          PITCH_FRAME_SIZE, &pitch_index, st->last_period, st->last_gain);
  st->last_period = pitch_index;
  st->last_gain = gain;
  const float *src = &st->pitch_buf[PITCH_BUF_SIZE-WINDOW_SIZE-pitch_index];
  arm_copy_f32(src, p, WINDOW_SIZE);
  apply_window(p);
  forward_transform(P, p);
  compute_band_energy(Ep, P);
  compute_band_corr(Exp, X, P);
  for (i=0;i<NB_BANDS;i++) Exp[i] = Exp[i]/sqrt(.001+Ex[i]*Ep[i]);
  dct(tmp, Exp);
  for (i=0;i<NB_DELTA_CEPS;i++) features[NB_BANDS+2*NB_DELTA_CEPS+i] = tmp[i];
  features[NB_BANDS+2*NB_DELTA_CEPS] -= 1.3;
  features[NB_BANDS+2*NB_DELTA_CEPS+1] -= 0.9;
  features[NB_BANDS+3*NB_DELTA_CEPS] = .01*(pitch_index-300);
  logMax = -2;
  follow = -2;
  for (i=0;i<NB_BANDS;i++) {
    Ly[i] = log10(1e-2+Ex[i]);
    Ly[i] = MAX16(logMax-7, MAX16(follow-1.5, Ly[i]));
    logMax = MAX16(logMax, Ly[i]);
    follow = MAX16(follow-1.5, Ly[i]);
    E += Ex[i];
  }
  if (!TRAINING && E < 0.04) {
    /* If there's no audio, avoid messing up the state. */
    RNN_CLEAR(features, NB_FEATURES);
    return 1;
  }
  dct(features, Ly);
  features[0] -= 12;
  features[1] -= 4;
  ceps_0 = st->cepstral_mem[st->memid];
  ceps_1 = (st->memid < 1) ? st->cepstral_mem[CEPS_MEM+st->memid-1] : st->cepstral_mem[st->memid-1];
  ceps_2 = (st->memid < 2) ? st->cepstral_mem[CEPS_MEM+st->memid-2] : st->cepstral_mem[st->memid-2];
  for (i=0;i<NB_BANDS;i++) ceps_0[i] = features[i];
  st->memid++;
  for (i=0;i<NB_DELTA_CEPS;i++) {
    features[i] = ceps_0[i] + ceps_1[i] + ceps_2[i];
    features[NB_BANDS+i] = ceps_0[i] - ceps_2[i];
    features[NB_BANDS+NB_DELTA_CEPS+i] =  ceps_0[i] - 2*ceps_1[i] + ceps_2[i];
  }
  /* Spectral variability features. */
  if (st->memid == CEPS_MEM) st->memid = 0;
  for (i=0;i<CEPS_MEM;i++)
  {
    int j;
    float mindist = 1e15f;
    for (j=0;j<CEPS_MEM;j++)
    {
      int k;
      float dist=0;
      for (k=0;k<NB_BANDS;k++)
      {
        float tmp;
        tmp = st->cepstral_mem[i][k] - st->cepstral_mem[j][k];
        dist += tmp*tmp;
      }
      if (j!=i)
        mindist = MIN32(mindist, dist);
    }
    spec_variability += mindist;
  }
  features[NB_BANDS+3*NB_DELTA_CEPS+1] = spec_variability/CEPS_MEM-2.1;
  return TRAINING && E < 0.1;
}

static void frame_synthesis(DenoiseState *st, float *out, const kiss_fft_cpx *y) {
  float x[WINDOW_SIZE];
  uint32_t i;
  inverse_transform(x, y);
  apply_window(x);
  for (i=0;i<FRAME_SIZE;i++) out[i] = x[i] + st->synthesis_mem[i];
  RNN_COPY(st->synthesis_mem, &x[FRAME_SIZE], FRAME_SIZE);
}

static void biquad(float *y, float mem[2], const float *x, const float *b, const float *a, int N) {
  int i;
  for (i=0;i<N;i++) {
    float xi, yi;
    xi = x[i];
    yi = x[i] + mem[0];
    mem[0] = mem[1] + (b[0]*(double)xi - a[0]*(double)yi);
    mem[1] = (b[1]*(double)xi - a[1]*(double)yi);
    y[i] = yi;
  }
}

static void pitch_filter(kiss_fft_cpx *X, const kiss_fft_cpx *P, const float *Ex, const float *Ep,
                  const float *Exp, const float *g) {
  int i;
  float r[NB_BANDS];
  float rf[FREQ_SIZE] = {0};
  float newE[NB_BANDS];
  float norm[NB_BANDS];
  float normf[FREQ_SIZE]={0};
  kiss_fft_cpx tmp[FREQ_SIZE];
  for (i=0;i<NB_BANDS;i++) {
#if 0
    if (Exp[i]>g[i]) r[i] = 1;
    else r[i] = Exp[i]*(1-g[i])/(.001 + g[i]*(1-Exp[i]));
    r[i] = MIN16(1, MAX16(0, r[i]));
#else
    if (Exp[i]>g[i]) r[i] = 1;
    else r[i] = SQUARE(Exp[i])*(1-SQUARE(g[i]))/(.001 + SQUARE(g[i])*(1-SQUARE(Exp[i])));
    r[i] = sqrt(MIN16(1, MAX16(0, r[i])));
#endif
    r[i] *= sqrt(Ex[i]/(1e-8+Ep[i]));
  }
  interp_band_gain(rf, r);

   for (i=0;i<FREQ_SIZE;i++) {
        X[i].r += rf[i]*P[i].r;
        X[i].i += rf[i]*P[i].i;
      }

  compute_band_energy(newE, X);
  for (i=0;i<NB_BANDS;i++) {
    norm[i] = sqrt(Ex[i]/(1e-8+newE[i]));
  }
  interp_band_gain(normf, norm);
  for (i=0;i<FREQ_SIZE;i++) {
    X[i].r *= normf[i];
    X[i].i *= normf[i];
  }

}

float rnnoise_process_frame(DenoiseState *st, float *out, const float *in) {
    int i;
    kiss_fft_cpx X[FREQ_SIZE];
    kiss_fft_cpx P[WINDOW_SIZE];
    float x[FRAME_SIZE];
    float Ex[NB_BANDS], Ep[NB_BANDS];
    float Exp[NB_BANDS];
    float features[NB_FEATURES];
    float g[NB_BANDS];
    float gf[FREQ_SIZE]={1};
    float vad_prob = 0;
    int silence;

    /* High-pass filter coefficient */
    static const float a_hp[2] = {-1.99599f, 0.99600f};
    static const float b_hp[2] = {-2, 1};

    /* Local accumulators — persist across calls */
    static uint32_t lat_frame_count = 0;
    static uint64_t lat_pre_sum     = 0;
    static uint64_t lat_rnnoise_sum = 0;
    static uint64_t lat_post_sum    = 0;

    /* Latency time get */
    uint32_t t0 = 0; /* pre-DSP latency  */
    uint32_t t1 = 0; /* pre-DSP latency  */
    uint32_t t2 = 0; /* rnnoise latency  */
    uint32_t t3 = 0; /* rnnoise latency  */
    uint32_t t4 = 0; /* post-DSP latency  */
    uint32_t t5 = 0; /* post-DSP latency  */

    /* Record a Performance Marker Start: ID = 0 */
    SEGGER_SYSVIEW_OnUserStart(0);
    t0 = DWT->CYCCNT;

    biquad(x, st->mem_hp_x, in, b_hp, a_hp, FRAME_SIZE);
    silence = compute_frame_features(st, X, P, Ex, Ep, Exp, features, x);

    t1 = DWT->CYCCNT; /* end of pre-DSP */

#ifdef RUHMI_USED
    int8_t    feature_int8[NB_FEATURES]   = {0};
    float     feature_scale               = 0.22150060534477234f;
    int8_t    zero_point                  = 14;

    /* Quantize feature from float to int8 */
    AffineQuantizeFloatToInt8(
        features,           // input float*
        feature_int8,       // output int8*
        NB_FEATURES,        // size
        zero_point,         // zero point
        feature_scale       // scale
    );
#endif

    /* Record a Performance Marker Stop: ID = 0 */
    SEGGER_SYSVIEW_OnUserStop(0);
    /* Record a Performance Marker Start: ID = 1 */
    SEGGER_SYSVIEW_OnUserStart(1);
    t2 = DWT->CYCCNT;

    if (!silence) {
        /* Execute Model */
#ifdef RUHMI_USED
        run_rnn(&st->rnn, g, &vad_prob, feature_int8);
#else
        compute_rnn(&st->rnn, g, &vad_prob, features);
#endif

        /* Record a Performance Marker Stop: ID = 1 */
        SEGGER_SYSVIEW_OnUserStop(1);
        /* Record a Performance Marker Start: ID = 2 */
        SEGGER_SYSVIEW_OnUserStart(2);
        t3 = DWT->CYCCNT;

        t4 = DWT->CYCCNT;
        pitch_filter(X, P, Ex, Ep, Exp, g);
        for (i=0;i<NB_BANDS;i++) {
            float alpha = .6f;
            g[i] = MAX16(g[i], alpha*st->lastg[i]);
            st->lastg[i] = g[i];
        }
        interp_band_gain(gf, g);
        for (i=0;i<FREQ_SIZE;i++) {
            X[i].r *= gf[i];
            X[i].i *= gf[i];
        }
    }

    frame_synthesis(st, out, X);
    t5 = DWT->CYCCNT;   /* end of post-DSP */

    /* Latency Measurement */
    if (g_measure_latency)
    {
        /* Latency accumulate */
        lat_pre_sum  += (uint64_t)(t1 - t0);
        lat_rnnoise_sum += (uint64_t)(t3 - t2);
        lat_post_sum += (uint64_t)(t5 - t4);
        lat_frame_count++;

        /* Calculate for 100 frames = 1 second */
        if (lat_frame_count >= LATENCY_MEASURE_FRAMES)
        {
            uint64_t clk = (uint64_t)SystemCoreClock;

            g_latency_us_pre  = (int16_t)((lat_pre_sum  / LATENCY_MEASURE_FRAMES * 1000000ULL) / clk);
            g_latency_us_rnnoise = (int16_t)((lat_rnnoise_sum / LATENCY_MEASURE_FRAMES * 1000000ULL) / clk);
            g_latency_us_post = (int16_t)((lat_post_sum / LATENCY_MEASURE_FRAMES * 1000000ULL) / clk);

            /* Reset for next measurement request */
            lat_frame_count     = 0;
            lat_pre_sum         = 0;
            lat_rnnoise_sum     = 0;
            lat_post_sum        = 0;
            g_measure_latency   = false;
            g_latency_ready     = true;
        }
    }
    /* Record a Performance Marker Stop: ID = 2 */
    SEGGER_SYSVIEW_OnUserStop(2);

    return vad_prob;
}

#if TRAINING

static float uni_rand() {
  return rand()/(double)RAND_MAX-.5;
}

static void rand_resp(float *a, float *b) {
  a[0] = .75*uni_rand();
  a[1] = .75*uni_rand();
  b[0] = .75*uni_rand();
  b[1] = .75*uni_rand();
}

int main(int argc, char **argv) {
  int i;
  int count=0;
  static const float a_hp[2] = {-1.99599, 0.99600};
  static const float b_hp[2] = {-2, 1};
  float a_noise[2] = {0};
  float b_noise[2] = {0};
  float a_sig[2] = {0};
  float b_sig[2] = {0};
  float mem_hp_x[2]={0};
  float mem_hp_n[2]={0};
  float mem_resp_x[2]={0};
  float mem_resp_n[2]={0};
  float x[FRAME_SIZE];
  float n[FRAME_SIZE];
  float xn[FRAME_SIZE];
  int vad_cnt=0;
  int gain_change_count=0;
  float speech_gain = 1, noise_gain = 1;
  FILE *f1, *f2;
  int maxCount;
  DenoiseState *st;
  DenoiseState *noise_state;
  DenoiseState *noisy;
  st = rnnoise_create(NULL);
  noise_state = rnnoise_create(NULL);
  noisy = rnnoise_create(NULL);
  if (argc!=4) {
    fprintf(stderr, "usage: %s <speech> <noise> <count>\n", argv[0]);
    return 1;
  }
  f1 = fopen(argv[1], "r");
  f2 = fopen(argv[2], "r");
  maxCount = atoi(argv[3]);
  for(i=0;i<150;i++) {
    short tmp[FRAME_SIZE];
    fread(tmp, sizeof(short), FRAME_SIZE, f2);
  }
  while (1) {
    kiss_fft_cpx X[FREQ_SIZE], Y[FREQ_SIZE], N[FREQ_SIZE], P[WINDOW_SIZE];
    float Ex[NB_BANDS], Ey[NB_BANDS], En[NB_BANDS], Ep[NB_BANDS];
    float Exp[NB_BANDS];
    float Ln[NB_BANDS];
    float features[NB_FEATURES];
    float g[NB_BANDS];
    short tmp[FRAME_SIZE];
    float vad=0;
    float E=0;
    if (count==maxCount) break;
    if ((count%1000)==0) fprintf(stderr, "%d\r", count);
    if (++gain_change_count > 2821) {
      speech_gain = pow(10., (-40+(rand()%60))/20.);
      noise_gain = pow(10., (-30+(rand()%50))/20.);
      if (rand()%10==0) noise_gain = 0;
      noise_gain *= speech_gain;
      if (rand()%10==0) speech_gain = 0;
      gain_change_count = 0;
      rand_resp(a_noise, b_noise);
      rand_resp(a_sig, b_sig);
      lowpass = FREQ_SIZE * 3000./24000. * pow(50., rand()/(double)RAND_MAX);
      for (i=0;i<NB_BANDS;i++) {
        if (eband5ms[i]<<FRAME_SIZE_SHIFT > lowpass) {
          band_lp = i;
          break;
        }
      }
    }
    if (speech_gain != 0) {
      fread(tmp, sizeof(short), FRAME_SIZE, f1);
      if (feof(f1)) {
        rewind(f1);
        fread(tmp, sizeof(short), FRAME_SIZE, f1);
      }
      for (i=0;i<FRAME_SIZE;i++) x[i] = speech_gain*tmp[i];
      for (i=0;i<FRAME_SIZE;i++) E += tmp[i]*(float)tmp[i];
    } else {
      for (i=0;i<FRAME_SIZE;i++) x[i] = 0;
      E = 0;
    }
    if (noise_gain!=0) {
      fread(tmp, sizeof(short), FRAME_SIZE, f2);
      if (feof(f2)) {
        rewind(f2);
        fread(tmp, sizeof(short), FRAME_SIZE, f2);
      }
      for (i=0;i<FRAME_SIZE;i++) n[i] = noise_gain*tmp[i];
    } else {
      for (i=0;i<FRAME_SIZE;i++) n[i] = 0;
    }
    biquad(x, mem_hp_x, x, b_hp, a_hp, FRAME_SIZE);
    biquad(x, mem_resp_x, x, b_sig, a_sig, FRAME_SIZE);
    biquad(n, mem_hp_n, n, b_hp, a_hp, FRAME_SIZE);
    biquad(n, mem_resp_n, n, b_noise, a_noise, FRAME_SIZE);
    for (i=0;i<FRAME_SIZE;i++) xn[i] = x[i] + n[i];
    if (E > 1e9f) {
      vad_cnt=0;
    } else if (E > 1e8f) {
      vad_cnt -= 5;
    } else if (E > 1e7f) {
      vad_cnt++;
    } else {
      vad_cnt+=2;
    }
    if (vad_cnt < 0) vad_cnt = 0;
    if (vad_cnt > 15) vad_cnt = 15;

    if (vad_cnt >= 10) vad = 0;
    else if (vad_cnt > 0) vad = 0.5f;
    else vad = 1.f;

    frame_analysis(st, Y, Ey, x);
    frame_analysis(noise_state, N, En, n);
    for (i=0;i<NB_BANDS;i++) Ln[i] = log10(1e-2+En[i]);
    int silence = compute_frame_features(noisy, X, P, Ex, Ep, Exp, features, xn);
    pitch_filter(X, P, Ex, Ep, Exp, g);
    //printf("%f %d\n", noisy->last_gain, noisy->last_period);
    for (i=0;i<NB_BANDS;i++) {
      g[i] = sqrt((Ey[i]+1e-3)/(Ex[i]+1e-3));
      if (g[i] > 1) g[i] = 1;
      if (silence || i > band_lp) g[i] = -1;
      if (Ey[i] < 5e-2 && Ex[i] < 5e-2) g[i] = -1;
      if (vad==0 && noise_gain==0) g[i] = -1;
    }
    count++;
#if 1
    fwrite(features, sizeof(float), NB_FEATURES, stdout);
    fwrite(g, sizeof(float), NB_BANDS, stdout);
    fwrite(Ln, sizeof(float), NB_BANDS, stdout);
    fwrite(&vad, sizeof(float), 1, stdout);
#endif
  }
  fprintf(stderr, "matrix size: %d x %d\n", count, NB_FEATURES + 2*NB_BANDS + 1);
  fclose(f1);
  fclose(f2);
  return 0;
}

#endif
