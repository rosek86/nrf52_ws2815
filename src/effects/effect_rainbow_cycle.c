#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "effect_rainbow_cycle.h"

static uint8_t *wheel(uint8_t WheelPos);

uint32_t effect_rainbow_cycle(effect_t *const effect, uint32_t *const delay) {
  uint8_t *c;

  for (int i = 0; i < effect->leds; i++) {
    c = wheel(((i * 256 / effect->leds) + effect->step) & 255);

    uint32_t color = effect_value_from_rgbw(*c, *(c + 1), *(c + 2), 0);
    effect->set_led(effect->from + i, color);
  }

  if (++effect->step >= 256*5) {
    effect->step = 0;
  }

  *delay = 10;
  return 0;
}

static uint8_t *wheel(uint8_t WheelPos) {
  static uint8_t c[3];

  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }

  return c;
}
