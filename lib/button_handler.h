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

#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H 1

#include "buttonmatrix3.h"

// keys
#define KEY_A 0
#define KEY_B 1
#define KEY_C 2
#define KEY_D 3

bool btn_onboard_value = 0;

void button_held_fn(uint8_t key) {
  printf("[bh] held %d\n", key);
  if (key < 4) {
    Zeemo_change_subview(&zeemo, key);
  }
}

void button_held_long_fn(uint8_t key) {
  printf("[bh] held long %d\n", key);
  if (key < 4) {
    if (zeemo.view >= VIEW_CHORD) {
      Zeemo_start_recording(&zeemo);
    } else if (zeemo.view == VIEW_MAIN) {
      if (key == 3) {
        Zeemo_toggle_tuning_mode(&zeemo);
      }
    }
  }
}

void button_on_fn(uint8_t key) {
  printf("[bh] key %d down\n", key);
  if (key >= 4) {
    Zeemo_press(&zeemo, key);
  }
}

void button_off_fn(uint8_t key, uint32_t held_time) {
  printf("[bh] key %d up, held %ld\n", key, held_time);
  if (held_time < DURATION_HOLD) {
    if (key < 4) {
      Zeemo_change_view(&zeemo, VIEW_VOICE_1 + key);
    }
  }
}

void button_handler_init(ButtonMatrix *bm) {
  ButtonMatrix_init(bm, button_on_fn, button_held_fn, button_held_long_fn,
                    button_off_fn);
}

void button_handler(ButtonMatrix *bm) {
  // read onboard button
  bool btn_onboard = gpio_get(BTN_ONBOARD);
  if (btn_onboard != btn_onboard_value) {
    btn_onboard_value = btn_onboard;
    printf("[bh] onboard %d\n", btn_onboard);
    if (btn_onboard_value == 1) {
      if (zeemo.view != VIEW_CHORD) {
        zeemo.view = VIEW_CHORD;
      } else {
        zeemo.view = VIEW_MAIN;
      }
      printf("view: %d\n", zeemo.view);
    }
  }

  // read the latest from the queue
  ButtonMatrix_read(bm);
}

#endif
