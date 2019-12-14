#ifndef EFFECT_H__
#define EFFECT_H__

#include <stdint.h>

typedef struct {
  uint32_t (*get_led)(uint32_t led, uint32_t *value);
  uint32_t (*set_led)(uint32_t led, uint32_t value);
  uint32_t from;
  uint32_t leds;
  uint32_t step;
  uint32_t counter;
} effect_t;

typedef uint32_t (*effect_func_t)(effect_t *const, uint32_t *const);

static inline uint32_t effect_value_from_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  return (uint32_t)w << 24 | (uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b << 0;
}

/*
 * Put a value 0 to 255 in to get a color value.
 * The colours are a transition r -> g -> b -> back to r
 * Inspired by the Adafruit examples.
 */
static inline uint32_t effect_color_wheel(uint8_t pos) {
  pos = 255 - pos;
  if(pos < 85) {
    return ((uint32_t)(255 - pos * 3) << 16) | ((uint32_t)(0) << 8) | (pos * 3);
  } else if(pos < 170) {
    pos -= 85;
    return ((uint32_t)(0) << 16) | ((uint32_t)(pos * 3) << 8) | (255 - pos * 3);
  } else {
    pos -= 170;
    return ((uint32_t)(pos * 3) << 16) | ((uint32_t)(255 - pos * 3) << 8) | (0);
  }
}

#endif // EFFECT_H__
