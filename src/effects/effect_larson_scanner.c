#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "effect_flame.h"

uint32_t mode_larson_scanner(effect_t *const effect, uint32_t *const delay) {
  fade_out();

  if (effect->step < effect->leds) {
    setPixelColor(effect->from + effect->step, SEGMENT.colors[0]);
  } else {
    uint16_t index = (SEGMENT_LENGTH * 2) - effect->step - 2;
    setPixelColor(effect->from + index, SEGMENT.colors[0]);
  }

  if ((effect->step % effect->leds) == 0) {
    SET_CYCLE;
  } else {
    CLR_CYCLE;
  }

  effect->step++;
  if (effect->step >= ((effect->leds * 2) - 2)) {
    effect->step = 0;
  }

  return (SEGMENT.speed / (effect->leds * 2));
}