#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/image.h"
#include "../include/filters.h"

static Image *make_image(int w, int h, uint8_t fill) {
    Image *img = image_create(w, h, 1);
    memset(img->data, fill, w * h);
    return img;
}

static int image_equals(const Image *a, const Image *b) {
    if (a->width != b->width || a->height != b->height) return 0;
    return memcmp(a->data, b->data, a->width * a->height) == 0;
}

void test_median_filter() {
    Image *src = make_image(5, 5, 0);
    src->data[2 * 5 + 2] = 255;
    Image *res = image_median_filter(src, 3);
    for (int i = 0; i < 5*5; i++) {
        assert(res->data[i] == 0);
    }
    image_free(src);
    image_free(res);
    printf("  [PASS] median_filter\n");
}
void test_gaussian_filter() {
    Image *src = make_image(10, 10, 100);
    Image *res = image_gaussian_filter(src, 5, 1.0);
    for (int i = 0; i < 10*10; i++) {
        assert(res->data[i] == 100);
    }
    image_free(src);
    image_free(res);
    printf("  [PASS] gaussian_filter_uniform\n");
}

void test_sobel_edge() {
    Image *src = make_image(6, 4, 0);
    for (int y = 0; y < 4; y++)
        for (int x = 3; x < 6; x++)
            src->data[y * 6 + x] = 255;
    Image *edges = image_sobel_edge(src, 200);
    int white_count = 0, black_count = 0;
    for (int i = 0; i < 6*4; i++) {
        if (edges->data[i] == 255) white_count++;
        if (edges->data[i] == 0) black_count++;
    }
    assert(white_count > 0 && black_count > 0);
    image_free(src);
    image_free(edges);
    printf("  [PASS] sobel_edge\n");
}

void test_histogram_equalize() {
    Image *src = make_image(10, 10, 128);
    Image *res = image_histogram_equalize(src);
    for (int i = 0; i < 10*10; i++)
        assert(res->data[i] == 128);
    image_free(src);
    image_free(res);
    printf("  [PASS] histogram_equalize_uniform\n");
}

void test_convolve_identity() {
    Image *src = make_image(5, 5, 50);
    src->data[0] = 10;
    src->data[24] = 200;
    float kernel[9] = {0,0,0, 0,1,0, 0,0,0};
    Image *res = image_convolve(src, kernel, 3);
    int max = 0, min = 255;
    for (int i = 0; i < 5*5; i++) {
        if (res->data[i] > max) max = res->data[i];
        if (res->data[i] < min) min = res->data[i];
    }
    assert(max > min);
    image_free(src);
    image_free(res);
    printf("  [PASS] convolve_identity\n");
}