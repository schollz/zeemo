// stdlib
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// pico stdlib
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "hardware/structs/clocks.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
// zeemo lib
#include "lib/WS2812.h"
#include "lib/adsr.h"
#include "lib/dac.h"
#include "lib/zeemo.h"
// zeemo imports (order matters!)
#include "lib/globals.h"
//
#include "lib/button_handler.h"
#include "lib/leds2.h"
//
#include "lib/screen.h"

struct repeating_timer timer;
int16_t bpm_last = 100;
uint64_t bpm_timer_counter = 0;
bool down_beat = false;
bool repeating_timer_callback(struct repeating_timer *t) {
  if (bpm_last != zeemo->bpm) {
    printf("[rtc] updating bpm timer: %d-> %d\n", bpm_last, zeemo->bpm);
    bpm_last = zeemo->bpm;
    cancel_repeating_timer(&timer);
    add_repeating_timer_us(-(round(30000000 / zeemo->bpm / 96)),
                           repeating_timer_callback, NULL, &timer);
  }

  bpm_timer_counter++;
  if (bpm_timer_counter % 96 == 0) {
    printf("[rtc] 96th note: %lld\n", bpm_timer_counter);
    down_beat = !down_beat;
    gpio_put(LED_TOP_GPIO, down_beat);
  }
  return true;
}

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

  gpio_init(LED_TOP_GPIO);
  gpio_set_dir(LED_TOP_GPIO, GPIO_OUT);

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

  // setup zeemo
  zeemo = Zeemo_malloc();

  // setup timer
  // Negative delay so means we will call repeating_timer_callback, and call
  // it again 500ms later regardless of how long the callback took to execute
  // add_repeating_timer_ms(-1000, repeating_timer_callback, NULL, &timer);
  // cancel_repeating_timer(&timer);
  add_repeating_timer_us(-(round(30000000 / zeemo->bpm / 96)),
                         repeating_timer_callback, NULL, &timer);

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

    button_handler();

    screen_update();

    sleep_us(100);
  }
  return 0;
}
