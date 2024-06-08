#ifndef LIB_ZEEMO_H
#define LIB_ZEEMO_H 1
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

void Sequence_print(Sequence *self) {
  for (uint8_t i = 0; i < self->len; i++) {
    printf("%d %d\n", self->vals[i], self->ticks[i]);
  }
}

int8_t Sequence_emit(Sequence *self, uint32_t tick) {
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
typedef struct Zeemo {
  int16_t bpm;
} Zeemo;

Zeemo *Zeemo_malloc() {
  Zeemo *self = (Zeemo *)malloc(sizeof(Zeemo));
  self->bpm = 60;
  return self;
}

void Zeemo_free(Zeemo *self) { free(self); }

#endif