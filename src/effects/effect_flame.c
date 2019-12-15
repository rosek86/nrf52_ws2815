#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "effect_flame.h"
#include "gamma.h"

static int random_between(int min, int max);

uint32_t effect_flame(effect_t *const effect, uint32_t *const delay) {
  int r = 255;
  int g = 96;
  int b = 12;

  for (int i = 0; i < effect->leds; i++) {
    int flicker = random_between(0, 40);

    int r1 = (r - flicker) < 0 ? 0 : r - flicker;
    int g1 = (g - flicker) < 0 ? 0 : g - flicker;
    int b1 = (b - flicker) < 0 ? 0 : b - flicker;

    uint32_t color = effect_value_from_rgbw(r1, g1, b1, 0);
    effect->set_led(effect->from + i, color);
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
