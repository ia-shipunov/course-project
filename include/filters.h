#ifndef FILTERS_H
#define FILTERS_H

#include "image.h"

Image *image_create(int w, int h, int ch);
Image *image_median_filter(const Image *src, int kernel_size);
Image *image_gaussian_filter(const Image *src, int kernel_size, double sigma);
Image *image_histogram_equalize(const Image *src);
Image *image_convolve(const Image *src, const float *kernel, int kernel_size);
Image *image_sobel_edge(const Image *src, uint8_t threshold);

#endif