#ifndef BaseHeader_h
#define BaseHeader_h

#include <Arduino.h>
#include <Logging.h>
#include "Config.h"

#define min(a,b) ((a)<(b)?(a):(b))
/*
 #define min(X, Y)                \
 ({ typeof (X) x_ = (X);          \
    typeof (Y) y_ = (Y);          \
    (x_ < y_) ? x_ : y_; })*/

enum Mode { NONE, OFF, HYPERION_UDP, STATIC_COLOR, RAINBOW, FIRE2012 };

enum LedType {
	LT_LPD6803 = 0,
	LT_LPD8806 = 1,
	LT_WS2801 = 2,
	LT_WS2803 = 3,
	LT_SM16716 = 4,
	LT_P9813 = 5,
	LT_APA102 = 6,
	LT_HD107S = 7,
	LT_SK9822 = 8,
	LT_DOTSTAR = 9,

	LT_NEOPIXEL = 10,
	LT_WS2811 = 11,
	LT_WS2812 = 12,
	LT_WS2812B = 13,
	LT_WS2813 = 14,
	LT_APA104 = 15,
	LT_APA106 = 16,
	LT_SK6812 = 17,
	LT_SK6822 = 18
};
const uint8_t nbLedType = 19;
// Same order that enum Ledtype!
const String LedTypeName[nbLedType] = {
        "LPD6803",
        "LPD8806",
        "WS2801",
        "WS2803",
        "SM16716",
        "P9813",
        "APA102",
        "HD107s",
        "SK9822",
        "DOTSTAR",

        "NEOPIXEL",
        "WS2811",
        "WS2812",
        "WS2812B",
        "WS2813",
        "APA104",
        "APA106",
        "SK6812",
        "SK6822"};


#endif

