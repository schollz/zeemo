// definitions
#define I2C_SDA0_PIN 20
#define I2C_SCL0_PIN 21
#define I2C_SDA1_PIN 14
#define I2C_SCL1_PIN 15
#define WS2812_PIN 11
#define WS2812_SM 2
#define WS2812_NUM_LEDS 8
#define BTN_ONBOARD 23
#define BTN_ROW_START 0
#define BTN_COL_START 5
#define LED_NONE 0
#define LED_DIM 1
#define LED_BRIGHT 2
#define LED_BLINK 3
#define LED_1_GPIO 9
#define LED_2_GPIO 19
#define LED_3_GPIO 29
#define LED_4_GPIO 24
#define LED_TOP_GPIO 25
#define DURATION_HOLD 500
#define DURATION_HOLD_LONG 1250

// globals
WS2812 *ws2812;
DAC *dac;
ADSR *adsr[4];

// utilities
typedef void (*callback_fn_uint8_t_uint32_t)(uint8_t, uint32_t);
typedef void (*callback_fn_uint8_t)(uint8_t);

// utility functions
#define util_clamp(x, a, b) ((x) > (b) ? (b) : ((x) < (a) ? (a) : (x)))

#define linlin(x, xmin, xmax, ymin, ymax)                                 \
  util_clamp((ymin + (x - xmin) * (ymax - ymin) / (xmax - xmin)), (ymin), \
             (ymax))
