/*
 * SPDX-FileCopyrightText: Copyright 2021 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MFCC_HPP
#define MFCC_HPP

#include <ai_apps/common/PlatformMath.hpp>
#include <vector>
#include <cstdint>
#include <cmath>
#include <limits>
#include <string>

namespace arm {
namespace app {
namespace audio {

    /* MFCC's consolidated parameters. */
    class MfccParams {
    public:
        float       m_samplingFreq;
        uint32_t    m_numFbankBins;
        float       m_melLoFreq;
        float       m_melHiFreq;
        uint32_t    m_numMfccFeatures;
        uint32_t    m_frameLen;
        uint32_t    m_frameLenPadded;
        bool        m_useHtkMethod;

        /** @brief  Constructor */
        MfccParams(float samplingFreq, uint32_t numFbankBins,
                   float melLoFreq, float melHiFreq,
                   uint32_t numMfccFeats, uint32_t frameLen,
                   bool useHtkMethod);

        MfccParams()  = delete;

        ~MfccParams() = default;

        /** @brief  Log parameters */
        void Log() const;
    };

    /**
     * @brief   Class for MFCC feature extraction.
     *          Based on https://github.com/ARM-software/ML-KWS-for-MCU/blob/master/Deployment/Source/MFCC/mfcc.cpp
     *          This class is designed to be generic and self-sufficient but
     *          certain calculation routines can be overridden to accommodate
     *          use-case specific requirements.
     */
    class MFCC {
    public:
        /**
         * @brief       Constructor
         * @param[in]   params   MFCC parameters
        */
        explicit MFCC(const MfccParams& params);

        MFCC() = delete;

        ~MFCC() = default;

        /**
        * @brief        Extract MFCC  features for one single small frame of
        *               audio data e.g. 640 samples.
        * @param[in]    audioData   Vector of audio samples to calculate
        *                           features for.
        * @return       Vector of extracted MFCC features.
        **/
        std::vector<float> MfccCompute(const std::vector<int16_t>& audioData);

        /** @brief  Initialise. */
        void Init();

       /**
        * @brief        Extract MFCC features and quantise for one single small
        *               frame of audio data e.g. 640 samples.
        * @param[in]    audioData     Vector of audio samples to calculate
        *                             features for.
        * @param[in]    quantScale    Quantisation scale.
        * @param[in]    quantOffset   Quantisation offset.
        * @return       Vector of extracted quantised MFCC features.
        **/
        template<typename T>
        std::vector<T> MfccComputeQuant(const std::vector<int16_t>& audioData,
                                        const float quantScale,
                                        const int quantOffset)
        {
            this->MfccComputePreFeature(audioData);
            float minVal = std::numeric_limits<T>::min();
            float maxVal = std::numeric_limits<T>::max();

            std::vector<T> mfccOut(this->m_params.m_numMfccFeatures);
            const size_t numFbankBins = this->m_params.m_numFbankBins;

            /* Take DCT. Uses matrix mul. */
            for (size_t i = 0, j = 0; i < mfccOut.size(); ++i, j += numFbankBins) {
                float sum = 0;
                for (size_t k = 0; k < numFbankBins; ++k) {
                    sum += this->m_dctMatrix[j + k] * this->m_melEnergies[k];
                }
                /* Quantize to T. */
                sum = std::round((sum / quantScale) + quantOffset);
                mfccOut[i] = static_cast<T>(std::min<float>(std::max<float>(sum, minVal), maxVal));
            }

            return mfccOut;
        }

        /* Constants */
        static constexpr float ms_logStep = /*logf(6.4)*/ 1.8562979903656 / 27.0;
        static constexpr float ms_freqStep = 200.0 / 3;
        static constexpr float ms_minLogHz = 1000.0;
        static constexpr float ms_minLogMel = ms_minLogHz / ms_freqStep;

    protected:
        /**
         * @brief       Project input frequency to Mel Scale.
         * @param[in]   freq           Input frequency in floating point.
         * @param[in]   useHTKMethod   bool to signal if HTK method is to be
         *                             used for calculation.
         * @return      Mel transformed frequency in floating point.
         **/
        static float MelScale(float freq,
                              bool  useHTKMethod = true);

        /**
         * @brief       Inverse Mel transform - convert MEL warped frequency
         *              back to normal frequency.
         * @param[in]   melFreq        Mel frequency in floating point.
         * @param[in]   useHTKMethod   bool to signal if HTK method is to be
         *                             used for calculation.
         * @return      Real world frequency in floating point.
         **/
        static float InverseMelScale(float melFreq,
                                     bool  useHTKMethod = true);

        /**
         * @brief       Populates MEL energies after applying the MEL filter
         *              bank weights and adding them up to be placed into
         *              bins, according to the filter bank's first and last
         *              indices (pre-computed for each filter bank element
         *              by CreateMelFilterBank function).
         * @param[in]   fftVec                  Vector populated with FFT magnitudes.
         * @param[in]   melFilterBank           2D Vector with filter bank weights.
         * @param[in]   filterBankFilterFirst   Vector containing the first indices of filter bank
         *                                      to be used for each bin.
         * @param[in]   filterBankFilterLast    Vector containing the last indices of filter bank
         *                                      to be used for each bin.
         * @param[out]  melEnergies             Pre-allocated vector of MEL energies to be
         *                                      populated.
         * @return      true if successful, false otherwise.
         */
        virtual bool ApplyMelFilterBank(
            std::vector<float>&                 fftVec,
            std::vector<std::vector<float>>&    melFilterBank,
            std::vector<uint32_t>&               filterBankFilterFirst,
            std::vector<uint32_t>&               filterBankFilterLast,
            std::vector<float>&                 melEnergies);

        /**
         * @brief           Converts the Mel energies for logarithmic scale.
         * @param[in,out]   melEnergies   1D vector of Mel energies.
         **/
        virtual void ConvertToLogarithmicScale(std::vector<float>& melEnergies);

        /**
         * @brief       Create a matrix used to calculate Discrete Cosine
         *              Transform.
         * @param[in]   inputLength        Input length of the buffer on which
         *                                 DCT will be performed.
         * @param[in]   coefficientCount   Total coefficients per input length.
         * @return      1D vector with inputLength x coefficientCount elements
         *              populated with DCT coefficients.
         */
        virtual std::vector<float> CreateDCTMatrix(
                                    int32_t inputLength,
                                    int32_t coefficientCount);

        /**
         * @brief       Given the low and high Mel values, get the normaliser
         *              for weights to be applied when populating the filter
         *              bank.
         * @param[in]   leftMel        Low Mel frequency value.
         * @param[in]   rightMel       High Mel frequency value.
         * @param[in]   useHTKMethod   bool to signal if HTK method is to be
         *                             used for calculation.
         * @return      Value to use for normalizing.
         */
        virtual float GetMelFilterBankNormaliser(
                        const float&   leftMel,
                        const float&   rightMel,
                        bool     useHTKMethod);

    private:
        MfccParams                      m_params;
        std::vector<float>              m_frame;
        std::vector<float>              m_buffer;
        std::vector<float>              m_melEnergies;
        std::vector<float>              m_windowFunc;
        std::vector<std::vector<float>> m_melFilterBank;
        std::vector<float>              m_dctMatrix;
        std::vector<uint32_t>           m_filterBankFilterFirst;
        std::vector<uint32_t>           m_filterBankFilterLast;
        bool                            m_filterBankInitialised;
        arm::app::math::FftInstance     m_fftInstance;

        /**
         * @brief       Initialises the filter banks and the DCT matrix. **/
        void InitMelFilterBank();

        /**
         * @brief       Signals whether the instance of MFCC has had its
         *              required buffers initialised.
         * @return      true if initialised, false otherwise.
         **/
        bool IsMelFilterBankInited() const;

        /**
         * @brief       Create mel filter banks for MFCC calculation.
         * @return      2D vector of floats.
         **/
        std::vector<std::vector<float>> CreateMelFilterBank();

        /**
         * @brief       Computes and populates internal memeber buffers used
         *              in MFCC feature calculation
         * @param[in]   audioData   1D vector of 16-bit audio data.
         */
        void MfccComputePreFeature(const std::vector<int16_t>& audioData);

        /** @brief       Computes the magnitude from an interleaved complex array. */
        void ConvertToPowerSpectrum();

    };

} /* namespace audio */
} /* namespace app */
} /* namespace arm */

#endif /* MFCC_HPP */