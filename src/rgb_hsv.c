#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "rgb_hsv.h"

static inline int rgb_value_from_float(float value) {
  if (value < 0) { return   0; }
  if (value > 1) { return 255; }
  return (int)((value * 255.0F) + 0.5F);
}

static inline void rgb_compose(rgb_t *rgb, float r, float g, float b) {
  *rgb = (rgb_t) {
    rgb_value_from_float(r),
    rgb_value_from_float(g),
    rgb_value_from_float(b),
  };
}

static inline void hsv_decompose(hsv_t const *hsv, float *h, float *s, float *v) {
  *h = hsv->h;
  *s = hsv->s / 100.0F;
  *v = hsv->v / 100.0F;
}

void hsv_to_rgb(hsv_t const *hsv, rgb_t *rgb) {
  float h, s, v;
  hsv_decompose(hsv, &h, &s, &v);

  float c = v * s;
  float x = c * (1.0F - fabs(fmodf(h / 60.0F, 2) - 1.0F));
  float m = v - c;

  switch ((int)(h / 60)) {
    case 0: rgb_compose(rgb, c + m, x + m, m); break;
    case 1: rgb_compose(rgb, x + m, c + m, m); break;
    case 2: rgb_compose(rgb, m, c + m, x + m); break;
    case 3: rgb_compose(rgb, m, x + m, c + m); break;
    case 4: rgb_compose(rgb, x + m, m, c + m); break;
    case 5: rgb_compose(rgb, c + m, m, x + m); break;
    default:
      rgb_compose(rgb, m, m, m);
      break;
  }
}
