#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../music.h"
#include "../../zeemo.h"

int main() {
  Sequence seq;
  Sequence_init(&seq);
  Sequence_add(&seq, 1, 10);
  Sequence_add(&seq, 2, 10);
  Sequence_add(&seq, 3, 15);
  Sequence_print(&seq);
  for (uint32_t i = 9; i < 60; i++) {
    int8_t val = Sequence_emit(&seq, i);
    if (val != SEQUENCE_NO_EMIT) {
      printf("%d) %d\n", i, Sequence_emit(&seq, i));
    }
  }
  return 0;
}
