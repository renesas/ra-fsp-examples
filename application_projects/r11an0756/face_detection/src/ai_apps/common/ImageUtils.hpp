/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef IMAGE_UTILS_HPP
#define IMAGE_UTILS_HPP

#include <cstddef>
#include <cstdint>
#include <forward_list>
#include <vector>

/* Helper macro to convert RGB888 to RGB565 format. */
#define RGB888_TO_RGB565(R8,G8,B8)  ((((R8>>3) & 0x1F) << 11) |     \
                                     (((G8>>2) & 0x3F) << 5)  |     \
                                     ((B8>>3) & 0x1F))

constexpr uint16_t COLOR_BLACK  = 0;
constexpr uint16_t COLOR_GREEN  = RGB888_TO_RGB565(  0, 255,  0); // 2016;
constexpr uint16_t COLOR_YELLOW = RGB888_TO_RGB565(255, 255,  0); // 65504;


namespace arm {
namespace app {
namespace image {

    /**
     * Contains the x,y co-ordinates of a box centre along with the box width and height.
     */
    struct Box {
        float x;
        float y;
        float w;
        float h;
    };

    struct Detection {
        Box bbox;
        std::vector<float> prob;
        float objectness;
    };

    /**
     * @brief       Calculate the 1D overlap.
     * @param[in]   x1Center   First center point.
     * @param[in]   width1     First width.
     * @param[in]   x2Center   Second center point.
     * @param[in]   width2     Second width.
     * @return      The overlap between the two lines.
     **/
    float Calculate1DOverlap(float x1Center, float width1, float x2Center, float width2);

    /**
     * @brief       Calculate the intersection between the two given boxes.
     * @param[in]   box1   First box.
     * @param[in]   box2   Second box.
     * @return      The intersection value.
     **/
    float CalculateBoxIntersect(Box& box1, Box& box2);

    /**
     * @brief       Calculate the union between the two given boxes.
     * @param[in]   box1   First box.
     * @param[in]   box2   Second box.
     * @return      The two given boxes union value.
     **/
    float CalculateBoxUnion(Box& box1, Box& box2);

    /**
     * @brief       Calculate the intersection over union between the two given boxes.
     * @param[in]   box1   First box.
     * @param[in]   box2   Second box.
     * @return      The intersection over union value.
     **/
    float CalculateBoxIOU(Box& box1, Box& box2);

    /**
     * @brief       Calculate the Non-Maxima suppression on the given detection boxes.
     * @param[in]   detections    List of Detection boxes.
     * @param[in]   classes       Number of classes.
     * @param[in]   iouThreshold  Intersection over union threshold.
     **/
    void CalculateNMS(std::forward_list<Detection>& detections, int classes, float iouThreshold);

    /**
     * @brief           Helper function to convert a UINT8 image to INT8 format.
     * @param[in,out]   data            Pointer to the data start.
     * @param[in]       kMaxImageSize   Total number of pixels in the image.
     **/
    void ConvertImgToInt8(void* data, size_t kMaxImageSize);

    /**
     * @brief       Converts RGB image to grayscale.
     * @param[in]   srcPtr   Pointer to RGB source image.
     * @param[out]  dstPtr   Pointer to grayscale destination image.
     * @param[in]   imgSz    Destination image size.
     **/
    void RgbToGrayscale(const uint8_t* srcPtr, uint8_t* dstPtr, size_t dstImgSz);

} /* namespace image */
} /* namespace app */
} /* namespace arm */

#endif /* IMAGE_UTILS_HPP */