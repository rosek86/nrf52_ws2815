#ifndef EFFECT_H__
#define EFFECT_H__

#include <stdint.h>

typedef struct {
  uint32_t (*get_led)(uint32_t led, uint32_t *value);
  uint32_t (*set_led)(uint32_t led, uint32_t value);
  uint32_t from;
  uint32_t leds;
  uint32_t counter;
} effect_t;

typedef uint32_t (*effect_func_t)(effect_t const *const, uint32_t *const);

static inline uint32_t effect_value_from_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  return (uint32_t)w << 24 | (uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b << 0;
}

#endif // EFFECT_H__
