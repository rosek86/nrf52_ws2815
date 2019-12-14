#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "effect_flame.h"

static int random_between(int min, int max);

uint32_t effect_flame(effect_t *const effect, uint32_t *const delay) {
  int r = 255;
  int g = 96;
  int b = 12;

  for (int i = 0; i < effect->leds; i++) {
    int flicker = random_between(0, 40);

    int r1 = r - flicker;
    if (r1 < 0) r1 = 0;
    int g1 = g - flicker;
    if (g1 < 0) g1 = 0;
    int b1 = b - flicker;
    if (b1 < 0) b1 = 0;

    effect->set_led(effect->from + i, r1 << 16 | g1 << 8 | b1 << 0);
  }

  *delay = random_between(50, 150);
  return 0;
}

static int random_between(int min, int max) {
  int tmp;
  if (max >= min) {
    max -= min;
  } else {
    tmp = min - max;
    min = max;
    max = tmp;
  }
  return max ? (rand() % max + min) : min;
}
