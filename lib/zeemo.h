#ifndef LIB_ZEEMO_H
#define LIB_ZEEMO_H 1
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "sequence.h"

// 6 views total (main, chord, voice 1, voice 2, voice 3, voice 4)
// 5 views have sequencers (chord, voice 1, voice 2, voice 3, voice 4)
// 4 subviews per view, each with a sequencer
typedef struct Zeemo {
  uint8_t view;
  uint8_t subview;
  Sequence seq[5][4];
  int16_t bpm;
} Zeemo;

Zeemo *Zeemo_malloc() {
  Zeemo *self = (Zeemo *)malloc(sizeof(Zeemo));
  self->bpm = 60;
  return self;
}

void Zeemo_free(Zeemo *self) { free(self); }

#endif