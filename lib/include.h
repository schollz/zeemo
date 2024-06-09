// stdlib
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// pico stdlib
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "hardware/structs/clocks.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
// zeemo lib
#include "WS2812.h"
#include "adsr.h"
#include "dac.h"
#include "filterexp.h"
#include "knob_change.h"
// zeemo imports (order matters!)
#include "globals.h"
//
#include "zeemo.h"
Zeemo zeemo;
//
#include "button_handler.h"
#include "leds2.h"
//
#include "screen.h"