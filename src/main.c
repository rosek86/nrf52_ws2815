#include <stdio.h>
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_drv_systick.h"

#include "drv_ws2815.h"

#define UPDATE_TIME_US (50*1000UL)

static volatile int m_nled = 0;
static volatile bool m_i2s_start = true;
static volatile bool m_i2s_running = false;

static void set_led_data(void);

int main(void) {
  uint32_t err_code = NRF_SUCCESS;

  bsp_board_init(BSP_INIT_LEDS);

  /* Init systick driver */
  nrf_drv_systick_init();

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

  nrfx_systick_state_t systick;
  nrfx_systick_get(&systick);

  for (;;) {
    NRF_LOG_FLUSH();

    // start I2S
    if (m_i2s_start && !m_i2s_running) {
      APP_ERROR_CHECK(drv_ws2815_start());
      set_led_data();

      m_i2s_running = true;
    }

    // stop I2S
    if (!m_i2s_start && m_i2s_running) {
      APP_ERROR_CHECK(drv_ws2815_stop());
      m_i2s_running = false;
    }

    // update
    if (m_i2s_running && nrfx_systick_test(&systick, UPDATE_TIME_US) == true) {
      nrfx_systick_get(&systick);

      drv_ws2815_framebuffer_commit();

      while (drv_ws2815_framebuffer_is_busy() == true) {
      }

      set_led_data();
    }
  }
}

static void set_led_data(void) {
  for (int led = 0; led < DRV_WS2815_LEDS_COUNT; led++) {
    if (led == m_nled) {
      drv_ws2815_framebuffer_set_led(led, 0xAA, 0x00, 0x00);
    } else {
      drv_ws2815_framebuffer_set_led(led, 0x01, 0x01, 0x01);
    }
  }
  m_nled = (m_nled + 1) % DRV_WS2815_LEDS_COUNT;
}

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info) {
  bsp_board_leds_on();
  app_error_save_and_stop(id, pc, info);
}
