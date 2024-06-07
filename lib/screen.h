
LEDS *leds;

void screen_init() {
  leds = LEDS_create();
  for (uint8_t i = 0; i < 20; i++) {
    LEDS_set(leds, i, LED_BLINK);
  }
  LEDS_render(leds);
}

void screen_update() { LEDS_render(leds); }