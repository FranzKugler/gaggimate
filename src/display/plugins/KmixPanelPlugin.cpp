#include "KmixPanelPlugin.h"
#include <display/core/Controller.h>
#include <display/core/Event.h>

static ExtensionIOXL9555 kmixpanel;

void KmixPanelPlugin::setup(Controller *controller, PluginManager *pluginManager) {
    this->controller = controller;
    
    // Initialize the XL9555 expansion chip
    if (!kmixpanel.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, XL9555_SLAVE_ADDRESS5)) {
        Serial.println(F("External GPIO expansion chip for KMix Paneldoes not exist."));
        assert(false);
    }

    kmixpanel.pinMode(brew_ok, OUTPUT);
    kmixpanel.digitalWrite(brew_ok, HIGH);
    kmixpanel.pinMode(steam_ok, OUTPUT);
    kmixpanel.digitalWrite(steam_ok, HIGH);    
    kmixpanel.pinMode(brew, OUTPUT);
    kmixpanel.digitalWrite(brew, HIGH);    
    kmixpanel.pinMode(steam, OUTPUT);
    kmixpanel.digitalWrite(steam, HIGH);
    pluginManager->on("controller:ready", [this](Event const) { initialized = true; });

}

void KmixPanelPlugin::loop() {
    if (!initialized) {
        return;
    }
    if (lastUpdate + KMIX_UPDATE_INTERVAL < millis()) {
        lastUpdate = millis();
        updateControl();
    }
}

void KmixPanelPlugin::updateControl() {
	Settings settings = this->controller->getSettings();
    int mode = this->controller->getMode();
	switch (mode) {
		case MODE_STANDBY:
			// all temperature LEDs off
			kmixpanel.digitalWrite(brew_ok, LOW);
			kmixpanel.digitalWrite(steam_ok, LOW);
			// button LEDs are on
			kmixpanel.digitalWrite(brew, HIGH);
			kmixpanel.digitalWrite(steam, HIGH);
			Serial.println("KMix STANDBY");
			break;
		case MODE_BREW:
			// steam button is on but steam temperature is off
			kmixpanel.digitalWrite(steam_ok, LOW);
			kmixpanel.digitalWrite(steam, HIGH);
			// if brewing is taking place - blink brew button
			if (this->controller->isActive()) {
				Serial.println("KMix Brewing is ongoing");
				kmixpanel.digitalToggle(brew);
			} else {
				Serial.println("KMix BREW");
				kmixpanel.digitalWrite(brew, HIGH);
			}
			// if temperature is ok, set brew_ok LED on
			if (this->controller->getTargetTemp() - this->controller->getCurrentTemp() < 1.0) {
				// we're 3deg close to target temperature, switch brew_ok LED on permanently
				Serial.println("KMix Brewing temperature is OK");
				kmixpanel.digitalWrite(brew_ok, HIGH);
			} else {
				// we're either heating or cooling, lets blink the LED instead
				Serial.println("KMix Brewing temperature is heating");
				kmixpanel.digitalToggle(brew_ok);
			}
			break;

		case MODE_STEAM:
		case MODE_WATER:
			// brew button is on but brew temperature is off
			kmixpanel.digitalWrite(brew, HIGH);
			kmixpanel.digitalWrite(brew_ok, LOW);

			// steam button is on 
			kmixpanel.digitalWrite(steam, HIGH);
			
			// if temperature is ok, set steam_ok LED on
			if (this->controller->getTargetTemp() - this->controller->getCurrentTemp() < 1.0) {
				// we're 1deg close to target temperature, switch steam_ok LED on permanently
				Serial.println("KMix STEAM Temperature OK");
				kmixpanel.digitalWrite(steam_ok, HIGH);
			} else {
				// we're either heating or cooling, lets blink the LED instead
				Serial.println("KMix STEAM Temperature heating");
				kmixpanel.digitalToggle(steam_ok);
			}
			break;
		
		default:
			// only mode left is MODE_GRIND - or some unknown state. Same behaviour as in STANDBY
			// all temperature LEDs off
			kmixpanel.digitalWrite(brew_ok, LOW);
			kmixpanel.digitalWrite(steam_ok, LOW);
			// button LEDs are on
			kmixpanel.digitalWrite(brew, HIGH);
			kmixpanel.digitalWrite(steam, HIGH);
			break;
		}		
			
    return;
}


