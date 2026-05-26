#include "../include/filters.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int compare_uint8(const void *a, const void *b) {
    return (*(uint8_t *)a - *(uint8_t *)b);
}

Image *image_create(int w, int h, int ch) {
    Image *img = (Image *)malloc(sizeof(Image));
    img->width = w;
    img->height = h;
    img->channels = ch;
    img->data = (uint8_t *)calloc(w * h * ch, sizeof(uint8_t));
    return img;
}

Image *image_median_filter(const Image *src, int kernel_size) {
    Image *dst = image_create(src->width, src->height, src->channels);
    int half = kernel_size / 2;
    uint8_t *window = (uint8_t *)malloc(kernel_size * kernel_size);
    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            int count = 0;
            for (int dy = -half; dy <= half; dy++) {
                for (int dx = -half; dx <= half; dx++) {
                    int ny = y + dy;
                    int nx = x + dx;
                    if (ny >= 0 && ny < src->height && nx >= 0 && nx < src->width)
                        window[count++] = src->data[ny * src->width + nx];
                }
            }
            if (count > 0) {
                qsort(window, count, sizeof(uint8_t), compare_uint8);
                dst->data[y * dst->width + x] = window[count / 2];
            }
        }
    }
    free(window);
    return dst;
}

Image *image_gaussian_filter(const Image *src, int kernel_size, double sigma) {
    if (sigma <= 0.0) sigma = kernel_size / 6.0;
    int half = kernel_size / 2;
    float *kernel = (float *)malloc(kernel_size * kernel_size * sizeof(float));
    double sum = 0.0;
    for (int dy = -half; dy <= half; dy++) {
        for (int dx = -half; dx <= half; dx++) {
            double val = exp(-(dx * dx + dy * dy) / (2.0 * sigma * sigma));
            kernel[(dy + half) * kernel_size + (dx + half)] = (float)val;
            sum += val;
        }
    }
    for (int i = 0; i < kernel_size * kernel_size; i++) kernel[i] /= (float)sum;
    Image *result = image_convolve(src, kernel, kernel_size);
    free(kernel);
    return result;
}

Image *image_histogram_equalize(const Image *src) {
    Image *dst = image_create(src->width, src->height, src->channels);
    int hist[256] = {0};
    for (int i = 0; i < src->width * src->height; i++)
        hist[src->data[i]]++;
    int cdf[256] = {0};
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++)
        cdf[i] = cdf[i-1] + hist[i];
    int total = src->width * src->height;
    uint8_t map[256];
    for (int i = 0; i < 256; i++)
        map[i] = (uint8_t)((cdf[i] - hist[0]) * 255.0f / (total - hist[0]) + 0.5f);
    for (int i = 0; i < total; i++)
        dst->data[i] = map[src->data[i]];
    return dst;
}

Image *image_convolve(const Image *src, const float *kernel, int kernel_size) {
    Image *dst = image_create(src->width, src->height, src->channels);
    int half = kernel_size / 2;
    
    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            double sum = 0.0;
            for (int dy = -half; dy <= half; dy++) {
                for (int dx = -half; dx <= half; dx++) {
                    int ny = y + dy;
                    int nx = x + dx;
                    if (ny >= 0 && ny < src->height && nx >= 0 && nx < src->width) {
                        sum += src->data[ny * src->width + nx] * kernel[(dy + half) * kernel_size + (dx + half)];
                    }
                }
            }
            int val = (int)(sum + 0.5);
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            dst->data[y * dst->width + x] = (uint8_t)val;
        }
    }
    return dst;
}

Image *image_sobel_edge(const Image *src, uint8_t threshold) {
    // Sobel kernels for X and Y directions
    float sobel_x[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    float sobel_y[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    
    Image *gx = image_convolve(src, sobel_x, 3);
    Image *gy = image_convolve(src, sobel_y, 3);
    
    Image *dst = image_create(src->width, src->height, src->channels);
    
    for (int i = 0; i < src->width * src->height; i++) {
        int mag = (int)sqrt((double)gx->data[i] * gx->data[i] + (double)gy->data[i] * gy->data[i]);
        if (mag > 255) mag = 255;
        if (mag < threshold) mag = 0;
        else mag = 255;
        dst->data[i] = (uint8_t)mag;
    }
    
    image_free(gx);
    image_free(gy);
    return dst;
}