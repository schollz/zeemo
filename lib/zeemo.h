#ifndef LIB_ZEEMO_H
#define LIB_ZEEMO_H 1

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