#include "Config.h"

ConfigStruct Config::_cfgStruct;
boolean Config::_cfgLoaded = false;
boolean Config::_resetEEPROM = false;

void Config::resetEEPROM() {
	_resetEEPROM = true;
	initConfig();
}

void Config::initConfig() {
	if (loadEEPROMConfig()) return;
	initResetEEPROM();
}

bool Config::loadEEPROMConfig() {
	if (_cfgLoaded) return true;
	if (_resetEEPROM) return false;
	EEPROM.begin(sizeof(ConfigStruct));
	uint8_t version = EEPROM.read(CONFIG_START_ADDRESS);
	if (version == CONFIG_ACTIVE_VERSION) {
		EEPROM.get(CONFIG_START_ADDRESS, _cfgStruct);
		EEPROM.end();
		_cfgLoaded = true;
		Log.info("Configuration load from EEPROM at 0x%x with v%i", CONFIG_START_ADDRESS, version);
		return true;
	}
	EEPROM.end();
	Log.info("CFG=%s", "Configuration not loaded from EEPROM");
	return false;
}

void Config::saveEEPROMConfig() {
	EEPROM.begin(sizeof(ConfigStruct));
	EEPROM.put(CONFIG_START_ADDRESS, _cfgStruct);
	_cfgStruct.version = CONFIG_ACTIVE_VERSION;
	Log.info("Configuration saved at 0x%x with v%i", CONFIG_START_ADDRESS, _cfgStruct.version);
	//EEPROM.commit(); (done with end())
	EEPROM.end();
}

void Config::initResetEEPROM() {
	if (_cfgLoaded && !_resetEEPROM) return;

	Log.info("CFG=%s", "Init or Reset EEPROM");
	EEPROM.begin(sizeof(ConfigStruct));
	// Version
	_cfgStruct.version = CONFIG_ACTIVE_VERSION;

	// Wifi
	strlcpy(_cfgStruct.wifi.ssid, CONFIG_WIFI_SSID, sizeof(_cfgStruct.wifi.ssid));
	strlcpy(_cfgStruct.wifi.password, CONFIG_WIFI_PASSWORD, sizeof(_cfgStruct.wifi.password));
	strlcpy(_cfgStruct.wifi.hostname, CONFIG_WIFI_HOSTNAME, sizeof(_cfgStruct.wifi.hostname));

	#ifdef CONFIG_WIFI_STATIC_IP
	_cfgStruct.wifi.isStatic = true;
	_cfgStruct.wifi.ip = ip2cfg(CONFIG_WIFI_IP);
	_cfgStruct.wifi.subnet = ip2cfg(CONFIG_WIFI_SUBNET);
	_cfgStruct.wifi.dns = ip2cfg(CONFIG_WIFI_DNS);
	#else
	_cfgStruct.wifi.isStatic = false;

	_cfgStruct.wifi.ip.a = 0;
	_cfgStruct.wifi.ip.b = 0;
	_cfgStruct.wifi.ip.c = 0;
	_cfgStruct.wifi.ip.d = 0;

	_cfgStruct.wifi.subnet.a = 0;
	_cfgStruct.wifi.subnet.b = 0;
	_cfgStruct.wifi.subnet.c = 0;
	_cfgStruct.wifi.subnet.d = 0;

	_cfgStruct.wifi.dns.a = 0;
	_cfgStruct.wifi.dns.b = 0;
	_cfgStruct.wifi.dns.c = 0;
	_cfgStruct.wifi.dns.d = 0;
	#endif

	// LED
	_cfgStruct.led.idleMode = CONFIG_LED_STANDARD_MODE;
	_cfgStruct.led.timeoutMs = CONFIG_LED_STANDARD_MODE_TIMEOUT_MS;
	_cfgStruct.led.autoswitch = CONFIG_LED_HYPERION_AUTOSWITCH;
	_cfgStruct.led.ledType = CONFIG_LED_CHIPSET;
	_cfgStruct.led.nbLed = CONFIG_LED_COUNT;
	//_cfgStruct.led.dataPin = CONFIG_LED_DATAPIN;
	//_cfgStruct.led.clkPin = CONFIG_LED_CLOCKPIN;

	// Ports
	_cfgStruct.ports.jsonServer = CONFIG_PORT_JSON_SERVER;
	_cfgStruct.ports.udpLed = CONFIG_PORT_UDP_LED;

	saveEEPROMConfig();
	_cfgLoaded = true;
	_resetEEPROM = false;
	Log.info("CFG=%s", "Init or Reset EEPROM Done");
}

ConfigStruct *Config::getConfig(void) {
	initConfig();
	return &_cfgStruct;
}

void Config::logConfig() {
	if (!_cfgLoaded) {
		Log.debug("Config not loaded. Load before log!");
		return;
	}

	if (_resetEEPROM) {
		Log.debug("Config must be reset.");
		return;
	}

	Log.debug("CFG Show Config");

	Log.debug("+WIFI+");
	Log.debug("  ssid=%s", _cfgStruct.wifi.ssid);
	Log.debug("  password=%s", _cfgStruct.wifi.password);
	Log.debug("  ip=%i.%i.%i.%i", _cfgStruct.wifi.ip.a, _cfgStruct.wifi.ip.b, _cfgStruct.wifi.ip.c,
			  _cfgStruct.wifi.ip.d);
	Log.debug("  subnet=%i.%i.%i.%i", _cfgStruct.wifi.subnet.a, _cfgStruct.wifi.subnet.b, _cfgStruct.wifi.subnet.c,
			  _cfgStruct.wifi.subnet.d);
	Log.debug("  dns=%i.%i.%i.%i", _cfgStruct.wifi.dns.a, _cfgStruct.wifi.dns.b, _cfgStruct.wifi.dns.c,
			  _cfgStruct.wifi.dns.d);
	Log.debug("  hostname=%s", _cfgStruct.wifi.hostname);

	Log.debug("+LED+");
	Log.debug("  idleMode=%i", _cfgStruct.led.idleMode);
	Log.debug("  timeoutMs=%i", _cfgStruct.led.timeoutMs);
	Log.debug("  autoswitch=%i", _cfgStruct.led.autoswitch);
	Log.debug("  ledType=%s", LedTypeName[_cfgStruct.led.ledType].c_str());
	Log.debug("  nbLed=%i", _cfgStruct.led.nbLed);
	//Log.debug("  dataPin=%i", _cfgStruct.led.dataPin);
	//Log.debug("  clockPin=%i", _cfgStruct.led.clkPin);

	Log.debug("+PORTS+");
	Log.debug("  jsonServer=%i", _cfgStruct.ports.jsonServer);
	Log.debug("  udpLed=%i", _cfgStruct.ports.udpLed);

}

byte *Config::cfg2ip(ConfigIP ipStruct) {
	Log.verbose("CFG=cfg2ip: %i.%i.%i.%i", ipStruct.a, ipStruct.b, ipStruct.c, ipStruct.d);
	byte *ipByte = new byte[4];
	ipByte[0] = ipStruct.a;
	ipByte[1] = ipStruct.b;
	ipByte[2] = ipStruct.c;
	ipByte[3] = ipStruct.d;
	//byte ipByte[] = { ipStruct.a, ipStruct.b, ipStruct.c, ipStruct.d };
	return ipByte;
}

ConfigIP Config::ip2cfg(const byte ip[4]) {
	Log.verbose("CFG=ip2cfg: %i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
	ConfigIP cfgIp;
	cfgIp.a = ip[0];
	cfgIp.b = ip[1];
	cfgIp.c = ip[2];
	cfgIp.d = ip[3];
	return cfgIp;
}