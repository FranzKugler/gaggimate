
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define PING_INTERVAL 1000
#define PROGRESS_INTERVAL 100
#define HOT_WATER_SAFETY_DURATION_MS 120000
#define STEAM_SAFETY_DURATION_MS 600000
#define BREW_MIN_DURATION_MS 5000
#define BREW_MAX_DURATION_MS 300000
#define BREW_SAFETY_DURATION_MS BREW_MAX_DURATION_MS
#define BREW_MIN_VOLUMETRIC 5.0
#define BREW_MAX_VOLUMETRIC 250.0
#define DEFAULT_STANDBY_TIMEOUT_MS 900000
#define MIN_TEMP 0
#define MAX_TEMP 160
#define DEFAULT_TEMPERATURE_OFFSET 0
#define DEFAULT_PRESSURE_SCALING 16.0f
#define DEFAULT_PID "58.397,1.027,249.055,0.0"
#define DEFAULT_PUMP_MODEL_COEFFS "10.205,5.521"
#define DEFAULT_MDNS_NAME "gaggimate"
#define DEFAULT_OTA_CHANNEL "latest"
#define DEFAULT_TIMEZONE "Europe/Rome"
#define DEFAULT_HOME_ASSISTANT_TOPIC "homeassistant"
#define DEFAULT_STEAM_PUMP_PERCENTAGE 4.f
#define DEFAULT_STEAM_PUMP_CUTOFF 2.f
#define WIFI_CONNECT_ATTEMPTS 20

#define MODE_STANDBY 0
#define MODE_BREW 1
#define MODE_STEAM 2
#define MODE_WATER 3
#define MODE_GRIND 4

// Alt Relay / SSR2 Function constants
#define ALT_RELAY_NONE 0
#define ALT_RELAY_GRIND 1
#define ALT_RELAY_STEAM_BOILER 2

#define WIFI_CONNECT_TIMEOUT_MS 30000
#define DEFAULT_WIFI_AP_TIMEOUT_MS 600000

// Machine type constants
// MACHINE_GAGGIA: Standard Gaggia Classic / original GaggiMate target machine
// MACHINE_KMIX:  Kenwood kMix - uses XL9555 I2C GPIO expander for LED panel, no 3-way solenoid valve
// MACHINE_AROMA: Aroma machine - uses same XL9555 chip at same address; only brew_ok+steam_ok LEDs
#define MACHINE_GAGGIA 0
#define MACHINE_KMIX   1
#define MACHINE_AROMA  2

// IO4 on the XL9555 expander is used as the machine-type identification pin.
// kMix  board: IO4 open/floating → reads HIGH (XL9555 internal pull-up)
// Aroma board: IO4 pulled to GND on PCB   → reads LOW
#define XL9555_MACHINE_DETECT_IO 4

#endif // CONSTANTS_H
