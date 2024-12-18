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
#ifndef DATA_STRUCTURES_HPP
#define DATA_STRUCTURES_HPP

#include <iterator>

namespace arm {
namespace app {

    /**
     * Class Array2d is a data structure that represents a two dimensional array.
     * The data is allocated in contiguous memory, arranged row-wise
     * and individual elements can be accessed with the () operator.
     * For example a two dimensional array D of size (M, N) can be accessed:
     *
     *               _|<------------- col size = N  -------->|
     *               |  D(r=0, c=0) D(r=0, c=1)... D(r=0, c=N)
     *               |  D(r=1, c=0) D(r=1, c=1)... D(r=1, c=N)
     *               |  ...
     *    row size = M  ...
     *               |  ...
     *               _  D(r=M, c=0) D(r=M, c=1)... D(r=M, c=N)
     *
     */
    template<typename T>
    class Array2d {
    public:
        /**
         * @brief     Creates the array2d with the given sizes.
         * @param[in] rows   Number of rows.
         * @param[in] cols   Number of columns.
         */
        Array2d(unsigned rows, unsigned cols): m_rows(rows), m_cols(cols)
        {
            if (rows == 0 || cols == 0) {
                printf("Array2d constructor has 0 size.\n");
                m_data = nullptr;
                return;
            }
            m_data = new T[rows * cols];
        }

        ~Array2d()
        {
            delete[] m_data;
        }

        T& operator() (unsigned int row, unsigned int col)
        {
#if defined(DEBUG)
            if (row >= m_rows || col >= m_cols ||  m_data == nullptr) {
                printf_err("Array2d subscript out of bounds.\n");
            }
#endif /* defined(DEBUG) */
            return m_data[m_cols * row + col];
        }

        T operator() (unsigned int row, unsigned int col) const
        {
#if defined(DEBUG)
            if (row >= m_rows || col >= m_cols ||  m_data == nullptr) {
                printf_err("const Array2d subscript out of bounds.\n");
            }
#endif /* defined(DEBUG) */
            return m_data[m_cols * row + col];
        }

        /**
         * @brief  Gets rows number of the current array2d.
         * @return Number of rows.
         */
        size_t size(size_t dim)
        {
            switch (dim)
            {
                case 0:
                    return m_rows;
                case 1:
                    return m_cols;
                default:
                    return 0;
            }
        }

        /**
         * @brief Gets the array2d total size.
         */
        size_t totalSize()
        {
            return m_rows * m_cols;
        }

        /**
         * array2d iterator.
         */
        using iterator=T*;
        using const_iterator=T const*;

        iterator begin() { return m_data; }
        iterator end() { return m_data + totalSize(); }
        const_iterator begin() const { return m_data; }
        const_iterator end() const { return m_data + totalSize(); };

    private:
        size_t m_rows;
        size_t m_cols;
        T* m_data;
    };

} /* namespace app */
} /* namespace arm */

#endif /* DATA_STRUCTURES_HPP */