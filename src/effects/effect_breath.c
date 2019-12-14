#include "effect_breath.h"

#include <stdlib.h>
#include <math.h>

uint32_t effect_breath(effect_t const *const effect, uint32_t *const delay) {
  effect_breath_t *e = (effect_breath_t *)effect;

  int lum = e->step;

  if (lum > 255) {
    lum = 511 - lum; // lum = 15 -> 255 -> 15
  }

  if      (lum ==  15) *delay = 970; // 970 pause before each breath
  else if (lum <=  25) *delay =  38; // 19
  else if (lum <=  50) *delay =  36; // 18
  else if (lum <=  75) *delay =  28; // 14
  else if (lum <= 100) *delay =  20; // 10
  else if (lum <= 125) *delay =  14; // 7
  else if (lum <= 150) *delay =  11; // 5
  else                 *delay =  10; // 4

  uint8_t w = (((e->color >> 24 & 0xFF) * lum) + 128) / 256;
  uint8_t r = (((e->color >> 16 & 0xFF) * lum) + 128) / 256;
  uint8_t g = (((e->color >>  8 & 0xFF) * lum) + 128) / 256;
  uint8_t b = (((e->color       & 0xFF) * lum) + 128) / 256;

  for (int i = 0; i < effect->leds; i++) {
    uint32_t output = effect_value_from_rgbw(r, g, b, w);
    effect->set_led(effect->from + i, output);
  }

  e->step += 2;
  if (e->step > (512-15)) {
    e->step = 15;
  }

  return 0;
}
