#include <stdio.h>
#include "nrf_drv_i2s.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define NLEDS 300
#define RESET_BITS 100

#define I2S_DATA_BLOCK_WORDS (3 * NLEDS + RESET_BITS)

// static uint32_t       * volatile mp_block_to_fill  = NULL;
// static uint32_t const * volatile mp_block_to_check = NULL;

static uint32_t m_buffer_tx[I2S_DATA_BLOCK_WORDS];
static volatile int nled = 1;

static volatile uint8_t g_demo_mode = 0;
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

void set_led_data(void) {
  for (int i = 0; i < 3 * NLEDS; i += 3) {
    if (i == (3 * nled)) {
      switch (g_demo_mode) {
        case 0:
          rgb(0xAA, 0, 0, &m_buffer_tx[i]);
          break;
        case 1:
          rgb(0, 0xAA, 0, &m_buffer_tx[i]);
          break;
        case 2:
          rgb(0, 0, 0xAA, &m_buffer_tx[i]);
          break;
      }
    }
    else {
      rgb(1, 1, 1, &m_buffer_tx[i]);
    }
  }

  // reset
  for (int i = 3 * NLEDS; i < I2S_DATA_BLOCK_WORDS; i++) {
    m_buffer_tx[i] = 0;
  }
}

static void data_handler(nrf_drv_i2s_buffers_t const * p_released,
                         uint32_t                      status)
{
    // // 'nrf_drv_i2s_next_buffers_set' is called directly from the handler
    // // each time next buffers are requested, so data corruption is not
    // // expected.
    // ASSERT(p_released);

    // // When the handler is called after the transfer has been stopped
    // // (no next buffers are needed, only the used buffers are to be
    // // released), there is nothing to do.
    // if (!(status & NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED))
    // {
    //     return;
    // }

    // // First call of this handler occurs right after the transfer is started.
    // // No data has been transferred yet at this point, so there is nothing to
    // // check. Only the buffers for the next part of the transfer should be
    // // provided.
    // if (!p_released->p_rx_buffer)
    // {
    //     nrf_drv_i2s_buffers_t const next_buffers = {
    //         .p_rx_buffer = m_buffer_rx[1],
    //         .p_tx_buffer = m_buffer_tx[1],
    //     };
    //     APP_ERROR_CHECK(nrf_drv_i2s_next_buffers_set(&next_buffers));

    //     mp_block_to_fill = m_buffer_tx[1];
    // }
    // else
    // {
    //     mp_block_to_check = p_released->p_rx_buffer;
    //     // The driver has just finished accessing the buffers pointed by
    //     // 'p_released'. They can be used for the next part of the transfer
    //     // that will be scheduled now.
    //     APP_ERROR_CHECK(nrf_drv_i2s_next_buffers_set(p_released));

    //     // The pointer needs to be typecasted here, so that it is possible to
    //     // modify the content it is pointing to (it is marked in the structure
    //     // as pointing to constant data because the driver is not supposed to
    //     // modify the provided data).
    //     mp_block_to_fill = (uint32_t *)p_released->p_tx_buffer;
    // }
}


void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    bsp_board_leds_on();
    app_error_save_and_stop(id, pc, info);
}

int main(void)
{
    uint32_t err_code = NRF_SUCCESS;

    bsp_board_init(BSP_INIT_LEDS);

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("I2S loopback example started.");

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

    for (;;)
    {
        // m_blocks_transferred = 0;
        // mp_block_to_fill  = NULL;
        // mp_block_to_check = NULL;

        // prepare_tx_data(m_buffer_tx);

        // nrf_drv_i2s_buffers_t const initial_buffers = {
        //     .p_tx_buffer = m_buffer_tx,
        //     .p_rx_buffer = m_buffer_rx,
        // };
        // err_code = nrf_drv_i2s_start(&initial_buffers, I2S_DATA_BLOCK_WORDS, 0);
        // APP_ERROR_CHECK(err_code);

        // do {
        //     // Wait for an event.
        //     __WFE();
        //     // Clear the event register.
        //     __SEV();
        //     __WFE();

        //     if (mp_block_to_fill)
        //     {
        //         prepare_tx_data(mp_block_to_fill);
        //         mp_block_to_fill = NULL;
        //     }
        //     if (mp_block_to_check)
        //     {
        //         check_rx_data(mp_block_to_check);
        //         mp_block_to_check = NULL;
        //     }
        // } while (m_blocks_transferred < BLOCKS_TO_TRANSFER);

        // nrf_drv_i2s_stop();

        NRF_LOG_FLUSH();

        // bsp_board_leds_off();
        // nrf_delay_ms(PAUSE_TIME);
        // start I2S
        if (g_i2s_start && !g_i2s_running) {
            nrf_drv_i2s_buffers_t const initial_buffers = {
                .p_tx_buffer = m_buffer_tx,
                .p_rx_buffer = NULL,
            };
            err_code = nrf_drv_i2s_start(&initial_buffers, I2S_DATA_BLOCK_WORDS, 0);
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
            nled = (nled + 1) % NLEDS;
            set_led_data();
        }
    }
}
