#include <stdio.h>
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "drv_ws2815.h"

// TODO: non-blocking loop

static volatile int nled = 1;

static volatile bool g_i2s_start = true;
static volatile bool g_i2s_running = false;

static void set_led_data(uint32_t *to_fill);

int main(void) {
  uint32_t err_code = NRF_SUCCESS;

  bsp_board_init(BSP_INIT_LEDS);

  err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_DEFAULT_BACKENDS_INIT();

  err_code = drv_ws2815_init(&(drv_ws2815_conf_t) {
    .sck_pin   = NRF_GPIO_PIN_MAP(1, 11),
    .lrck_pin  = NRF_GPIO_PIN_MAP(0, 3),
    .sdout_pin = NRF_GPIO_PIN_MAP(1, 10),
  });
  APP_ERROR_CHECK(err_code);

  NRF_LOG_INFO("WS2815 application started.");

  for (;;) {
    NRF_LOG_FLUSH();

    // start I2S
    if (g_i2s_start && !g_i2s_running) {
      APP_ERROR_CHECK(drv_ws2815_start());
      g_i2s_running = true;
    }

    // stop I2S
    if (!g_i2s_start && g_i2s_running) {
      APP_ERROR_CHECK(drv_ws2815_stop());
      g_i2s_running = false;
    }

    nrf_delay_ms(50);

    // update
    if (g_i2s_running) {
      while (drv_ws2815_framebuffer_is_ready() != DRV_WS2815_RC_SUCCESS) {
      }

      uint32_t *fill;
      drv_ws2815_framebuffer_get(&fill);

      nled = (nled + 1) % DRV_WS2815_LEDS_COUNT;
      set_led_data(fill);

      drv_ws2815_commit();
    }
  }
}

static void set_led_data(uint32_t *to_fill) {
  for (int i = 0; i < 3 * DRV_WS2815_LEDS_COUNT; i += 3) {
    if (i == (3 * nled)) {
      drv_ws2815_from_rgb(0xAA, 0, 0, &to_fill[i]);
    } else {
      drv_ws2815_from_rgb(1, 1, 1, &to_fill[i]);
    }
  }
}

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info) {
  bsp_board_leds_on();
  app_error_save_and_stop(id, pc, info);
}
