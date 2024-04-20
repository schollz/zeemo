#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
//
#include "bsp/board.h"
#include "tusb.h"
//
#include "lib/midi_comm.h"
#include "lib/midi_out.h"

// https://github.com/raspberrypi/pico-examples/blob/master/i2c/bus_scan/bus_scan.c
bool reserved_addr(uint8_t addr) {
  return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

#define I2C_SDA0_PIN 4
#define I2C_SCL0_PIN 5
#define I2C_SDA1_PIN 10
#define I2C_SCL1_PIN 11

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

void set_voltage_(uint8_t ch, float voltage) {
  // convert voltage float to 12-bit value
  uint16_t value = (uint16_t)round(voltage * 4095.0 / 5.0);
  // http://ww1.microchip.com/downloads/en/devicedoc/22187e.pdf
  // page 41
  // setup array of bytes
  uint8_t data[3];
  uint8_t ch2 = ch;
  if (ch2 > 3) ch2 = ch2 - 4;
  if (ch2 == 0) {
    data[0] = 0b00011110;
  } else if (ch2 == 1) {
    data[0] = 0b00011100;
  } else if (ch2 == 2) {
    data[0] = 0b00011010;
  } else if (ch2 == 3) {
    data[0] = 0b00011000;
  }
  data[1] = 0b00000000;
  // put the msb 4 bits of the 12-bit into the last 4 bits of the 2nd byte
  data[1] = data[1] | (value >> 8);
  // put the lsb 8 bits of the 12-bit into the 3rd byte
  data[2] = value & 0xff;
  printf("writing %2.3f volts %d data %x%x%x\n", voltage, value, data[0],
         data[1], data[2]);
  if (ch < 4) {
    printf("wrote %d bytes\n", i2c_write_blocking(i2c0, 0x60, data, 3, false));
  } else {
    printf("wrote %d bytes\n", i2c_write_blocking(i2c1, 0x60, data, 3, false));
  }
}

void update_voltages() {
  // page 38 http://ww1.microchip.com/downloads/en/devicedoc/22187e.pdf
  uint8_t data[8];
  for (int i = 0; i < 4; i++) {
    uint16_t value = (uint16_t)round(voltages_set[i] * 4095.0 / 4.096);
    data[i * 2] = 0b00000000 | (value >> 8);
    data[i * 2 + 1] = value & 0xff;
  }
  i2c_write_blocking(i2c0, 0x60, data, 8, false);
}

void set_voltage(uint8_t ch, float voltage) { voltages_set[ch] = voltage; }

float midi2cv(uint8_t note) { return ((float)note - 32.0) / 12.0; }

uint16_t random_integer(uint16_t low, uint16_t high) {
  return low + (rand() % (high - low));
}

int main() {
  stdio_init_all();

  sleep_ms(1000);

  // This example will use i2c0 on the default SDA and SCL (pins 6, 7 on a Pico)
  i2c_init(i2c0, 100 * 1000);
  gpio_set_function(I2C_SDA0_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL0_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA0_PIN);
  gpio_pull_up(I2C_SCL0_PIN);
  i2c_init(i2c1, 100 * 1000);
  gpio_set_function(I2C_SDA1_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL1_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA1_PIN);
  gpio_pull_up(I2C_SCL1_PIN);

// setup button gpio 14
#define BUTTON_PIN 14
  gpio_init(BUTTON_PIN);
  gpio_set_dir(BUTTON_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_PIN);

  sleep_ms(1000);

  printf("zeemo\n");

#define NOTES_TOTAL 16
  int notes[NOTES_TOTAL] = {
      // cmajor
      60, 64, 67, 69, 59, 62, 65, 67, 57, 60, 64, 65, 55, 59, 62, 64,
  };

  // endless loop
  int note_index = -1;

  bool button_on = false;

  float periods[3] = {8232, 9021, 12123};  // milliseconds
  tusb_init();

  while (true) {
    tud_task();
    midi_comm_task(NULL);

    // set_voltage(0, midi2voltage(48));
    // sleep_ms(5000);
    // set_voltage(0, midi2voltage(60));
    // sleep_ms(5000);
    note_index++;
    uint8_t note = notes[note_index % NOTES_TOTAL];
    float voltage = midi2cv(note);
    set_voltage(0, voltage);
    uint16_t wait_time = random_integer(250, 2520);
    for (uint16_t j = 0; j < wait_time; j++) {
      uint32_t current_time = to_ms_since_boot(get_absolute_time());
      for (int i = 0; i < 3; i++) {
        float val =
            (sin(2 * 3.14159 * (float)current_time / (float)periods[i]) + 1) *
            2.048;
        set_voltage(i + 1, val);
      }
      update_voltages();
      sleep_ms(1);

      // read button
      if (1 - gpio_get(BUTTON_PIN) != button_on) {
        button_on = !button_on;
        printf("button %d\n", button_on);
      }
    }
  }
  return 0;
}
