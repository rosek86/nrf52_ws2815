#ifndef FADE_OUT_H__
#define FADE_OUT_H__

#include "effect.h"
#include <stdint.h>

#define FADE_XFAST   (uint8_t)1
#define FADE_FAST    (uint8_t)2
#define FADE_MEDIUM  (uint8_t)3
#define FADE_SLOW    (uint8_t)4
#define FADE_XSLOW   (uint8_t)5
#define FADE_XXSLOW  (uint8_t)6
#define FADE_GLACIAL (uint8_t)7

typedef struct {
  effect_t effect;
  uint32_t color;
  uint8_t rate;
} effect_fade_out_t;

uint32_t effect_fade_out(effect_t const *const effect, uint32_t *const delay);

#endif // FADE_OUT_H__
