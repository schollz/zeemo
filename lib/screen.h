
LEDS *leds;

void screen_init() {
  leds = LEDS_create();
  for (uint8_t i = 0; i < 20; i++) {
    LEDS_set(leds, i, LED_BLINK);
  }
  LEDS_render(leds);
}

void screen_update(ButtonMatrix *bm) {
  LEDS_clear(leds);
  for (uint8_t i = 0; i < 4; i++) {
    if (i == zeemo.subview) {
      if (zeemo.recording) {
        LEDS_set(leds, i, LED_BLINK);
      } else {
        LEDS_set(leds, i, LED_BRIGHT);
      }
    } else {
      LEDS_set(leds, i, LED_NONE);
    }
  }
  // int8_t led_sequence =
  //     SimpleSequence_leds(&zeemo.seq[zeemo.view][zeemo.subview]);
  // if (led_sequence >= 0) {
  //   LEDS_set(leds, led_sequence, LED_DIM);
  // }
  if (zeemo.playing[zeemo.view][zeemo.subview] >= 0)
    LEDS_set(leds, zeemo.playing[zeemo.view][zeemo.subview], LED_BRIGHT);

  // illuminate pressed buttons
  for (uint8_t i = 0; i < 20; i++) {
    if (bm->button_time[i] > 0 && LEDS_is_on(leds, i) == false) {
      LEDS_set(leds, i, LED_BRIGHT);
    }
  }

  LEDS_render(leds);

  uint8_t note_brightness[4] = {255, 255, 255, 255};
  uint8_t note_env[4] = {0, 0, 0, 0};
  for (uint8_t i = 0; i < 4; i++) {
    note_brightness[i] = linlin(zeemo.last_note[0] % 12, 0, 11, 0, 255);
    note_env[i] = adsr[i]->level * 255;
  }

  switch (zeemo.view) {
    case VIEW_MAIN:
      WS2812_set_color(ws2812, 0, WS2812_RED, note_brightness[0]);
      WS2812_set_color(ws2812, 1, WS2812_RED, note_env[0]);
      WS2812_set_color(ws2812, 2, WS2812_YELLOW, note_brightness[1]);
      WS2812_set_color(ws2812, 3, WS2812_YELLOW, note_env[1]);
      WS2812_set_color(ws2812, 4, WS2812_GREEN, note_brightness[2]);
      WS2812_set_color(ws2812, 5, WS2812_GREEN, note_env[2]);
      WS2812_set_color(ws2812, 6, WS2812_BLUE, note_brightness[3]);
      WS2812_set_color(ws2812, 7, WS2812_BLUE, note_env[3]);
      break;
    case VIEW_CHORD:
      WS2812_set_color(ws2812, 0, WS2812_MAGENTA, 255);
      WS2812_set_color(ws2812, 1, WS2812_MAGENTA, 255);
      WS2812_set_color(ws2812, 2, WS2812_MAGENTA, 255);
      WS2812_set_color(ws2812, 3, WS2812_MAGENTA, 255);
      WS2812_set_color(ws2812, 4, WS2812_MAGENTA, 255);
      WS2812_set_color(ws2812, 5, WS2812_MAGENTA, 255);
      WS2812_set_color(ws2812, 6, WS2812_MAGENTA, 255);
      WS2812_set_color(ws2812, 7, WS2812_MAGENTA, 255);
      break;
    case VIEW_VOICE_1:
      WS2812_set_color(ws2812, 0, WS2812_RED, note_brightness[0]);
      WS2812_set_color(ws2812, 1, WS2812_RED, note_env[0]);
      WS2812_set_color(ws2812, 2, WS2812_YELLOW, 0);
      WS2812_set_color(ws2812, 3, WS2812_YELLOW, 0);
      WS2812_set_color(ws2812, 4, WS2812_GREEN, 0);
      WS2812_set_color(ws2812, 5, WS2812_GREEN, 0);
      WS2812_set_color(ws2812, 6, WS2812_BLUE, 0);
      WS2812_set_color(ws2812, 7, WS2812_BLUE, 0);
      break;
    case VIEW_VOICE_2:
      WS2812_set_color(ws2812, 0, WS2812_RED, 0);
      WS2812_set_color(ws2812, 1, WS2812_RED, 0);
      WS2812_set_color(ws2812, 2, WS2812_YELLOW, note_brightness[1]);
      WS2812_set_color(ws2812, 3, WS2812_YELLOW, note_env[1]);
      WS2812_set_color(ws2812, 4, WS2812_GREEN, 0);
      WS2812_set_color(ws2812, 5, WS2812_GREEN, 0);
      WS2812_set_color(ws2812, 6, WS2812_BLUE, 0);
      WS2812_set_color(ws2812, 7, WS2812_BLUE, 0);
      break;
    case VIEW_VOICE_3:
      WS2812_set_color(ws2812, 0, WS2812_RED, 0);
      WS2812_set_color(ws2812, 1, WS2812_RED, 0);
      WS2812_set_color(ws2812, 2, WS2812_YELLOW, 0);
      WS2812_set_color(ws2812, 3, WS2812_YELLOW, 0);
      WS2812_set_color(ws2812, 4, WS2812_GREEN, note_brightness[2]);
      WS2812_set_color(ws2812, 5, WS2812_GREEN, note_env[2]);
      WS2812_set_color(ws2812, 6, WS2812_BLUE, 0);
      WS2812_set_color(ws2812, 7, WS2812_BLUE, 0);
      break;
    case VIEW_VOICE_4:
      WS2812_set_color(ws2812, 0, WS2812_RED, 0);
      WS2812_set_color(ws2812, 1, WS2812_RED, 0);
      WS2812_set_color(ws2812, 2, WS2812_YELLOW, 0);
      WS2812_set_color(ws2812, 3, WS2812_YELLOW, 0);
      WS2812_set_color(ws2812, 4, WS2812_GREEN, 0);
      WS2812_set_color(ws2812, 5, WS2812_GREEN, 0);
      WS2812_set_color(ws2812, 6, WS2812_BLUE, note_brightness[3]);
      WS2812_set_color(ws2812, 7, WS2812_BLUE, note_env[3]);
      break;
    default:
      break;
  }
  WS2812_show(ws2812);
}