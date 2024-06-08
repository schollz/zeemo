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

enum View {
  VIEW_MAIN,
  VIEW_CHORD,
  VIEW_VOICE_1,
  VIEW_VOICE_2,
  VIEW_VOICE_3,
  VIEW_VOICE_4,
};

typedef struct Zeemo {
  enum View view;
  uint8_t subview;
  Sequence seq[5][4];
  int16_t bpm;
  bool recording;
} Zeemo;

Zeemo *Zeemo_malloc() {
  Zeemo *self = (Zeemo *)malloc(sizeof(Zeemo));
  self->bpm = 60;
  self->view = VIEW_MAIN;
  return self;
}

void Zeemo_free(Zeemo *self) { free(self); }

void Zeemo_change_view(Zeemo *self, enum View view) {
  self->view = view;
  self->subview = 0;
  self->recording = false;
}

void Zeemo_change_subview(Zeemo *self, uint8_t subview) {
  self->subview = subview;
  self->recording = false;
}

void Zeemo_start_recording(Zeemo *self) { self->recording = true; }

#endif