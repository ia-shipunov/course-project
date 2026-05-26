#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../libs/stb_image.h"
#include "../libs/stb_image_write.h"
#include "../include/image.h"
#include <stdlib.h>

Image *image_load(const char *filename) {
    int w, h, ch;
    uint8_t *data = stbi_load(filename, &w, &h, &ch, 1);
    if (!data) return NULL;
    Image *img = (Image *)malloc(sizeof(Image));
    img->width = w;
    img->height = h;
    img->channels = 1;
    img->data = data;
    return img;
}

int image_save(const Image *img, const char *filename) {
    return stbi_write_png(filename, img->width, img->height, img->channels, img->data, img->width * img->channels);
}

void image_free(Image *img) {
    if (img) {
        stbi_image_free(img->data);
        free(img);
    }
}