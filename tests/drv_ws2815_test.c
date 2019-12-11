#include <stdio.h>
#include <stdint.h>

#include "../src/drv_ws2815.h"

int main(void) {
  uint32_t buffer[3];

  uint32_t value;
  drv_ws2815_from_rgb_value(0x00123456, buffer, 0);
  drv_ws2815_to_rgb_value(buffer, 0, &value);
  printf("%08x\n", value);

  uint8_t r, g, b;
  drv_ws2815_from_rgb(0x12, 0x34, 0x56, buffer, 0);
  drv_ws2815_to_rgb(buffer, 0, &r, &g, &b);
  printf("00%02x%02x%02x\n", r, g, b);

  return 0;
}
