#include "WrapperLedControl.h"

void WrapperLedControl::begin(ConfigLed ledConfig) {
	//typeLedDefine(ledConfig.ledType);
	_ledCount = ledConfig.nbLed;

	leds = new CRGB[_ledCount];
	_fire2012Heat = new byte[_ledCount];

	#if !defined CONFIG_LED_PWM
	switch (ledConfig.ledType) {
		case LT_LPD6803:
			FastLED.addLeds<LPD6803, CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_LPD8806:
			FastLED.addLeds<LPD8806, CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_WS2801:
			FastLED.addLeds<WS2801, CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_WS2803:
			FastLED.addLeds<WS2803, CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_SM16716:
			FastLED.addLeds<SM16716, CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_P9813:
			FastLED.addLeds<P9813, CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_SK9822:
			FastLED.addLeds<SK9822, CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_DOTSTAR:
			FastLED.addLeds<DOTSTAR, CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;

		case LT_NEOPIXEL:
			FastLED.addLeds<NEOPIXEL, CONFIG_LED_DATAPIN>(leds, _ledCount);
			break;
		case LT_WS2811:
			FastLED.addLeds<WS2811, CONFIG_LED_DATAPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_WS2812:
			FastLED.addLeds<WS2811, CONFIG_LED_DATAPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_WS2812B:
			FastLED.addLeds<WS2812B, CONFIG_LED_DATAPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_WS2813:
			FastLED.addLeds<WS2813, CONFIG_LED_DATAPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_APA104:
			FastLED.addLeds<APA104, CONFIG_LED_DATAPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_APA106:
			FastLED.addLeds<APA106, CONFIG_LED_DATAPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_SK6812:
			FastLED.addLeds<SK6812, CONFIG_LED_DATAPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
		case LT_SK6822:
			FastLED.addLeds<SK6822, CONFIG_LED_DATAPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;

		case LT_HD107S:
		case LT_APA102:
		default:
			FastLED.addLeds<APA102, CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
			break;
	}
	if (ledConfig.ledType <= 9) {
		Log.debug("Chipset=%s, dataPin=%i, clockPin=%i, colorOrder=%i, ledCount=%i",
				  LedTypeName[ledConfig.ledType].c_str(),
				  CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER, _ledCount);
	} else {
		Log.debug("Chipset=%s, dataPin=%i, clockPin=%s, colorOrder=%i, ledCount=%i",
				  LedTypeName[ledConfig.ledType].c_str(), CONFIG_LED_DATAPIN, "NONE", CONFIG_LED_COLOR_ORDER,
				  CONFIG_LED_COUNT);
	}

	Log.info("CFG=%s (%s)", "Led chipset initialisation!", LedTypeName[ledConfig.ledType].c_str());
	#ifdef FASTLED_RGBW
	Log.info("CFG=%s", "RGBW Enable");
	#endif
	#else
	Log.debug("Chipset=%s, redPin=%i, greenPin=%i, bluePin=%i, ledCount=%i", "PWM", CONFIG_LED_PWM_RED,
			  CONFIG_LED_PWM_GREEN, CONFIG_LED_PWM_BLUE, CONFIG_LED_COUNT);
			  #if _ledCount != 1
			  #error "PWM only supports LED count set to one (even if you have multiple LEDs on your strip, they will all show the same color)"
			  #endif
	#endif
}

