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

#include "buttonmatrix3.h"

// keys
#define KEY_A 0
#define KEY_B 1
#define KEY_C 2
#define KEY_D 3

ButtonMatrix *bm;
uint32_t tap_tempo_last = 0;
uint8_t tap_tempo_hits = 0;
uint8_t key_held_num = 0;
bool key_held_on = false;
int32_t key_timer = 0;
int32_t key_timer_on = 0;
uint32_t key_timer_first = 0;
uint8_t key_pressed[100];
uint8_t key_pressed_num = 0;
uint8_t key_total_pressed = 0;
int16_t key_on_buttons[BUTTONMATRIX_BUTTONS_MAX];
int16_t key_on_buttons_last[BUTTONMATRIX_BUTTONS_MAX];
bool key_did_go_off[BUTTONMATRIX_BUTTONS_MAX];
uint16_t key_num_presses;
bool KEY_C_sample_select = false;

void button_handler_init() {
  bm = ButtonMatrix_create(BTN_ROW_START, BTN_COL_START);
}

bool is_button_pressed(uint8_t key) { return key_on_buttons[key] > 0; }

int8_t is_single_button_pressed() {
  uint8_t pressed = 0;
  int8_t val = -1;
  for (uint8_t i = 4; i < BUTTONMATRIX_BUTTONS_MAX; i++) {
    if (key_on_buttons[i]) {
      pressed++;
      val = i - 4;
    }
  }
  if (pressed == 1) {
    return val;
  }
  return -1;
}

void button_key_off_held(uint8_t key) { printf("[bh] off held %d\n", key); }

void button_key_off_any(uint8_t key) { printf("[bh] off any %d\n", key); }

void button_key_on_single(uint8_t key) { printf("[bh] on single %d\n", key); }

void button_key_on_double(uint8_t key1, uint8_t key2) {
  printf("[bh] on double %d+%d\n", key1, key2);
}

bool btn_onboard_value = false;

void button_handler() {
  // read button

  bool btn_onboard = gpio_get(BTN_ONBOARD);
  if (btn_onboard != btn_onboard_value) {
    btn_onboard_value = btn_onboard;
    printf("[bh] onboard %d\n", btn_onboard);
    if (btn_onboard_value == 1) {
      if (zeemo->view != VIEW_CHORD) {
        zeemo->view = VIEW_CHORD;
      } else {
        zeemo->view = VIEW_MAIN;
      }
      printf("view: %d\n", zeemo->view);
    }
  }

  if (key_total_pressed == 0) {
    key_timer++;
    if (key_timer_first > 0) {
      uint32_t duration =
          to_ms_since_boot(get_absolute_time()) - key_timer_first;
      if (duration > 500) {
        if (key_pressed_num == 1) {
          printf("[bh] hold %d\n", key_pressed[0]);
          if (key_pressed[0] < 4) {
            zeemo->subview = key_pressed[0];
          }
        }
      } else {
        if (key_pressed_num == 1) {
          printf("[btc] tap %d\n", key_pressed[0]);
          if (key_pressed[0] < 4) {
            zeemo->view = key_pressed[0] + 2;
          }
        }
      }
      key_timer_first = 0;
    }
  }
  if (key_timer == 5 && key_pressed_num > 0) {
    // create string
    char key_pressed_str[256];
    int pos = snprintf(key_pressed_str, sizeof(key_pressed_str),
                       "[bh] (t=%ld) combo: ", key_timer_on);

    // Ensure the snprintf was successful and within the buffer size
    if (pos >= 0 && pos < sizeof(key_pressed_str)) {
      for (uint8_t i = 0; i < key_pressed_num; i++) {
        // Calculate remaining space in the buffer
        int remaining = sizeof(key_pressed_str) - pos;
        if (remaining > 0) {
          int ret =
              snprintf(key_pressed_str + pos, remaining, "%d ", key_pressed[i]);
          // Check if snprintf was successful
          if (ret < 0 || ret >= remaining) {
            // Handle error (e.g., truncate string, log error, etc.)
            key_pressed_str[sizeof(key_pressed_str) - 1] = '\0';
            break;
          }
          pos += ret;
        } else {
          // No space left in the buffer
          break;
        }
      }
      printf("[bh] %s\n", key_pressed_str);
    }

    // combo matching
    if (key_pressed_num == 3) {
      if (key_pressed[0] == 8 && key_pressed[1] == 9 && key_pressed[2] == 8) {
        printf("[bh] combo: 8 9 8!!!\n");
      }
    }

    key_timer = 0;
    key_pressed_num = 0;
  }

  // read the latest from the queue
  ButtonMatrix_read(bm);

  // check queue for buttons that turned off
  for (uint8_t i = 0; i < BUTTONMATRIX_BUTTONS_MAX; i++) {
    key_did_go_off[i] = false;
  }
  for (uint8_t i = 0; i < bm->off_num; i++) {
    key_total_pressed--;
    key_on_buttons_last[bm->off[i]] = key_on_buttons[bm->off[i]];
    key_on_buttons[bm->off[i]] = 0;
    key_did_go_off[bm->off[i]] = true;
    button_key_off_any(bm->off[i]);
    // printf("turned off %d\n", bm->off[i]);
    if (key_held_on && (bm->off[i] == key_held_num)) {
      button_key_off_held(bm->off[i]);

      key_held_on = false;
      // TODO:
      // use the last key pressed that is still held as the new hold
      if (key_total_pressed > 0) {
        uint16_t *indexes =
            sort_int16_t(key_on_buttons, BUTTONMATRIX_BUTTONS_MAX);
        key_held_on = true;
        key_held_num = indexes[BUTTONMATRIX_BUTTONS_MAX - 1];
        free(indexes);
      } else {
        key_num_presses = 0;
        for (uint8_t i = 0; i < BUTTONMATRIX_BUTTONS_MAX; i++) {
          key_on_buttons[i] = 0;
        }
      }
    } else if (key_held_on) {
      printf("[bh] off %d+%d\n", key_held_num, bm->off[i]);
    } else {
      printf("[bh] off %d\n", bm->off[i]);
    }
  }

  // check queue for buttons that turned on
  for (uint8_t i = 0; i < bm->on_num; i++) {
    key_total_pressed++;
    if (key_total_pressed == 1) {
      key_timer_on = 0;
    }
    if (!key_held_on) {
      key_held_on = true;
      key_held_num = bm->on[i];
      button_key_on_single(bm->on[i]);
    } else {
      button_key_on_double(key_held_num, bm->on[i]);
    }

    // keep track of combos
    key_pressed[key_pressed_num] = bm->on[i];
    if (key_pressed_num < 100) {
      key_pressed_num++;
    }
    key_timer = 0;

    // keep track of all
    key_num_presses++;
    key_on_buttons_last[bm->on[i]] = key_on_buttons[bm->on[i]];
    key_on_buttons[bm->on[i]] = key_num_presses;
  }

  if (key_total_pressed > 0) {
    key_timer_on++;
    if (key_timer_first == 0) {
      key_timer_first = to_ms_since_boot(get_absolute_time());
    }
  }
}
