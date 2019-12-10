#ifndef EFFECT_H__
#define EFFECT_H__

#include <stdint.h>

typedef struct {
  uint32_t (*get_led)(uint32_t led, uint32_t *value);
  uint32_t (*set_led)(uint32_t led, uint32_t value);
  uint32_t leds;
} effect_t;

#endif // EFFECT_H__
