/* ----------------------------------------------------------------------
* Copyright (C) 2010-2012 ARM Limited. All rights reserved.
*
* $Date:         17. January 2013
* $Revision:     V1.4.0
*
* Project:       CMSIS DSP Library
* Title:	     arm_fft_bin_example_f32.c
*
* Description:   Example code demonstrating calculation of Max energy bin of
*                frequency domain of input signal.
*
* Target Processor: Cortex-M4/Cortex-M3
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*   - Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   - Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.
*   - Neither the name of ARM LIMITED nor the names of its contributors
*     may be used to endorse or promote products derived from this
*     software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
 * -------------------------------------------------------------------- */

/**
 * @addtogroup groupExamples
 * @{
 *
 * @defgroup FrequencyBin Frequency Bin Example
 *
 * \par Description
 * \par
 * Demonstrates the calculation of the maximum energy bin in the frequency
 * domain of the input signal with the use of Complex FFT, Complex
 * Magnitude, and Maximum functions.
 *
 * \par Algorithm:
 * \par
 * The input test signal contains a 10 kHz signal with uniformly distributed white noise.
 * Calculating the FFT of the input signal will give us the maximum energy of the
 * bin corresponding to the input frequency of 10 kHz.
 *
 * \par Block Diagram:
 * \image html FFTBin.gif "Block Diagram"
 * \par
 * The figure below shows the time domain signal of 10 kHz signal with
 * uniformly distributed white noise, and the next figure shows the input
 * in the frequency domain. The bin with maximum energy corresponds to 10 kHz signal.
 * \par
 * \image html FFTBinInput.gif "Input signal in Time domain"
 * \image html FFTBinOutput.gif "Input signal in Frequency domain"
 *
 * \par Variables Description:
 * \par
 * \li \c testInput_f32_10khz points to the input data
 * \li \c testOutput points to the output data
 * \li \c fftSize length of FFT
 * \li \c ifftFlag flag for the selection of CFFT/CIFFT
 * \li \c doBitReverse Flag for selection of normal order or bit reversed order
 * \li \c refIndex reference index value at which maximum energy of bin ocuurs
 * \li \c testIndex calculated index value at which maximum energy of bin ocuurs
 *
 * \par CMSIS DSP Software Library Functions Used:
 * \par
 * - arm_cfft_f32()
 * - arm_cmplx_mag_f32()
 * - arm_max_f32()
 *
 * <b> Refer  </b>
 * \link arm_fft_bin_example_f32.c \endlink
 *
 * \example arm_fft_bin_example_f32.c
 *
 * @}*/

#include "arm_math.h"
#include "arm_const_structs.h"
#include "common_utils.h"

#if defined(SEMIHOSTING)
#include <stdio.h>
#endif

#define TEST_LENGTH_SAMPLES 2048

/* -------------------------------------------------------------------
* External Input and Output buffer Declarations for FFT Bin Example
* ------------------------------------------------------------------- */
extern float32_t testInput_f32_10khz[TEST_LENGTH_SAMPLES];
static float32_t testOutput[TEST_LENGTH_SAMPLES/2];

/* ------------------------------------------------------------------
* Global variables for FFT Bin Example
* ------------------------------------------------------------------- */
uint32_t fftSize = 1024;
uint8_t ifftFlag = 0;
uint8_t doBitReverse = 1;
arm_cfft_instance_f32 varInstCfftF32;

uint32_t fs = 10000;

/* Reference index at which max energy of bin ocuurs */
uint32_t refIndex = 213, testIndex = 0;

/* ----------------------------------------------------------------------
* Max magnitude FFT Bin test
* ------------------------------------------------------------------- */

int32_t main_arm_fft_bin_example_f32(void)
{
  arm_status status;
  float32_t maxValue;

  status = ARM_MATH_SUCCESS;

  status=arm_cfft_init_1024_f32(&varInstCfftF32);

  // Enable DWT and the cycle counter if not
  if((CoreDebug_DEMCR_TRCENA_Msk & CoreDebug->DEMCR) == 0)
  {
      CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;     // Enable tracing
      DWT->CYCCNT = 0;                                    
      DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  }

  uint32_t start = DWT->CYCCNT;

  /* Process the data through the CFFT/CIFFT module */
  arm_cfft_f32(&varInstCfftF32, testInput_f32_10khz, ifftFlag, doBitReverse);

  /* Process the data through the Complex Magnitude Module for
  calculating the magnitude at each bin */
  arm_cmplx_mag_f32(testInput_f32_10khz, testOutput, fftSize);

  /* Calculates maxValue and returns corresponding BIN value */
  arm_max_f32(testOutput, fftSize, &maxValue, &testIndex);

  uint32_t end = DWT->CYCCNT;
  uint32_t elapsed_cycles = end - start;
  float32_t time_us = ((float32_t)elapsed_cycles) / ((float32_t)SystemCoreClock / 1e6f);

  status = (testIndex != refIndex) ? ARM_MATH_TEST_FAILURE : ARM_MATH_SUCCESS;
  if (status != ARM_MATH_SUCCESS)
  {
#if defined (SEMIHOSTING)
    printf("FAILURE\n");
#elif defined (SERIAL_LOG)
      APP_PRINT("ARM_FFT_BIN_EXAMPLE: FAILURE\r\n");
#else
    while (1);                             /* main function does not return */
#endif
  }
  else
  {
#if defined (SEMIHOSTING)
    printf("SUCCESS\n");
#elif defined (SERIAL_LOG)
    APP_PRINT(LOG_INFO, fftSize, fs, (fs / fftSize), testIndex,(float32_t)(testIndex * fs / fftSize), maxValue, refIndex, "ARM_MATH_SUCCESS", elapsed_cycles, time_us);
#else
    while (1);                             /* main function does not return */
#endif
  }
  return status;
}

 /** \endlink */
