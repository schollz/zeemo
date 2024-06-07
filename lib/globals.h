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

// globals
WS2812 *ws2812;
DAC *dac;
Zeemo *zeemo;
