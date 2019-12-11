#include "effect_fade_out.h"

#include <stdlib.h>

void effect_fade_out(effect_t const *const effect, uint32_t target, uint8_t rate) {
  static const uint8_t rateMapH[] = { 0, 1, 1, 1, 2, 3, 4, 6 };
  static const uint8_t rateMapL[] = { 0, 2, 3, 8, 8, 8, 8, 8 };

  if (rate >= sizeof(rateMapH)) {
    return;
  }

  uint8_t rateH = rateMapH[rate];
  uint8_t rateL = rateMapL[rate];

  uint32_t color = target;
  int w2 = (color >> 24) & 0xff;
  int r2 = (color >> 16) & 0xff;
  int g2 = (color >>  8) & 0xff;
  int b2 = (color >>  0) & 0xff;

  for (int i = 0; i < effect->leds; i++) {
    effect->get_led(i, &color); // current color

    int w1 = (color >> 24) & 0xff;
    int r1 = (color >> 16) & 0xff;
    int g1 = (color >>  8) & 0xff;
    int b1 = (color >>  0) & 0xff;

    // calculate the color differences between the current and target colors
    int wdelta = w2 - w1;
    int rdelta = r2 - r1;
    int gdelta = g2 - g1;
    int bdelta = b2 - b1;

    // if the current and target colors are almost the same, jump right to the target
    // color, otherwise calculate an intermediate color. (fixes rounding issues)
    wdelta = abs(wdelta) < 3 ? wdelta : (wdelta >> rateH) + (wdelta >> rateL);
    rdelta = abs(rdelta) < 3 ? rdelta : (rdelta >> rateH) + (rdelta >> rateL);
    gdelta = abs(gdelta) < 3 ? gdelta : (gdelta >> rateH) + (gdelta >> rateL);
    bdelta = abs(bdelta) < 3 ? bdelta : (bdelta >> rateH) + (bdelta >> rateL);

    uint32_t output = effect_value_from_rgbw(r1 + rdelta, g1 + gdelta, b1 + bdelta, w1 + wdelta);
    effect->set_led(i, output);
  }
}
