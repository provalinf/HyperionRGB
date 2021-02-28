#include <Arduino.h>

#include <Thread.h>
#include <ThreadController.h>
#include "BaseHeader.h"

#include "EnhancedThread.h"

#include "LoggerInit.h"

#include "WrapperWiFi.h"
#include "WrapperOTA.h"
#include "WrapperLedControl.h"
#include "WrapperUdpLed.h"
#include "WrapperJsonServer.h"

#include <WiFiManager.h>
#include "WrapperWebconfig.h"

#if defined(ESP8266)
#define LED LED_BUILTIN
#elif defined(ESP32)
#define LED 2 // LED in NodeMCU at pin GPIO16 (D0) or LED_BUILTIN @Lolin32.
#endif
int ledState = LOW;

LoggerInit loggerInit;

WrapperWiFi wifi;
WrapperOTA ota;

WrapperLedControl ledStrip;

WrapperUdpLed udpLed;
WrapperJsonServer jsonServer;

#if defined(ESP32)
WebServer server(80);
#endif

WiFiManager wifiManager;
WrapperWebconfig webServer;

Mode activeMode = NONE;
boolean autoswitch, initDone;

ThreadController threadController = ThreadController();
Thread statusThread = Thread();
EnhancedThread animationThread = EnhancedThread();
EnhancedThread resetThread = EnhancedThread();

int ledAllumeeCompteur = 3;

void statusInfo(void) {
	if (ledAllumeeCompteur <= 0) {
		threadController.remove(&statusThread);
		interrupts();
	}
	ledAllumeeCompteur--;

	if (ledState == LOW) {
		ledState = HIGH;
	} else {
		ledState = LOW;
		Log.debug("HEAP=%i", ESP.getFreeHeap());
	}
	digitalWrite(LED, ledState);
}

void animationStep() {
	switch (activeMode) {
		case RAINBOW:
			ledStrip.rainbowStep();
			break;
		case FIRE2012:
			ledStrip.fire2012Step();
			break;
	}
}

void changeMode(Mode newMode, int interval = 0) {
	if (newMode != activeMode) {
		Log.info("Mode changed to %i", newMode);
		activeMode = newMode;
		if (!autoswitch)
			udpLed.stop();

		switch (activeMode) {
			case OFF:
				ledStrip.clear();
				ledStrip.show();
				break;
			case STATIC_COLOR:
				break;
			case RAINBOW:
				if (interval == 0)
					interval = 500;
				animationThread.setInterval(interval);
				break;
			case FIRE2012:
				if (interval == 0)
					interval = 16;
				animationThread.setInterval(interval);
				break;
			case HYPERION_UDP:
				if (!autoswitch && initDone)
					udpLed.begin();
		}
		if (interval > 0)
			Log.debug("Interval set to %ims", interval);
	}
}

void updateLed(int id, byte r, byte g, byte b) {
	if (activeMode == HYPERION_UDP) {
		Log.verbose("LED %i, r=%i, g=%i, b=%i", id + 1, r, g, b);
		ledStrip.leds[id].setRGB(r, g, b);
	}
}

void refreshLeds(void) {
	if (activeMode == HYPERION_UDP) {
		Log.debug("refresh LEDs");
		ledStrip.show();
		if (autoswitch)
			resetThread.reset();
	} else if (autoswitch) {
		changeMode(HYPERION_UDP);
		Log.info("Autoswitch to HYPERION_UDP");
	}
}

void ledColorWipe(byte r, byte g, byte b) {
	Log.debug("LED color wipe: r=%i, g=%i, b=%i", r, g, b);
	changeMode(STATIC_COLOR);
	ledStrip.fillSolid(r, g, b);
}

void resetMode(void) {
	Log.info("Reset Mode");
	changeMode(static_cast<Mode>(Config::getConfig()->led.idleMode));

	if (CONFIG_LED_STANDARD_MODE == OFF) {
		Log.info("Inactivity loop");
		resetThread.enabled = true;
		wifi.reconnect();
	}
}

void initConfig(void) {
	const char *ssid;
	const char *password;

	ConfigStruct *cfg = Config::getConfig();
	Config::logConfig();

	ssid = cfg->wifi.ssid;
	password = cfg->wifi.password;
	autoswitch = cfg->led.autoswitch;

	wifiManager.autoConnect(cfg->wifi.hostname);

	if (strcmp(wifiManager.getWiFiSSID(true).c_str(), ssid) != 0
		|| strcmp(wifiManager.getWiFiPass(true).c_str(), password) != 0) {
		strcpy(cfg->wifi.ssid, wifiManager.getWiFiSSID(true).c_str());
		strcpy(cfg->wifi.password, wifiManager.getWiFiPass(true).c_str());
		Config::saveEEPROMConfig();
	}

	Log.info("WifiManager SSID %s", wifiManager.getWiFiSSID(true).c_str());
	Log.debug("WifiManager PASSWD %s", wifiManager.getWiFiPass(true).c_str());

	wifi = WrapperWiFi(ssid, password, Config::cfg2ip(cfg->wifi.ip), Config::cfg2ip(cfg->wifi.subnet),
					   Config::cfg2ip(cfg->wifi.dns));
	udpLed = WrapperUdpLed(cfg->led.nbLed, cfg->ports.udpLed);
	jsonServer = WrapperJsonServer(cfg->led.nbLed, cfg->ports.jsonServer);
}

void handleEvents(void) {
	ota.handle();
	udpLed.handle();
	jsonServer.handle();

	#ifdef CONFIG_ENABLE_WEBCONFIG
	webServer.handle();
	#endif

	threadController.run();
}

void setup(void) {
	initDone = false;
	LoggerInit loggerInit = LoggerInit(115200);

	initConfig();
	ota = WrapperOTA();
	ledStrip = WrapperLedControl();

	statusThread.onRun(statusInfo);
	statusThread.setInterval(5000);
	threadController.add(&statusThread);

	animationThread.onRun(animationStep);
	animationThread.setInterval(1000);

	resetThread.onRun(resetMode);
	resetThread.setInterval(Config::getConfig()->led.timeoutMs);
	resetThread.enabled = false;
	threadController.add(&resetThread);

	ledStrip.begin(Config::getConfig()->led);
	resetMode();
	animationStep();

	wifi.begin();

//  uint8_t mac[6];
//  esp_efuse_mac_get_default(mac);
//  Serial.begin(115200);
//  Serial.printf("\nAdresse mac : %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	webServer = WrapperWebconfig();
	#if defined(ESP32)
	webServer._server = &server;
	#endif
	webServer.begin();
	#if !defined(CONFIG_ENABLE_WEBCONFIG)
	webServer._server->close();
	#endif

	ota.begin(Config::getConfig()->wifi.hostname);

	if (autoswitch || activeMode == HYPERION_UDP)
		udpLed.begin();

	udpLed.onUpdateLed(updateLed);
	udpLed.onRefreshLeds(refreshLeds);

	jsonServer.begin();
	jsonServer.onLedColorWipe(ledColorWipe);
	jsonServer.onClearCmd(resetMode);
	jsonServer.onEffectChange(changeMode);

	pinMode(LED, OUTPUT);   // LED pin as output.
	Log.info("HEAP=%i", ESP.getFreeHeap());
	initDone = true;
}

void loop(void) {
	handleEvents();
	switch (activeMode) {
		case RAINBOW:
		case FIRE2012:
			animationThread.runIfNeeded();
			break;
		case STATIC_COLOR:
			break;
		case HYPERION_UDP:
			break;
	}
}
