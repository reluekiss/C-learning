#ifndef RTW_STB_IMAGE_H
#define RTW_STB_IMAGE_H

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "external/stb_image.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define RTW_IMAGE_BPP 3

typedef struct rtw_image {
    int image_width;
    int image_height;
    int bytes_per_scanline;
    float *fdata;          // Linear floating-point pixel data
    unsigned char *bdata;  // 8-bit per channel pixel data (RGB)
} rtw_image;

static inline int rtw_clamp(int x, int low, int high) {
    if (x < low) return low;
    if (x < high) return x;
    return high - 1;
}

static inline unsigned char rtw_float_to_byte(float value) {
    if (value <= 0.0f)
        return 0;
    if (value >= 1.0f)
        return 255;
    return (unsigned char)(256.0f * value);
}

static inline void rtw_convert_to_bytes(rtw_image *img) {
    int total_bytes = img->image_width * img->image_height * RTW_IMAGE_BPP;
    img->bdata = (unsigned char *)malloc(total_bytes);
    if (!img->bdata) return;
    for (int i = 0; i < total_bytes; i++)
        img->bdata[i] = rtw_float_to_byte(img->fdata[i]);
}

static inline int rtw_image_load(rtw_image *img, const char *filename) {
    int n = RTW_IMAGE_BPP;  // Dummy out parameter
    img->fdata = stbi_loadf(filename, &img->image_width, &img->image_height, &n, RTW_IMAGE_BPP);
    if (img->fdata == NULL)
        return 0;
    img->bytes_per_scanline = img->image_width * RTW_IMAGE_BPP;
    rtw_convert_to_bytes(img);
    return 1;
}

static inline int rtw_image_width(const rtw_image *img) {
    return (img->fdata == NULL) ? 0 : img->image_width;
}

static inline int rtw_image_height(const rtw_image *img) {
    return (img->fdata == NULL) ? 0 : img->image_height;
}

static inline const unsigned char* rtw_pixel_data(const rtw_image *img, int x, int y) {
    static unsigned char magenta[3] = { 255, 0, 255 };
    if (img->bdata == NULL)
        return magenta;
    x = rtw_clamp(x, 0, img->image_width);
    y = rtw_clamp(y, 0, img->image_height);
    return img->bdata + y * img->bytes_per_scanline + x * RTW_IMAGE_BPP;
}

static inline void rtw_image_destroy(rtw_image *img) {
    if (img->bdata) {
        free(img->bdata);
        img->bdata = NULL;
    }
    if (img->fdata) {
        stbi_image_free(img->fdata);
        img->fdata = NULL;
    }
}

#endif
