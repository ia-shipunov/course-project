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

Image *image_convolve(const Image *src, const float *kernel, int kernel_size) {
    Image *dst = image_create(src->width, src->height, src->channels);
    int half = kernel_size / 2;
    float *tmp = (float *)calloc(src->width * src->height, sizeof(float));
    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            float sum = 0.0f;
            for (int dy = -half; dy <= half; dy++) {
                for (int dx = -half; dx <= half; dx++) {
                    int ny = y + dy;
                    int nx = x + dx;
                    if (ny >= 0 && ny < src->height && nx >= 0 && nx < src->width) {
                        int kidx = (dy + half) * kernel_size + (dx + half);
                        sum += src->data[ny * src->width + nx] * kernel[kidx];
                    }
                }
            }
            tmp[y * src->width + x] = sum;
        }
    }
    float min_val = tmp[0], max_val = tmp[0];
    for (int i = 0; i < src->width * src->height; i++) {
        if (tmp[i] < min_val) min_val = tmp[i];
        if (tmp[i] > max_val) max_val = tmp[i];
    }
    float range = max_val - min_val;
    if (range == 0) range = 1;
    for (int i = 0; i < src->width * src->height; i++) {
        dst->data[i] = (uint8_t)(255.0f * (tmp[i] - min_val) / range + 0.5f);
    }
    free(tmp);
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

Image *image_sobel_edge(const Image *src, uint8_t threshold) {
    int ksize = 3;
    float Gx[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    float Gy[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    Image *grad_x = image_convolve(src, Gx, ksize);
    Image *grad_y = image_convolve(src, Gy, ksize);
    Image *dst = image_create(src->width, src->height, 1);
    for (int i = 0; i < src->width * src->height; i++) {
        float mag = sqrtf((float)(grad_x->data[i] - 127) * (grad_x->data[i] - 127) +
                          (float)(grad_y->data[i] - 127) * (grad_y->data[i] - 127));
        dst->data[i] = (mag > threshold) ? 255 : 0;
    }
    image_free(grad_x);
    image_free(grad_y);
    return dst;
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