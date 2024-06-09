#ifndef SIMPLESEQUENCE_LIB_H
#define SIMPLESEQUENCE_LIB_H 1

typedef struct SimpleSequence {
  int8_t vals[128];
  uint8_t i;
  uint8_t len;
  uint8_t show_i;
  uint32_t show_last;
} SimpleSequence;

void SimpleSequence_init(SimpleSequence *self) {
  self->len = 0;
  self->i = 0;
  self->show_i = 0;
  self->show_last = to_ms_since_boot(get_absolute_time());
}

// SimpleSequence_leds shows the sequence in a loop
// with blinking values
int8_t SimpleSequence_leds(SimpleSequence *self) {
  if (self->len == 0) {
    return -1;
  }
  uint32_t ct = to_ms_since_boot(get_absolute_time());
  uint32_t diff = ct - self->show_last;
  int8_t val = -1;
  uint16_t max_diff = 200;
  uint16_t min_diff = max_diff * 2 / 3;
  if (self->show_i == self->len - 1) {
    max_diff += 350;
  }
  if (diff > 2000) {
    self->show_i = 0;
    self->show_last = ct;
  } else if (diff > max_diff) {
    self->show_i++;
    if (self->show_i >= self->len) {
      self->show_i = 0;
    }
    self->show_last = ct;
  } else if (diff < min_diff) {
    val = self->vals[self->show_i];
  }
  return val;
}

void SimpleSequence_clear(SimpleSequence *self) {
  self->i = 0;
  self->len = 0;
}

void SimpleSequence_add(SimpleSequence *self, int8_t val) {
  self->vals[self->len] = val;
  self->len++;
}

int8_t SimpleSequence_next(SimpleSequence *self) {
  if (self->len == 0) {
    return -1;
  }
  int8_t v = self->vals[self->i];
  self->i++;
  if (self->i >= self->len) {
    self->i = 0;
  }
  return v;
}

#endif