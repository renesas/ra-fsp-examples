#ifndef DETECTOR_POST_PROCESSING_HPP
#define DETECTOR_POST_PROCESSING_HPP

#include "DetectionResult.hpp"
#include "Classifier.hpp"

#define DISPLAY_RGB_IMAGE (1)

#define INPUT_IMAGE_WIDTH 192
#define INPUT_IMAGE_HEIGHT 192

#define ORIGINAL_IMAGE_WIDTH 192
#define ORIGINAL_IMAGE_HEIGHT 192

#if DISPLAY_RGB_IMAGE
#define FORMAT_MULTIPLY_FACTOR 3
#else
#define FORMAT_MULTIPLY_FACTOR 1
#endif /* DISPLAY_RGB_IMAGE */

void runPostProcessing(uint8_t *img_in,TfLiteTensor* model_output[2],std::vector<arm::app::DetectionResult> & results_out);

void draw_box_on_image(uint8_t *img_in,int im_w,int im_h,int bx,int by,int bw,int bh);
void rgb_to_grayscale(const uint8_t *rgb,uint8_t *gray, int im_w,int im_h);

#endif /* DETECTOR_POST_PROCESSING_HPP */
