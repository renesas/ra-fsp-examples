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
#ifndef AUDIO_UTILS_HPP
#define AUDIO_UTILS_HPP

#include <cstddef>
#include <cstdint>

namespace arm {
namespace app {
namespace audio {

    template<class T>
    class SlidingWindow {
    public:

        /**
         * @brief     Creates the window slider through the given data.
         *
         * @param[in] data         Pointer to the data to slide through.
         * @param[in] dataSize     Size in T type elements wise.
         * @param[in] windowSize   Sliding window size in T type wise elements.
         * @param[in] stride       Stride size in T type wise elements.
         */
        SlidingWindow(T *data, size_t dataSize,
                      size_t windowSize, size_t stride) {
            m_start = data;
            m_dataSize = dataSize;
            m_size = windowSize;
            m_stride = stride;
        }

        SlidingWindow() = default;

        ~SlidingWindow() = default;

        /**
         * @brief  Get the next data window.
         * @return Pointer to the next window, if next window is not available nullptr is returned.
         */
        virtual T *Next() {
            if (HasNext()) {
                m_count++;
                return m_start + Index() * m_stride;
            } else {
                return nullptr;
            }
        }

        /**
         * @brief  Checks if the next data portion is available.
         * @return true if next data portion is available.
         */
        virtual bool HasNext() {
            return m_size + m_count * m_stride <= m_dataSize;
        }

        /**
         * @brief Reset the slider to the initial position.
         */
        virtual void Reset() {
            m_count = 0;
        }

        /**
         * @brief     Resets the slider to the start of the new data.
         *            New data size MUST be the same as the old one.
         * @param[in] newStart   Pointer to the new data to slide through.
         */
        virtual void Reset(T *newStart) {
            m_start = newStart;
            Reset();
        }

        /**
         * @brief  Gets current index of the sliding window.
         * @return Current position of the sliding window in number of strides.
         */
        size_t Index() {
            return m_count == 0? 0: m_count - 1;
        }

        /**
         * @brief  Gets the index from the start of the data where the next window will begin.
         *         While Index() returns the index of sliding window itself this function
         *         returns the index of the data element itself.
         * @return Index from the start of the data where the next sliding window will begin.
         */
        virtual uint32_t NextWindowStartIndex() {
            return m_count == 0? 0: ((m_count) * m_stride);
        }

        /**
         * @brief     Go to given sliding window index.
         * @param[in] index   New position of the sliding window. If index is invalid
         *                    (greater than possible range of strides) then next call to Next() will return nullptr.
         */
        void FastForward(size_t index) {
            m_count = index;
        }

        /**
         * @brief  Calculates whole number of times the window can stride through the given data.
         * @return Maximum number of whole strides.
         */
         size_t TotalStrides() {
            if (m_size > m_dataSize) {
                return 0;
            }
            return ((m_dataSize - m_size)/m_stride);
        }


    protected:
        T *m_start = nullptr;
        size_t m_dataSize = 0;
        size_t m_size = 0;
        size_t m_stride = 0;
        size_t m_count = 0;
    };

    /*
     * Sliding window that will cover the whole length of the input, even if
     * this means the last window is not a full window length.
     */
    template<class T>
    class FractionalSlidingWindow : public SlidingWindow<T> {
    public:
        using SlidingWindow<T>::SlidingWindow;

        /**
         * @brief  Checks if the next data portion is available.
         * @return true if next data portion is available.
         */
        bool HasNext() {
            return this->m_count < 1 + this->FractionalTotalStrides() && (this->NextWindowStartIndex() < this->m_dataSize);
        }

        /**
        * @brief  Calculates number of times the window can stride through the given data.
        *         May not be a whole number.
        * @return Number of strides to cover all data.
        */
        float FractionalTotalStrides() {
            if (this->m_dataSize < this->m_size) {
                return 0;
            } else {
                return ((this->m_dataSize - this->m_size) / static_cast<float>(this->m_stride));
            }
        }
    };


} /* namespace audio */
} /* namespace app */
} /* namespace arm */

#endif /* AUDIO_UTILS_HPP */