#ifndef EFFECT_FLAME_H__
#define EFFECT_FLAME_H__

#include <stdint.h>
#include "effect.h"

typedef struct {
  effect_t effect;
} effect_flame_t;

uint32_t effect_flame(effect_t *const effect, uint32_t *const delay);

#endif // EFFECT_FLAME_H__
