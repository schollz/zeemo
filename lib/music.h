#ifndef LIB_MUSIC_H
#define LIB_MUSIC_H 1

/* SuperCollider code for generating scales
(
("const uint8_t scale_major[14]={").postln;
2.do({arg i;
        var v = Scale.major.degrees;
        v.do({ arg v;
                ((v+(i*12)).asString++",").postln;
        })
});
("};").postln;
("const uint8_t scale_minor[14]={").postln;
2.do({arg i;
        var v = Scale.minor.degrees;
        v.do({ arg v;
                ((v+(i*12)).asString++",").postln;
        })
});
("};").postln;
)
*/
const uint8_t scale_major[14] = {
    0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23,
};
const uint8_t scale_minor[14] = {
    0, 2, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 20, 22,
};

typedef struct Chord {
  uint8_t notes[4];
  bool is_major;
} Chord;

Chord Chord_new(uint8_t kind, bool is_major) {
  Chord c;
  c.is_major = is_major;
  kind--;
  if (is_major) {
    // 1, 2, 3, 4, 5, 6, 7 chords ->
    // I, ii, iii, IV, V, vi, vii
    c.notes[0] = scale_major[kind];
    c.notes[1] = scale_major[kind + 2];
    c.notes[2] = scale_major[kind + 4];
    c.notes[3] = scale_major[kind + 6];
  } else {
    // 1, 2, 3, 4, 5, 6, 7 chords ->
    // i, ii, III, iv, v, VI, VII
    c.notes[0] = scale_minor[kind];
    c.notes[1] = scale_minor[kind + 2];
    c.notes[2] = scale_minor[kind + 4];
    c.notes[3] = scale_minor[kind + 6];
  }
  return c;
}

#endif