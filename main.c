// stdlib
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// pico stdlib
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
// zeemo lib
#include "lib/WS2812.h"
#include "lib/adsr.h"
#include "lib/dac.h"

// zeemo imports (order matters!)
#include "lib/globals.h"
//
#include "lib/button_handler.h"
#include "lib/leds2.h"
//
#include "lib/screen.h"

int main() {
  stdio_init_all();

  sleep_ms(1000);

  // This example will use i2c0 on the default SDA and SCL (pins 6, 7 on a Pico)
  i2c_init(i2c0, 50 * 1000);
  gpio_set_function(I2C_SDA0_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL0_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA0_PIN);
  gpio_pull_up(I2C_SCL0_PIN);
  i2c_init(i2c1, 50 * 1000);
  gpio_set_function(I2C_SDA1_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL1_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA1_PIN);
  gpio_pull_up(I2C_SCL1_PIN);

  gpio_init(BTN_ONBOARD);
  gpio_set_dir(BTN_ONBOARD, GPIO_IN);
  gpio_pull_up(BTN_ONBOARD);

  sleep_ms(1000);

  // setup WS2812
  ws2812 = WS2812_new(WS2812_PIN, pio0, WS2812_SM, WS2812_NUM_LEDS);
  WS2812_set_brightness(ws2812, 50);
  for (int i = 0; i < WS2812_NUM_LEDS; i++) {
    WS2812_fill(ws2812, i, 150, 0, 255);
  }
  WS2812_show(ws2812);

  // setup DAC
  dac = DAC_malloc();

  // setup button handler
  button_handler_init();

  // setup LEDs screen
  screen_init();

  bool button_on = false;
  uint16_t debounce_startup = 1000;
  while (true) {
    // don't do anything before the startup
    if (debounce_startup > 0) {
      debounce_startup--;
      if (debounce_startup == 0) {
        DAC_set_voltage_update(dac, 0, 0.5);
        printf("[main] startup complete\n");
      }
      sleep_ms(1);
      continue;
    }

    // read button
    if (gpio_get(BTN_ONBOARD) != button_on) {
      button_on = !button_on;
      DAC_set_voltage_update(dac, 0, dac->voltages[0] + 0.1);
      printf("[main] button %d\n", button_on);
    }

    button_handler();

    screen_update();

    sleep_us(100);
  }
  return 0;
}
