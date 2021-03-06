#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "effect_blink.h"

/*
 * Blink/strobe function
 * Alternate between color1 and color2
 * if(strobe == true) then create a strobe effect
 */
uint32_t effect_blink(effect_t const *const effect, uint32_t *const delay) {
  effect_blink_t *e = (effect_blink_t *)effect;

  uint32_t color = ((effect->counter & 1) == 0) ? e->color1 : e->color2;

  for (int i = 0; i < effect->leds; i++) {
    effect->set_led(effect->from + i, color);
  }

  if ((effect->counter & 1) == 0) {
    *delay = e->strobe ? 20 : (e->speed / 2);
  } else {
    *delay = e->strobe ? (e->speed - 20) : (e->speed / 2);
  }

  return 0;
}

uint32_t effect_blink_rainbow(effect_t const *const effect, uint32_t *const delay) {
  effect_blink_t *e = (effect_blink_t *)effect;
  e->color1 = effect_color_wheel(effect->counter & 0xFF);
  return effect_blink(effect, delay);
}
