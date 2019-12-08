#ifndef DRV_WS2815_H__
#define DRV_WS2815_H__

#include <stdint.h>
#include <stdbool.h>

// leds configuration
#define DRV_WS2815_INVERTED     1   // transistor inverts bits
#define DRV_WS2815_LEDS_COUNT   300
#define DRV_WS2815_RESET_BYTES  100
// example for DRV_WS2815_RESET_BYTES = 100
// we have 8us per ws2815 byte 
// 100 byte * 8us / byte = 800 us
// so reset time is 800 us > 280 us (min. specified)

// total update time
// example for DRV_WS2815_LEDS_COUNT = 300, DRV_WS2815_RESET_BYTES = 100
// (300*3 + 100) byte * 8us / byte = 8 ms
// which gives 1000 ms / 8 ms = 125 fps

// return codes
#define DRV_WS2815_RET_CODE_BASE 0x1234
typedef enum {
  DRV_WS2815_RC_SUCCESS = 0,
  DRV_WS2815_RC_BUSY = DRV_WS2815_RET_CODE_BASE,
  DRV_WS2815_RC_INVALID_STATE,
} drv_ws2815_rc_t;

typedef struct {
  uint32_t sck_pin;
  uint32_t lrck_pin;
  uint32_t sdout_pin;
} drv_ws2815_conf_t;

uint32_t drv_ws2815_init(drv_ws2815_conf_t *const conf);

uint32_t drv_ws2815_start(void);
uint32_t drv_ws2815_stop(void);

bool drv_ws2815_framebuffer_is_busy(void);

uint32_t drv_ws2815_framebuffer_set_led(uint32_t led, uint8_t r, uint8_t g, uint8_t b);
uint32_t drv_ws2815_framebuffer_get(uint32_t **fb);
uint32_t drv_ws2815_framebuffer_commit(void);

// color conversion
#define _DRV_WS2815_I2S_BITS_PER_LED_BIT 4
#define _DRV_WS2815_FROM_COLOR_BIT(x, bit) (                  \
    ((x) & (1 << (bit))) ?                                    \
      (0x0E << ((bit) * _DRV_WS2815_I2S_BITS_PER_LED_BIT)) :  \
      (0x08 << ((bit) * _DRV_WS2815_I2S_BITS_PER_LED_BIT))    \
  )
#define _DRV_WS2815_FROM_COLOR(x) (_DRV_WS2815_FROM_COLOR_BIT(x, 0) | _DRV_WS2815_FROM_COLOR_BIT(x, 1) | \
                                   _DRV_WS2815_FROM_COLOR_BIT(x, 2) | _DRV_WS2815_FROM_COLOR_BIT(x, 3) | \
                                   _DRV_WS2815_FROM_COLOR_BIT(x, 4) | _DRV_WS2815_FROM_COLOR_BIT(x, 5) | \
                                   _DRV_WS2815_FROM_COLOR_BIT(x, 6) | _DRV_WS2815_FROM_COLOR_BIT(x, 7))

#if (DRV_WS2815_INVERTED == 0)
#define DRV_WS2815_FROM_COLOR(x) (_DRV_WS2815_FROM_COLOR(x))
#define DRV_WS2815_RESET_DWORD 0x00000000
#else
#define DRV_WS2815_FROM_COLOR(x) (~_DRV_WS2815_FROM_COLOR(x))
#define DRV_WS2815_RESET_DWORD 0xFFFFFFFF
#endif

static inline uint32_t drv_ws2815_from_color(uint8_t color) {
  uint32_t c = DRV_WS2815_FROM_COLOR(color);
  return ((c & 0xFFFF0000) >> 16) | ((c & 0x0000FFFF) << 16);
}

static inline void drv_ws2815_from_rgb(uint8_t r, uint8_t g, uint8_t b, uint32_t *buffer, uint32_t led) {
  buffer[(led * 3) + 0] = drv_ws2815_from_color(g);
  buffer[(led * 3) + 1] = drv_ws2815_from_color(r);
  buffer[(led * 3) + 2] = drv_ws2815_from_color(b);
}

#endif // DRV_WS2815_H__
