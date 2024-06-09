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

enum VoiceSel {
  NOTE_VAL,
  NOTE_DUR,
  MODU_VAL,
  MODU_DUR,
};

const uint32_t note_dur_vals[20] = {
    0,        0,       0,       0,       192 / 32, 192 / 24, 192 / 16,
    192 / 12, 192 / 8, 192 / 6, 192 / 4, 192 / 3,  192 / 2,  192,
    192 * 2,  192 * 3, 192 * 4, 192 * 6, 192 * 8};

typedef struct Zeemo {
  enum View view;
  uint8_t subview;
  SimpleSequence seq[6][4];
  int8_t playing[6][4];
  int16_t bpm;
  bool recording;
  bool mode_tuning;
} Zeemo;

void Zeemo_init(Zeemo *self) {
  self->bpm = 60;
  self->view = VIEW_VOICE_1;
  self->subview = 0;

  for (uint8_t i = 0; i < 6; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      SimpleSequence_init(&self->seq[i][j]);
      self->playing[i][j] = -1;
    }
  }

  SimpleSequence_add(&self->seq[self->view][NOTE_VAL], 4);
  SimpleSequence_add(&self->seq[self->view][NOTE_VAL], 11);
  SimpleSequence_add(&self->seq[self->view][NOTE_VAL], 6);
  SimpleSequence_add(&self->seq[self->view][NOTE_VAL], 11);
  SimpleSequence_add(&self->seq[self->view][NOTE_VAL], 8);
  SimpleSequence_add(&self->seq[self->view][NOTE_VAL], 11);
  SimpleSequence_add(&self->seq[self->view][NOTE_VAL], 6);
  SimpleSequence_add(&self->seq[self->view][NOTE_VAL], 11);

  // durations
  SimpleSequence_add(&self->seq[self->view][NOTE_DUR], 10);

  SimpleSequence_reset(&self->seq[self->view][NOTE_VAL]);
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

int8_t Zeemo_duration_index(Zeemo *self, uint8_t i, uint8_t j,
                            uint64_t total_ticks) {
  // find the total duration
  uint64_t dur_ticks = 0;
  for (uint8_t k = 0; k < 4; k++) {
    dur_ticks += note_dur_vals[self->seq[i][j].vals[k]];
  }
  dur_ticks = total_ticks % dur_ticks;
  int8_t index = -1;
  uint64_t dur_total = 0;
  for (uint8_t k = 0; k < self->seq[i][j].len; k++) {
    if (dur_total == dur_ticks) {
      index = k;
      break;
    }
    dur_total += note_dur_vals[self->seq[i][j].vals[k]];
  }
  return index;
}

void Zeemo_tick(Zeemo *self, uint64_t total_ticks) {
  // go through each voice
  for (uint8_t i = 0; i < 4; i++) {
    // skip voices that have no notes
    if (self->seq[VIEW_VOICE_1 + i][NOTE_VAL].len == 0 ||
        self->seq[VIEW_VOICE_1 + i][NOTE_DUR].len == 0) {
      continue;
    }
    int8_t index =
        Zeemo_duration_index(self, VIEW_VOICE_1 + i, NOTE_DUR, total_ticks);
    if (index == -1) {
      continue;
    }
    self->playing[VIEW_VOICE_1 + i][NOTE_DUR] =
        self->seq[VIEW_VOICE_1 + i][NOTE_DUR].vals[index];
    self->playing[VIEW_VOICE_1 + i][NOTE_VAL] =
        SimpleSequence_next(&self->seq[VIEW_VOICE_1 + i][NOTE_VAL]);
    // printf("[zeemo] voice %d, index: %d, note_val: %d\n", i, index,
    //        self->playing[VIEW_VOICE_1 + i][NOTE_VAL]);
  }
}

void Zeemo_press(Zeemo *self, uint8_t key) {
  if (self->recording && key > 3) {
    printf("[zeemo] recording key %d\n", key);
    SimpleSequence_add(&self->seq[self->view][self->subview], key);
  }
}

#endif