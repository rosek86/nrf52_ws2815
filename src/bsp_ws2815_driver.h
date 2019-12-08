#ifndef BSP_WS2815_DRIVER_H__
#define BSP_WS2815_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

// LEDs definitions
#define LEDS_NUMBER    4

#define LED1_G         NRF_GPIO_PIN_MAP(0,13)
#define LED2_R         NRF_GPIO_PIN_MAP(0,14)
#define LED2_G         NRF_GPIO_PIN_MAP(0,15)
#define LED2_B         NRF_GPIO_PIN_MAP(0,16)

#define LED_1          LED1_G
#define LED_2          LED2_R
#define LED_3          LED2_G
#define LED_4          LED2_B

#define LED_START      LED_1
#define LED_STOP       LED_4

#define LEDS_ACTIVE_STATE 0

#define LEDS_LIST { LED_1, LED_2, LED_3, LED_4 }

#define LEDS_INV_MASK  LEDS_MASK

#define BSP_LED_0      LED_1
#define BSP_LED_1      LED_2
#define BSP_LED_2      LED_3
#define BSP_LED_3      LED_4

#define BUTTONS_NUMBER 1

#define BUTTON_1       NRF_GPIO_PIN_MAP(1,2)
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1 }

#define BSP_BUTTON_0   BUTTON_1

#define RX_PIN_NUMBER  NRF_GPIO_PIN_MAP(0, 6)
#define TX_PIN_NUMBER  NRF_GPIO_PIN_MAP(0, 8)
#define CTS_PIN_NUMBER NRF_GPIO_PIN_MAP(0, 5)
#define RTS_PIN_NUMBER NRF_GPIO_PIN_MAP(0, 7)
#define HWFC           true

#define BSP_SELF_PINRESET_PIN NRF_GPIO_PIN_MAP(0,19)

#define BSP_DBG1       NRF_GPIO_PIN_MAP(1, 13)
#define BSP_DBG2       NRF_GPIO_PIN_MAP(1, 15)
#define BSP_DBG3       NRF_GPIO_PIN_MAP(0,  2)

#define BSP_PMU_LED_EN NRF_GPIO_PIN_MAP(0, 26)
#define BSP_LED_DRV    NRF_GPIO_PIN_MAP(0, 12)

#define BSP_ENV_CS     NRF_GPIO_PIN_MAP(0, 24)
#define BSP_ENV_SCK    NRF_GPIO_PIN_MAP(0, 20)
#define BSP_ENV_SDI    NRF_GPIO_PIN_MAP(0, 22)
#define BSP_ENV_SDO    NRF_GPIO_PIN_MAP(1,  0)

#define BSP_I2S_SCK    NRF_GPIO_PIN_MAP(1,  10)
#define BSP_I2S_LRCK   NRF_GPIO_PIN_MAP(1,  11)

#ifdef __cplusplus
}
#endif

#endif // BSP_WS2815_DRIVER_H__
