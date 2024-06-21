#include "lib/include.h"

struct repeating_timer timer;
int16_t bpm_last = 100;
uint64_t bpm_timer_counter = 0;
bool down_beat = false;
bool repeating_timer_callback(struct repeating_timer *t) {
  if (bpm_last != zeemo.bpm) {
    printf("[rtc] updating bpm timer: %d-> %d\n", bpm_last, zeemo.bpm);
    bpm_last = zeemo.bpm;
    cancel_repeating_timer(&timer);
    add_repeating_timer_us(-(round(30000000 / zeemo.bpm / 96)),
                           repeating_timer_callback, NULL, &timer);
  }

  bpm_timer_counter++;
  if (bpm_timer_counter % 96 == 0) {
    // printf("[rtc] 96th note: %lld\n", bpm_timer_counter);
    down_beat = !down_beat;
    gpio_put(LED_TOP_GPIO, down_beat);
  }

  Zeemo_tick(&zeemo, bpm_timer_counter);
  return true;
}

int main() {
  stdio_init_all();

  sleep_ms(100);

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

  sleep_ms(10);

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
  ButtonMatrix *bm = ButtonMatrix_malloc(BTN_ROW_START, BTN_COL_START);
  button_handler_init(bm);

  // setup LEDs screen
  screen_init();

  // setup zeemo
  Zeemo_init(&zeemo);

  // setup adcs
  adc_init();
  adc_gpio_init(26);
  adc_gpio_init(27);
  adc_gpio_init(28);
  KnobChange *knob_change[3];
  FilterExp *filter_exp[3];
  for (uint8_t i = 0; i < 3; i++) {
    knob_change[i] = KnobChange_malloc(100);
    filter_exp[i] = FilterExp_create(10);
  }

  // setup adsrs
  for (uint8_t i = 0; i < 4; i++) {
    if (i == 0) {
      adsr[i] = ADSR_malloc(1500, 100, 1, 2000, 2.7);

    } else {
      adsr[i] = ADSR_malloc(500, 100, 1, 200, 2.7);
    }
  }

  // setup timer
  // Negative delay so means we will call repeating_timer_callback, and call
  // it again 500ms later regardless of how long the callback took to execute
  // add_repeating_timer_ms(-1000, repeating_timer_callback, NULL, &timer);
  // cancel_repeating_timer(&timer);
  add_repeating_timer_us(-(round(30000000 / zeemo.bpm / 96)),
                         repeating_timer_callback, NULL, &timer);

  uint16_t debounce_startup = 100;
  while (true) {
    // don't do anything before the startup
    if (debounce_startup > 0) {
      debounce_startup--;
      if (debounce_startup == 0) {
        printf("[main] startup complete\n");
      }
      sleep_ms(1);
      continue;
    }

    for (uint8_t knob = 0; knob < 3; knob++) {
      adc_select_input(knob);
      uint16_t adc_raw = adc_read();
      int16_t adc = KnobChange_update(knob_change[knob], adc_raw);
      if (adc > 0) {
        adc = FilterExp_update(filter_exp[knob], adc);
        printf("[main] knob %d: %d\n", knob, adc);
        switch (zeemo.view) {
          case VIEW_MAIN:
            switch (knob) {
              case 0:
                break;
              case 1:
                zeemo.bpm = adc * 240 / 4095 + 60;
                break;
              case 2:
                break;
            }
            break;
          default:
            break;
        }
      }
    }

    Zeemo_update(&zeemo);

    uint32_t ct = to_ms_since_boot(get_absolute_time());
    for (uint8_t i = 0; i < 4; i++) {
      ADSR_process(adsr[i], (float)ct);
    }

    button_handler(bm);

    screen_update(bm);

    sleep_us(100);
  }
  return 0;
}
