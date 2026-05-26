#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/image.h"
#include "../include/filters.h"

void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s <input> <operation> [params] <output>\n", prog);
    fprintf(stderr, "Operations:\n");
    fprintf(stderr, "  -median <kernel_size>\n");
    fprintf(stderr, "  -gauss <kernel_size> [sigma]\n");
    fprintf(stderr, "  -edges [threshold]\n");
    fprintf(stderr, "  -equalize\n");
    fprintf(stderr, "  -convolve <kernel_size> <values...>\n");
}

int main(int argc, char **argv) {
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    char *input = argv[1];
    char *op = argv[2];
    Image *src = image_load(input);
    if (!src) {
        fprintf(stderr, "Failed to load %s\n", input);
        return 1;
    }

    Image *result = NULL;

    if (strcmp(op, "-median") == 0) {
        if (argc != 5) { print_usage(argv[0]); image_free(src); return 1; }
        int k = atoi(argv[3]);
        result = image_median_filter(src, k);
    }
    else if (strcmp(op, "-gauss") == 0) {
        if (argc < 5 || argc > 6) { print_usage(argv[0]); image_free(src); return 1; }
        int k = atoi(argv[3]);
        double sigma = -1.0;
        if (argc == 6) sigma = atof(argv[4]);
        result = image_gaussian_filter(src, k, sigma);
    }
    else if (strcmp(op, "-edges") == 0) {
        if (argc < 4 || argc > 5) { print_usage(argv[0]); image_free(src); return 1; }
        uint8_t thresh = 128;
        if (argc == 5) thresh = (uint8_t)atoi(argv[3]);
        result = image_sobel_edge(src, thresh);
    }
    else if (strcmp(op, "-equalize") == 0) {
        if (argc != 4) { print_usage(argv[0]); image_free(src); return 1; }
        result = image_histogram_equalize(src);
    }
    else if (strcmp(op, "-convolve") == 0) {
        if (argc < 5) { print_usage(argv[0]); image_free(src); return 1; }
        int k = atoi(argv[3]);
        if (k % 2 == 0) { fprintf(stderr, "Kernel size must be odd\n"); image_free(src); return 1; }
        if (argc != 4 + 1 + k * k) { print_usage(argv[0]); image_free(src); return 1; }
        float *kernel = (float *)malloc(k * k * sizeof(float));
        for (int i = 0; i < k * k; i++) kernel[i] = (float)atof(argv[4 + i]);
        result = image_convolve(src, kernel, k);
        free(kernel);
    }
    else {
        fprintf(stderr, "Unknown operation %s\n", op);
        image_free(src);
        return 1;
    }

    if (!result) {
        fprintf(stderr, "Operation failed\n");
        image_free(src);
        return 1;
    }

    char *output = argv[argc - 1];
    if (!image_save(result, output)) {
        fprintf(stderr, "Failed to save %s\n", output);
    }

    image_free(src);
    image_free(result);
    return 0;
}