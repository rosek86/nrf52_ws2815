#ifndef EFFECT_BREATH_H__
#define EFFECT_BREATH_H__

#include "effect.h"

typedef struct {
  effect_t effect;
  uint32_t color;
  int step;
} effect_breath_t;

uint32_t effect_breath(effect_t const *const effect, uint32_t *const delay);

#endif // EFFECT_BREATH_H__
