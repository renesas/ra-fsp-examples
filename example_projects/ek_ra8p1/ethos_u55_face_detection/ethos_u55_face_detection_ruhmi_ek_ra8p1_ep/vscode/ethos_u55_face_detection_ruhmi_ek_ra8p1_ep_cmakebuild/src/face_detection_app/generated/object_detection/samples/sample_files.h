/*
 * sample_files.h
 *
 *  Created on: May 19, 2025
 *      Author: buiba
 */

#ifndef FACE_DETECTION_APP_GENERATED_OBJECT_DETECTION_SAMPLES_SAMPLE_FILES_H_
#define FACE_DETECTION_APP_GENERATED_OBJECT_DETECTION_SAMPLES_SAMPLE_FILES_H_

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#include <stdint.h>

#define NUMBER_OF_FILES (1U)   /**< Total number of files data is available for */
#define IMAGE_DATA_SIZE (110592U)     /**< Size (bytes) for each image */
#define IMAGE_DATA_W    (192)     /**< Width of each image in pixels */
#define IMAGE_DATA_H    (192)    /**< Height of each image pixels */

extern const uint8_t im0[IMAGE_DATA_SIZE];
extern const uint8_t im1[IMAGE_DATA_SIZE];
extern const uint8_t im2[IMAGE_DATA_SIZE];
extern const uint8_t im3[IMAGE_DATA_SIZE];

/**
 * @brief       Gets the filename for the baked-in input array
 * @param[in]   idx     Index of the input.
 * @return      const C string pointer to the name.
 **/
const char* get_sample_data_filename(const uint32_t idx);

/**
 * @brief       Gets the pointer to image data.
 * @param[in]   idx     Index of the input.
 * @return      Pointer to the 8-bit unsigned integer data.
 **/
const uint8_t* get_sample_data_ptr(const uint32_t idx);

/**
 * @brief       Gets the total number of samples available.
 * @return      Number of samples.
 **/
uint32_t get_sample_n_elements(void);

/**
 * @brief       Gets the size (in bytes) of sample images (all images
 *              are expected to be the same size).
 * @return      Size of the image in bytes.
 **/
uint32_t get_sample_img_total_bytes(void);

/**
 * @brief       Gets the image width (pixels).
 * @return      Image width.
 **/
uint32_t get_sample_img_width(void);

/**
 * @brief       Gets the image height (pixels).
 * @return      Image height.
 **/
uint32_t get_sample_img_height(void);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* FACE_DETECTION_APP_GENERATED_OBJECT_DETECTION_SAMPLES_SAMPLE_FILES_H_ */
