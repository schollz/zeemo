#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
//
// #include "bsp/board.h"
// #include "tusb.h"
//
#include "lib/adsr.h"
// #include "lib/midi_comm.h"
// #include "lib/midi_out.h"

// globals

ADSR *adsrs[2];

// https://github.com/raspberrypi/pico-examples/blob/master/i2c/bus_scan/bus_scan.c
bool reserved_addr(uint8_t addr) {
  return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

#define I2C_SDA0_PIN 20
#define I2C_SCL0_PIN 21
#define I2C_SDA1_PIN 14
#define I2C_SCL1_PIN 15

void test_mcp4728() {
  printf("\nI2C Bus Scan\n");
  printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

  for (int addr = 0; addr < (1 << 7); ++addr) {
    if (addr % 16 == 0) {
      printf("%02x ", addr);
    }

    int ret;
    uint8_t rxdata;
    if (reserved_addr(addr))
      ret = PICO_ERROR_GENERIC;
    else
      ret = i2c_read_timeout_us(i2c0, addr, &rxdata, 1, false, 20000);

    printf(ret < 0 ? "." : "@");
    printf(addr % 16 == 15 ? "\n" : "  ");
  }
  for (int addr = 0; addr < (1 << 7); ++addr) {
    if (addr % 16 == 0) {
      printf("%02x ", addr);
    }

    int ret;
    uint8_t rxdata;
    if (reserved_addr(addr))
      ret = PICO_ERROR_GENERIC;
    else
      ret = i2c_read_timeout_us(i2c1, addr, &rxdata, 1, false, 20000);

    printf(ret < 0 ? "." : "@");
    printf(addr % 16 == 15 ? "\n" : "  ");
  }
  printf("Done.\n");

  // setup array of bytes
  uint8_t data[3] = {0b01000110, 0b00000000, 0b10111000};
  printf("writing data\n");
  printf("wrote %d bytes\n", i2c_write_blocking(i2c0, 0x60, data, 3, false));
  printf("wrote %d bytes\n", i2c_write_blocking(i2c0, 0x60, data, 3, false));
  while (1) {
    // printf("Hello, world!\n");
    sleep_ms(1000);
  }
}

float voltages_set[8] = {
    0, 0, 0, 0, 0, 0, 0, 0,
};

void write_voltage_reference() {
  // set to Internal Reference (2.048V)
  // page 43 http://ww1.microchip.com/downloads/en/devicedoc/22187e.pdf
  uint8_t data = 0b10001111;
  i2c_write_blocking(i2c0, 0x60, &data, 1, false);
  sleep_ms(1);
  i2c_write_blocking(i2c1, 0x60, &data, 1, false);
  sleep_ms(1);

  // set to Gain of 2 so Vref = 4.096V
  // page 44 http://ww1.microchip.com/downloads/en/devicedoc/22187e.pdf
  data = 0b11001111;
  i2c_write_blocking(i2c0, 0x60, &data, 1, false);
  sleep_ms(1);
  i2c_write_blocking(i2c1, 0x60, &data, 1, false);
  sleep_ms(1);
}

void update_voltages() {
  // page 38 http://ww1.microchip.com/downloads/en/devicedoc/22187e.pdf
  uint8_t data[8];
  for (int i = 0; i < 4; i++) {
    uint16_t value = (uint16_t)round(voltages_set[i] * 4095.0 / 4.096);
    data[i * 2] = 0b00000000 | (value >> 8);
    data[i * 2 + 1] = value & 0xff;
  }
  printf("[update_voltages] %d %d %d %d\n", data[0], data[1], data[2], data[3]);
  uint8_t ret = i2c_write_blocking(i2c0, 0x60, data, 8, false);
  printf("[update_voltages] written %d\n", ret);
  sleep_us(500);
  for (int i = 4; i < 8; i++) {
    uint16_t value = (uint16_t)round(voltages_set[i] * 4095.0 / 4.096);
    data[(i - 4) * 2] = 0b00000000 | (value >> 8);
    data[(i - 4) * 2 + 1] = value & 0xff;
  }
  i2c_write_blocking(i2c1, 0x60, data, 8, false);
}

void set_voltage(uint8_t ch, float voltage) { voltages_set[ch] = voltage; }

float midi2cv(uint8_t note) { return ((float)note - 32.0) / 12.0; }

uint16_t random_integer(uint16_t low, uint16_t high) {
  return low + (rand() % (high - low));
}

// keep track of two midi notes
uint8_t notes_playing[2] = {0, 0};

void midi_callback(uint8_t status, uint8_t channel, uint8_t note,
                   uint8_t velocity) {
  printf("status %x channel %x note %x velocity %x\n", status, channel, note,
         velocity);
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

#define BUTTON_PIN 23
  gpio_init(BUTTON_PIN);
  gpio_set_dir(BUTTON_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_PIN);

  sleep_ms(1000);

  bool button_on = false;
  uint16_t debounce_startup = 1000;
  while (true) {
    // read button
    if (gpio_get(BUTTON_PIN) != button_on) {
      button_on = !button_on;
      if (button_on) {
        test_mcp4728();
      } else {
        printf("[main] button %d\n", button_on);
      }
    }

    if (debounce_startup > 0) {
      debounce_startup--;
      if (debounce_startup == 0) {
        // startup stuff
        write_voltage_reference();
        for (int i = 0; i < 8; i++) {
          set_voltage(i, 3.1415);
        }
        update_voltages();
      }
    }
    sleep_ms(1);
  }
  return 0;
}
