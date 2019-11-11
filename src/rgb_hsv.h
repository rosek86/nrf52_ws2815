#ifndef RGB_HSV_H__
#define RGB_HSV_H__

#include <stdint.h>

typedef struct {
  uint8_t r, g, b;
} rgb_t;

typedef struct {
  uint16_t h;
  uint8_t s;
  uint8_t v;
} hsv_t;

void hsv_to_rgb(hsv_t const *hsv, rgb_t *rgb);

#endif // RGB_HSV_H__
