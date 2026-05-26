#ifndef FILTERS_H
#define FILTERS_H

#include "image.h"

Image *image_median_filter(const Image *src, int kernel_size);
Image *image_gaussian_filter(const Image *src, int kernel_size, double sigma);
Image *image_histogram_equalize(const Image *src);

#endif