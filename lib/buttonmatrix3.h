// Copyright 2023-2024 Zack Scholl.
//
// Author: Zack Scholl (zack.scholl@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.

#ifndef BUTTONMATRIX3_H
#define BUTTONMATRIX3_H 1

#include "buttonmatrix3.pio.h"
#include "sort.h"

#define BUTTONMATRIX_BUTTONS_MAX 20
#define BUTTONMATRIX_ROWS 5
#define BUTTONMATRIX_COLS 4

typedef struct ButtonMatrix {
  PIO pio;
  uint sm;
  uint32_t last_value;
  uint8_t mapping[BUTTONMATRIX_BUTTONS_MAX];
  uint32_t button_time[BUTTONMATRIX_BUTTONS_MAX];
  bool button_hold_emit[BUTTONMATRIX_BUTTONS_MAX];
  bool button_hold_long_emit[BUTTONMATRIX_BUTTONS_MAX];
  callback_fn_uint8_t fn_button_on;
  callback_fn_uint8_t fn_button_held;
  callback_fn_uint8_t fn_button_held_long;
  callback_fn_uint8_t_uint32_t fn_button_off;
} ButtonMatrix;

void ButtonMatrix_dec_to_binary(ButtonMatrix *bm, uint32_t num) {
  if (num == 0) {
    printf("\n");
  }
  for (uint8_t i = 0; i < BUTTONMATRIX_BUTTONS_MAX; i++) {
    // if ((num >> i) & 1) {
    //   printf("%d ", bm->mapping[i]);
    // }
    uint8_t bit = (num >> i) & 1;
    printf("%u", bit);  // Print the bit
  }
  printf("\n");
}

void ButtonMatrix_init(ButtonMatrix *bm, callback_fn_uint8_t fn_button_on,
                       callback_fn_uint8_t fn_button_held,
                       callback_fn_uint8_t fn_button_held_long,
                       callback_fn_uint8_t_uint32_t fn_button_off) {
  bm->fn_button_on = fn_button_on;
  bm->fn_button_off = fn_button_off;
  bm->fn_button_held = fn_button_held;
  bm->fn_button_held_long = fn_button_held_long;
}

ButtonMatrix *ButtonMatrix_malloc(uint base_input, uint base_output) {
  ButtonMatrix *bm = (ButtonMatrix *)malloc(sizeof(ButtonMatrix));
  bm->pio = pio0;
  bm->sm = 1;
  bm->last_value = 0;

  for (int i = 0; i < BUTTONMATRIX_ROWS; i++) {
    pio_gpio_init(bm->pio, base_input + i);
    gpio_pull_down(base_input + i);
  }

  for (int i = 0; i < BUTTONMATRIX_COLS; i++) {
    pio_gpio_init(bm->pio, base_output + i);
  }

  bm->mapping[0] = 3;
  bm->mapping[1] = 7;
  bm->mapping[2] = 11;
  bm->mapping[3] = 15;
  bm->mapping[4] = 19;
  bm->mapping[5] = 2;
  bm->mapping[6] = 6;
  bm->mapping[7] = 10;
  bm->mapping[8] = 14;
  bm->mapping[9] = 18;
  bm->mapping[10] = 1;
  bm->mapping[11] = 5;
  bm->mapping[12] = 9;
  bm->mapping[13] = 13;
  bm->mapping[14] = 17;
  bm->mapping[15] = 0;
  bm->mapping[16] = 4;
  bm->mapping[17] = 8;
  bm->mapping[18] = 12;
  bm->mapping[19] = 16;

  for (uint8_t i = 0; i < BUTTONMATRIX_BUTTONS_MAX; i++) {
    bm->button_time[i] = 0;
    bm->button_hold_emit[i] = false;
    bm->button_hold_long_emit[i] = false;
  }

  uint offset = pio_add_program(bm->pio, &button_matrix_program);
  pio_sm_config c = button_matrix_program_get_default_config(offset);
  sm_config_set_in_pins(&c, base_input);
  pio_sm_set_consecutive_pindirs(bm->pio, bm->sm, base_output,
                                 BUTTONMATRIX_COLS, true);
  sm_config_set_set_pins(&c, base_output, BUTTONMATRIX_COLS);
  sm_config_set_in_shift(&c, 0, 0, 0);  // Corrected the shift setup
  pio_sm_init(bm->pio, bm->sm, offset, &c);
  pio_sm_set_enabled(bm->pio, bm->sm, true);

  return bm;
}

void ButtonMatrix_read(ButtonMatrix *bm) {
  // read new value;
  uint32_t value = 0;
  pio_sm_clear_fifos(bm->pio, bm->sm);
  sleep_ms(1);
  if (pio_sm_is_rx_fifo_empty(bm->pio, bm->sm)) {
    printf("fifo empty\n");
    return;
  }
  value = pio_sm_get(bm->pio, bm->sm);

  if (value == bm->last_value) {
    // check for holdings
    uint32_t now = to_ms_since_boot(get_absolute_time());
    for (uint8_t i = 0; i < BUTTONMATRIX_BUTTONS_MAX; i++) {
      uint8_t j = bm->mapping[i];
      if ((value >> i) & 1) {
        if (now - bm->button_time[j] > DURATION_HOLD &&
            !bm->button_hold_emit[j]) {
          if (bm->fn_button_held != NULL) {
            bm->fn_button_held(j);
          }
          bm->button_hold_emit[j] = true;
        } else if (now - bm->button_time[j] > DURATION_HOLD_LONG &&
                   !bm->button_hold_long_emit[j]) {
          if (bm->fn_button_held_long != NULL) {
            bm->fn_button_held_long(j);
          }
          bm->button_hold_long_emit[j] = true;
        }
      }
    }
    return;
  }
  for (uint8_t i = 0; i < BUTTONMATRIX_BUTTONS_MAX; i++) {
    uint8_t j = bm->mapping[i];
    if ((value >> i) & 1) {
      if (bm->button_time[j] == 0) {
        // button turned off to on
        bm->button_time[j] = to_ms_since_boot(get_absolute_time());
        if (bm->fn_button_on != NULL) {
          bm->fn_button_on(j);
        }
      }
    } else if (bm->button_time[j] > 0) {
      // button turned on to off
      if (bm->fn_button_off != NULL) {
        bm->fn_button_off(
            j, to_ms_since_boot(get_absolute_time()) - bm->button_time[j]);
      }
      bm->button_time[j] = 0;
      bm->button_hold_emit[j] = false;
      bm->button_hold_long_emit[j] = false;
    }
  }

  bm->last_value = value;
}

#endif