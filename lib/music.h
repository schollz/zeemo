#ifndef LIB_MUSIC_H
#define LIB_MUSIC_H 1

/* SuperCollider code for generating scales
(
("const uint8_t scale_major[28]={").postln;
4.do({arg i;
        var v = Scale.major.degrees;
        v.do({ arg v;
                ((v+(i*12)).asString++",").postln;
        })
});
("};").postln;
("const uint8_t scale_minor[28]={").postln;
4.do({arg i;
        var v = Scale.minor.degrees;
        v.do({ arg v;
                ((v+(i*12)).asString++",").postln;
        })
});
("};").postln;
)
*/
const uint8_t scale_major[28] = {
    0,  2,  4,  5,  7,  9,  11, 12, 14, 16, 17, 19, 21, 23,
    24, 26, 28, 29, 31, 33, 35, 36, 38, 40, 41, 43, 45, 47,
};
const uint8_t scale_minor[28] = {
    0,  2,  3,  5,  7,  8,  10, 12, 14, 15, 17, 19, 20, 22,
    24, 26, 27, 29, 31, 32, 34, 36, 38, 39, 41, 43, 44, 46,
};
#endif