#ifndef EFFECT_BLINK_H__
#define EFFECT_BLINK_H__

#include <stdint.h>
#include "effect.h"

typedef struct {
  effect_t effect;
  uint32_t color1;
  uint32_t color2;
  bool strobe;
  uint32_t speed;
} effect_blink_t;

uint32_t effect_blink(effect_t const *const effect, uint32_t *const delay);
uint32_t effect_blink_rainbow(effect_t const *const effect, uint32_t *const delay);

#endif // EFFECT_BLINK_H__
