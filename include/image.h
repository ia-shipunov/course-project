#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

typedef struct {
    int width;
    int height;
    int channels;
    uint8_t *data;
} Image;

Image *image_load(const char *filename);
int image_save(const Image *img, const char *filename);
void image_free(Image *img);

#endif