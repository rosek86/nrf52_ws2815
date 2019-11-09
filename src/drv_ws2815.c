#include "drv_ws2815.h"

#include "nrf_gpio.h"
#include "nrf_drv_i2s.h"

#define I2S_DATA_BLOCK_WORDS (3 * DRV_WS2815_LEDS_COUNT + DRV_WS2815_RESET_BYTES)

static bool _started = false;

static uint32_t _buffer_tx[2][I2S_DATA_BLOCK_WORDS];
static uint32_t * volatile _block_to_send = NULL;
static uint32_t * volatile _block_to_fill = NULL;

static void data_handler(nrf_drv_i2s_buffers_t const * p_released, uint32_t status);

// I2S configuration
// MCLK = 32 MHz / 8 = 4 MHz
// LRCK = MCLK / 32 = 4 MHz / 32
// SCLK = STEREO * WIDTH * LRCK = 2 * 16 * 4 MHz / 32 = 4 MHz
// 4-bits (i2s) per 1-bit (ws2815)
// 4 MHz / 4-bits = 1 MHz, so 1 us per 1-bit (ws2815)
uint32_t drv_ws2815_init(drv_ws2815_conf_t *const conf) {
  _started = false;
  _block_to_send = NULL;
  _block_to_fill = NULL;

  nrf_drv_i2s_config_t i2s_conf = {
    .sck_pin      = conf->sck_pin,
    .lrck_pin     = conf->lrck_pin,
    .mck_pin      = NRFX_I2S_PIN_NOT_USED,
    .sdout_pin    = conf->sdout_pin,
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

  uint32_t err_code;
  if ((err_code = nrf_drv_i2s_init(&i2s_conf, data_handler)) != 0) {
    return err_code;
  }

  return 0;
}

static void data_handler(nrf_drv_i2s_buffers_t const * p_released,
                         uint32_t                      status) {
  static bool swapping_buffers = false;
  ASSERT(p_released);

  // When the handler is called after the transfer has been stopped
  // (no next buffers are needed, only the used buffers are to be
  // released), there is nothing to do.
  if (!(status & NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED)) {
    return;
  }

  if (swapping_buffers == true) {
    swapping_buffers = false;
    if (_block_to_send == _buffer_tx[0]) {
      _block_to_fill = _buffer_tx[1];
    } else {
      _block_to_fill = _buffer_tx[0];
    }
  }

  if (_block_to_fill == NULL) {
    if (p_released->p_tx_buffer == _buffer_tx[0]) {
      _block_to_send = _buffer_tx[1];
    } else {
      _block_to_send = _buffer_tx[0];
    }
    swapping_buffers = true;
  }

  uint32_t err_code = nrf_drv_i2s_next_buffers_set(&(nrf_drv_i2s_buffers_t) {
    .p_tx_buffer = _block_to_send,
    .p_rx_buffer = NULL,
  });
  APP_ERROR_CHECK(err_code);
}

uint32_t drv_ws2815_start(void) {
  if (_started == true) {
    return DRV_WS2815_RC_INVALID_STATE;
  }

  for (int led = 0; led < DRV_WS2815_LEDS_COUNT; led++) {
    drv_ws2815_from_rgb(0, 0, 0, _buffer_tx[0], led);
    drv_ws2815_from_rgb(0, 0, 0, _buffer_tx[1], led);
  }
  _block_to_send = _buffer_tx[0];
  _block_to_fill = _buffer_tx[1];

  return nrf_drv_i2s_start(&(nrf_drv_i2s_buffers_t) {
    .p_tx_buffer = _block_to_send,
    .p_rx_buffer = NULL,
  }, I2S_DATA_BLOCK_WORDS, 0);
}

uint32_t drv_ws2815_stop(void) {
  nrf_drv_i2s_stop();
  return DRV_WS2815_RC_SUCCESS;
}

bool drv_ws2815_framebuffer_is_busy(void) {
  return _block_to_fill == NULL;
}

uint32_t drv_ws2815_framebuffer_set_led(uint32_t led, uint8_t r, uint8_t g, uint8_t b) {
  if (drv_ws2815_framebuffer_is_busy()) {
    return DRV_WS2815_RC_BUSY;
  }
  drv_ws2815_from_rgb(r, g, b, _block_to_fill, led);
  return DRV_WS2815_RC_SUCCESS;
}

uint32_t drv_ws2815_framebuffer_get(uint32_t **fb) {
  if (drv_ws2815_framebuffer_is_busy()) {
    return DRV_WS2815_RC_BUSY;
  }
  *fb = _block_to_fill;
  return DRV_WS2815_RC_SUCCESS;
}

uint32_t drv_ws2815_framebuffer_commit(void) {
  if (drv_ws2815_framebuffer_is_busy()) {
    return DRV_WS2815_RC_BUSY;
  }

  _block_to_fill = NULL;
  return DRV_WS2815_RC_SUCCESS;
}
