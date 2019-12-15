#include <stdio.h>
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"

#include "app_timer.h"
#include "app_button.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_drv_clock.h"
#include "nrf_drv_systick.h"

#include "drv_ws2815.h"

#include "effects/effect_fade_out.h"
#include "effects/effect_breath.h"
#include "effects/effect_blink.h"
#include "effects/effect_flame.h"
#include "effects/effect_rainbow_cycle.h"

#define UPDATE_TIME_US (2*1000UL)

static volatile int m_nled = 0;
static volatile bool m_i2s_start = true;
static volatile bool m_i2s_running = false;

// static effect_breath_t _effect_breath = {
//   .effect = _effect_info,
//   .color = 0x00AA0000,
//   .step = 15
// };
// static effect_t *_effect = (effect_t *)&_effect_breath;
// static effect_func_t _effect_func = effect_breath;

// static effect_blink_t _effect_blink = {
//   .effect = _effect_info,
//   .color1 = 0x00100000,
//   .color2 = 0x00000000,
//   .strobe = false,
//   .speed  = 5000
// };
// static effect_t *_effect = (effect_t *)&_effect_blink;
// static effect_func_t _effect_func = effect_blink_rainbow;

// static effect_fade_out_t _effect_fade_out = {
//   .effect = _effect_info,
//   .color = 0x00FF0000,
//   .rate = 7,
// };
// static effect_t *_effect = (effect_t *)&_effect_fade_out;
// static effect_func_t _effect_func = effect_fade_out;

static effect_t _effect_general = {
  .get_led  = drv_ws2815_framebuffer_get_led_value,
  .set_led  = drv_ws2815_framebuffer_set_led_value,
  .from     = 0,
  .leds     = DRV_WS2815_LEDS_COUNT,
  .counter  = 0,
};

typedef struct {
  effect_t *effect;
  effect_func_t func;
} effects_t;

static uint32_t _effects_sel = 0;
static effects_t _effects[] = {
  { &_effect_general, .func = effect_flame         },
  { &_effect_general, .func = effect_rainbow_cycle },
};

static effect_t *_effect;
static effect_func_t _effect_func;

static void set_led_data(void);

/**@brief Function for handling events from the button handler module.
 *
 * @param[in] pin_no        The pin that the event applies to.
 * @param[in] button_action The button action (press/release).
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action) {
  if (button_action == APP_BUTTON_RELEASE) {
    return;
  }

  switch (pin_no) {
    case BSP_BUTTON_0:
      NRF_LOG_INFO("Send button state change.");
      _effects_sel = (_effects_sel + 1) & 1;
      break;
    default:
      APP_ERROR_HANDLER(pin_no);
      break;
  }
}

/**@brief Function for initializing the button handler module.
 */
static void buttons_init(void) {
  ret_code_t err_code;

  // The array must be static because a pointer to it will be saved in the button handler module.
  static app_button_cfg_t buttons[] = {
    { BSP_BUTTON_0, false, BUTTON_PULL, button_event_handler }
  };

  err_code = app_button_init(buttons, ARRAY_SIZE(buttons), APP_TIMER_TICKS(50));
  APP_ERROR_CHECK(err_code);

  err_code = app_button_enable();
  APP_ERROR_CHECK(err_code);
}

/** @brief Function starting the internal LFCLK XTAL oscillator.
 */
static void lfclk_config(void) {
  ret_code_t err_code = nrf_drv_clock_init();
  APP_ERROR_CHECK(err_code);

  nrf_drv_clock_lfclk_request(NULL);
}

int main(void) {
  uint32_t err_code = NRF_SUCCESS;
  bool update_framebuffer = false;

  lfclk_config();

  /* Init systick driver */
  nrf_drv_systick_init();

  err_code = app_timer_init();
  APP_ERROR_CHECK(err_code);

  err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_DEFAULT_BACKENDS_INIT();

  NRF_LOG_INFO("WS2815 logging initialized.");

  bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
  buttons_init();

  nrf_gpio_cfg_output(BSP_DBG1);
  nrf_gpio_cfg_output(BSP_DBG2);
  nrf_gpio_cfg_output(BSP_DBG3);

  nrf_gpio_pin_set(LED1_G);
  nrf_gpio_cfg_output(LED1_G);
  nrf_gpio_pin_clear(LED1_G);

  nrf_gpio_pin_set(LED2_R);
  nrf_gpio_cfg_output(LED2_R);
  nrf_gpio_pin_set(LED2_G);
  nrf_gpio_cfg_output(LED2_G);
  nrf_gpio_pin_set(LED2_B);
  nrf_gpio_cfg_output(LED2_B);

  nrf_gpio_pin_set(BSP_PMU_LED_EN);
  nrf_gpio_cfg_output(BSP_PMU_LED_EN);

  err_code = drv_ws2815_init(&(drv_ws2815_conf_t) {
    .sck_pin   = BSP_I2S_SCK,
    .lrck_pin  = BSP_I2S_LRCK,
    .sdout_pin = BSP_LED_DRV,
  });
  APP_ERROR_CHECK(err_code);

  NRF_LOG_INFO("WS2815 application started.");

  nrfx_systick_state_t systick;
  nrfx_systick_get(&systick);

  m_i2s_start = true;
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

    if (m_i2s_running) {
      if (nrfx_systick_test(&systick, UPDATE_TIME_US) == true) {
        nrfx_systick_get(&systick);
        update_framebuffer = true;
      }

      if (update_framebuffer == true && drv_ws2815_framebuffer_is_busy() == false) {
        update_framebuffer = false;
        set_led_data();
      }
    }
  }
}

static void set_led_data(void) {
  static uint32_t time = 0;
  static uint32_t next_time = 0;
  uint32_t delay_ms;

  _effect = _effects[_effects_sel].effect;
  _effect_func = _effects[_effects_sel].func;

  if (time >= next_time) {
    _effect_func(_effect, &delay_ms);
    drv_ws2815_framebuffer_commit();

    next_time = time + delay_ms;
    _effect->counter++;
  }

  time += UPDATE_TIME_US / 1000;
}

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info) {
  bsp_board_leds_on();
  app_error_save_and_stop(id, pc, info);
}
