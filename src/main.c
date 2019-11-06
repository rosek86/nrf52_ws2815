#include <stdio.h>
#include "nrf_drv_i2s.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// TODO: separate module for low level display functions

#define NLEDS 300
#define RESET_BYTES 100 // RESET_BYTES * (32bits or 8bits (ws2815))
                        // 100 * 8 * 1us (100 * 8 bit * 1 us / bit) = 800 us
                        // so reset time is 800 us > 280 us (min. specified)

#define I2S_DATA_BLOCK_WORDS (3 * NLEDS + RESET_BYTES)
// total time is (900+100) * 8 * 1us = 8 ms
// or 125 Hz refresh rate

static uint32_t m_buffer_tx[2][I2S_DATA_BLOCK_WORDS];

static uint32_t * volatile m_block_to_send = NULL;
static uint32_t * volatile m_block_to_fill_next = NULL;
static uint32_t * volatile m_block_to_send_next = NULL;

static volatile int nled = 1;

static volatile bool g_i2s_start = true;
static volatile bool g_i2s_running = false;

static inline uint32_t calcOutputColor(uint8_t level) {
  // 0x8 - 0
  // 0xE - 1
  // 4-bits (i2s) per 1-bit (ws2815)

  if (level == 0) {
    return 0x88888888;
  }
  if (level == 255) {
    return 0xEEEEEEEE;
  }

  uint32_t val = 0;
  for (int i = 0; i < 8; i++) {
    if (level & (1 << i)) {
      val |= 0xE << (i << 2);
    } else {
      val |= 0x8 << (i << 2);
    }
  }

  return val;
}

static inline void rgb(uint8_t r, uint8_t g, uint8_t b, uint32_t *buffer) {
  buffer[0] = calcOutputColor(g);
  buffer[1] = calcOutputColor(r);
  buffer[2] = calcOutputColor(b);
}

void set_led_data(uint32_t *to_fill) {
  for (int i = 0; i < 3 * NLEDS; i += 3) {
    if (i == (3 * nled)) {
      rgb(0xAA, 0, 0, &to_fill[i]);
    } else {
      rgb(1, 1, 1, &to_fill[i]);
    }
  }
}

static void data_handler(nrf_drv_i2s_buffers_t const * p_released,
                         uint32_t                      status) {
  ASSERT(p_released);

  // When the handler is called after the transfer has been stopped
  // (no next buffers are needed, only the used buffers are to be
  // released), there is nothing to do.
  if (!(status & NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED)) {
    return;
  }

  if (m_block_to_send_next) {
    m_block_to_send = m_block_to_send_next;
    m_block_to_send_next = NULL;
  }

  uint32_t err_code = nrf_drv_i2s_next_buffers_set(&(nrf_drv_i2s_buffers_t) {
    .p_tx_buffer = m_block_to_send,
    .p_rx_buffer = NULL,
  });
  APP_ERROR_CHECK(err_code);

  if (m_block_to_send == m_buffer_tx[0]) {
    m_block_to_fill_next = m_buffer_tx[1];
  } else {
    m_block_to_fill_next = m_buffer_tx[0];
  }
}

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info) {
  bsp_board_leds_on();
  app_error_save_and_stop(id, pc, info);
}

int main(void) {
  uint32_t err_code = NRF_SUCCESS;

  bsp_board_init(BSP_INIT_LEDS);

  err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_DEFAULT_BACKENDS_INIT();

  // I2S configuration
  // MCLK = 32 MHz / 8 = 4 MHz
  // LRCK = MCLK / 32 = 4 MHz / 32
  // SCLK = STEREO * WIDTH * LRCK = 2 * 16 * 4 MHz / 32 = 4 MHz
  // 4-bits (i2s) per 1-bit (ws2815)
  // 4 MHz / 4-bits = 1 MHz, so 1 us per 1-bit (ws2815)

  nrf_drv_i2s_config_t config = {
    .sck_pin      = NRF_GPIO_PIN_MAP(1, 11),
    .lrck_pin     = NRF_GPIO_PIN_MAP(0, 3),
    .mck_pin      = NRFX_I2S_PIN_NOT_USED,
    .sdout_pin    = NRF_GPIO_PIN_MAP(1, 10),
    .sdin_pin     = NRFX_I2S_PIN_NOT_USED,
    .irq_priority = NRFX_I2S_CONFIG_IRQ_PRIORITY,
    .mode         = NRF_I2S_MODE_MASTER,
    .format       = NRF_I2S_FORMAT_I2S,
    .alignment    = NRF_I2S_ALIGN_LEFT,
    .sample_width = NRF_I2S_SWIDTH_16BIT,
    .channels     = NRF_I2S_CHANNELS_STEREO,
    .mck_setup    = NRF_I2S_MCK_32MDIV8,
    .ratio        = NRF_I2S_RATIO_32X,
  };
  err_code = nrf_drv_i2s_init(&config, data_handler);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_INFO("WS2815 application started.");

  for (;;) {
    NRF_LOG_FLUSH();

    // start I2S
    if (g_i2s_start && !g_i2s_running) {
      m_block_to_send = m_buffer_tx[0];
      err_code = nrf_drv_i2s_start(&(nrf_drv_i2s_buffers_t) {
        .p_tx_buffer = m_block_to_send,
        .p_rx_buffer = NULL,
      }, I2S_DATA_BLOCK_WORDS, 0);
      APP_ERROR_CHECK(err_code);

      g_i2s_running = true;
    }

    // stop I2S
    if (!g_i2s_start && g_i2s_running) {
      nrf_drv_i2s_stop();

      g_i2s_running = false;
    }

    nrf_delay_ms(50);

    // update
    if (g_i2s_running) {
      while (m_block_to_fill_next == NULL) {
      }
      uint32_t *fill = m_block_to_fill_next;

      nled = (nled + 1) % NLEDS;
      set_led_data(fill);

      // NOTE: this can be in critical section
      m_block_to_fill_next = NULL;
      m_block_to_send_next = fill;
      // --
    }
  }
}