/*void WrapperLedControl::typeLedDefine(uint8_t typeLed) {
	#undef CONFIG_LED_CLOCKLESS_CHIPSET
	#undef CONFIG_LED_SPI_CHIPSET
	switch (typeLed) {
		case LT_LPD6803:
			#define CONFIG_LED_SPI_CHIPSET LPD6803
			break;
		case LT_LPD8806:
			#define CONFIG_LED_SPI_CHIPSET LPD8806
			break;
		case LT_WS2801:
			#define CONFIG_LED_SPI_CHIPSET WS2801
			break;
		case LT_WS2803:
			#define CONFIG_LED_SPI_CHIPSET WS2803
			break;
		case LT_SM16716:
			#define CONFIG_LED_SPI_CHIPSET SM16716
			break;
		case LT_P9813:
			#define CONFIG_LED_SPI_CHIPSET P9813
			break;
		case LT_SK9822:
			#define CONFIG_LED_SPI_CHIPSET SK9822
			break;
		case LT_DOTSTAR:
			#define CONFIG_LED_SPI_CHIPSET DOTSTAR
			break;

		case LT_NEOPIXEL:
			#define CONFIG_LED_CLOCKLESS_CHIPSET NEOPIXEL
			break;
		case LT_WS2811:
			#define CONFIG_LED_CLOCKLESS_CHIPSET WS2811
			break;
		case LT_WS2812:
			#define CONFIG_LED_CLOCKLESS_CHIPSET WS2812
			break;
		case LT_WS2812B:
			#define CONFIG_LED_CLOCKLESS_CHIPSET WS2812B
			break;
		case LT_WS2813:
			#define CONFIG_LED_CLOCKLESS_CHIPSET WS2813
			break;
		case LT_APA104:
			#define CONFIG_LED_CLOCKLESS_CHIPSET APA104
			break;
		case LT_APA106:
			#define CONFIG_LED_CLOCKLESS_CHIPSET APA106
			break;
		case LT_SK6812:
			#define CONFIG_LED_CLOCKLESS_CHIPSET SK6812
			break;
		case LT_SK6822:
			#define CONFIG_LED_CLOCKLESS_CHIPSET SK6822
			break;

		case LT_HD107S:
		case LT_APA102:
		default:
			#define CONFIG_LED_SPI_CHIPSET APA102
			break;
	}

	Log.info("CFG=%s (%s)", "Led chipset initialisation!", LedTypeName[typeLed].c_str());
}*/

void WrapperLedControl::show(void) {
	#if defined CONFIG_LED_PWM
	analogWrite(CONFIG_LED_PWM_RED, map(leds[0].red, 0, 255, 0, PWMRANGE));
	analogWrite(CONFIG_LED_PWM_GREEN, map(leds[0].green, 0, 255, 0, PWMRANGE));
	analogWrite(CONFIG_LED_PWM_BLUE, map(leds[0].blue, 0, 255, 0, PWMRANGE));
	#else
	FastLED.show();
	#endif
}

void WrapperLedControl::clear(void) {
	#if defined CONFIG_LED_PWM
	leds[0] = CRGB::Black;
	#else
	FastLED.clear();
	#endif
}

void WrapperLedControl::fillSolid(CRGB color) {
	fill_solid(leds, _ledCount, color);
	show();
}

void WrapperLedControl::fillSolid(byte r, byte g, byte b) {
	fillSolid(CRGB(r, g, b));
}

void WrapperLedControl::rainbowStep(void) {
	for (int i = 0; i < _ledCount; i++) {
		leds[i] = wheel((i + _rainbowStepState) % 255);
	}
	show();

	if (_rainbowStepState < 255) {
		_rainbowStepState++;
	} else {
		_rainbowStepState = 0;
	}
}

CRGB WrapperLedControl::wheel(byte wheelPos) {
	CRGB color = CRGB();
	if (wheelPos < 85) {
		return color.setRGB(wheelPos * 3, 255 - wheelPos * 3, 0);
	} else if (wheelPos < 170) {
		wheelPos -= 85;
		return color.setRGB(255 - wheelPos * 3, 0, wheelPos * 3);
	} else {
		wheelPos -= 170;
		return color.setRGB(0, wheelPos * 3, 255 - wheelPos * 3);
	}
	return color;
}

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  50

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

void WrapperLedControl::fire2012Step(void) {
	// Step 1.  Cool down every cell a little
	for (int i = 0; i < _ledCount; i++) {
		_fire2012Heat[i] = qsub8(_fire2012Heat[i], random8(0, ((COOLING * 10) / _ledCount) + 2));
	}

	// Step 2.  _fire2012Heat from each cell drifts 'up' and diffuses a little
	for (int k = _ledCount - 1; k >= 2; k--) {
		_fire2012Heat[k] = (_fire2012Heat[k - 1] + _fire2012Heat[k - 2] + _fire2012Heat[k - 2]) / 3;
	}

	// Step 3.  Randomly ignite new 'sparks' of _fire2012Heat near the bottom
	if (random8() < SPARKING) {
		int y = random8(min(7, _ledCount - 1));
		_fire2012Heat[y] = qadd8(_fire2012Heat[y], random8(160, 255));
	}

	// Step 4.  Map from _fire2012Heat cells to LED colors
	for (int j = 0; j < _ledCount; j++) {
		CRGB color = HeatColor(_fire2012Heat[j]);
		int pixelnumber;
		if (_fire2012Direction) {
			pixelnumber = (_ledCount - 1) - j;
		} else {
			pixelnumber = j;
		}
		leds[pixelnumber] = color;
	}
	show();
}
