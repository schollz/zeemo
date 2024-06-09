#ifndef LIB_ZEEMO_H
#define LIB_ZEEMO_H 1
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "dac.h"
#include "sequence.h"
#include "simplesequence.h"

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
  SimpleSequence seq[6][4];
  int16_t bpm;
  bool recording;
  bool mode_tuning;
} Zeemo;

void Zeemo_init(Zeemo *self) {
  self->bpm = 60;
  self->view = VIEW_VOICE_1;
  self->subview = 0;

  for (uint8_t i = 0; i < 5; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      SimpleSequence_init(&self->seq[i][j]);
    }
  }

  SimpleSequence_add(&self->seq[self->view][self->subview], 4);
  SimpleSequence_add(&self->seq[self->view][self->subview], 5);
  SimpleSequence_add(&self->seq[self->view][self->subview], 6);
  SimpleSequence_add(&self->seq[self->view][self->subview], 7);
  SimpleSequence_add(&self->seq[self->view][self->subview], 8);
  SimpleSequence_add(&self->seq[self->view][self->subview], 7);
  SimpleSequence_add(&self->seq[self->view][self->subview], 6);
  SimpleSequence_add(&self->seq[self->view][self->subview], 5);
}

void Zeemo_change_view(Zeemo *self, enum View view) {
  self->view = view;
  self->subview = 0;
  self->recording = false;
}

void Zeemo_change_subview(Zeemo *self, uint8_t subview) {
  self->subview = subview;
  self->recording = false;
}

void Zeemo_start_recording(Zeemo *self) {
  self->recording = true;
  SimpleSequence_clear(&self->seq[self->view][self->subview]);
  printf("[zeemo] recording %d\n", self->recording);
}

void Zeemo_toggle_tuning_mode(Zeemo *self) {
  self->mode_tuning = !self->mode_tuning;
  self->recording = false;
  printf("[zeemo] tuning mode %d\n", self->mode_tuning);
}

void Zeemo_update(Zeemo *self) {
  if (self->mode_tuning) {
    for (uint8_t ch = 0; ch < 4; ch++) {
      // set to 2 volts
      DAC_set_voltage(dac, (ch * 2), 2);
      // set to high (4 volts)
      DAC_set_voltage(dac, (ch * 2) + 1, 4);
    }
    DAC_update(dac);
    return;
  }
}

void Zeemo_press(Zeemo *self, uint8_t key) {
  if (self->recording && key > 3) {
    printf("[zeemo] recording key %d\n", key);
    SimpleSequence_add(&self->seq[self->view][self->subview], key);
  }
}

#endif