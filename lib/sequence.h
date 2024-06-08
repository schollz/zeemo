#ifndef SEQUENCE_LIB_H
#define SEQUENCE_LIB_H 1

#define SEQUENCE_NO_EMIT -120

typedef struct Sequence {
  int8_t vals[128];
  uint16_t ticks[128];
  uint16_t total_ticks;
  uint8_t len;
} Sequence;

void Sequence_init(Sequence *self) {
  self->total_ticks = 0;
  self->len = 0;
}

void Sequence_add(Sequence *self, int8_t val, uint16_t ticks) {
  self->vals[self->len] = val;
  self->ticks[self->len] = ticks;
  self->total_ticks += ticks;
  self->len++;
}

void Sequence_clear(Sequence *self) {
  self->total_ticks = 0;
  self->len = 0;
}

void Sequence_print(Sequence *self) {
  for (uint8_t i = 0; i < self->len; i++) {
    printf("%d %d\n", self->vals[i], self->ticks[i]);
  }
}

int8_t Sequence_emit(Sequence *self, uint32_t tick) {
  if (self->len == 0) {
    return SEQUENCE_NO_EMIT;
  }
  tick = tick % self->total_ticks;
  uint32_t t = 0;
  for (uint8_t i = 0; i < self->len; i++) {
    if (tick == t) {
      return self->vals[i];
    }
    t += self->ticks[i];
  }
  return SEQUENCE_NO_EMIT;
}

#endif