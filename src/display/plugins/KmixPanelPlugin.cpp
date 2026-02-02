#include "KmixPanelPlugin.h"
#include <display/core/Controller.h>
#include <display/core/Event.h>

//static ExtensionIOXL9555 kmixpanel;

void KmixPanelPlugin::setup(Controller *controller, PluginManager *pluginManager) {
    this->controller = controller;
    
    // Initialize the XL9555 expansion chip
    if (!kmixpanel.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, XL9555_SLAVE_ADDRESS5)) {
        Serial.println(F("External GPIO expansion chip for KMix Panel does not exist."));
        //assert(false);
    } else {

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
}

void KmixPanelPlugin::loop() {
    if (!initialized) {
        return;
    }
	
    if (lastUpdate + KMIX_UPDATE_INTERVAL < millis()) {
        lastUpdate = millis();
        blinkOn = (bool)((unsigned int)(lastUpdate/KMIX_UPDATE_INTERVAL) % 2);
		// reset shadowLEDs every 10 sec to re-sync if something went wrong
		shadowLEDs = (unsigned int)(lastUpdate/KMIX_UPDATE_INTERVAL) % 20 ? shadowLEDs : -1;
		updateControl();
    }
}

void KmixPanelPlugin::updateControl() {
	Settings settings = this->controller->getSettings();
    int mode = this->controller->getMode();
	bool isActive = this->controller->isActive();
	bool temperatureOK = (this->controller->getTargetTemp() - this->controller->getCurrentTemp()) < 0.1;
	int newLEDs = 0;
	
	switch (mode) {
		case MODE_STANDBY:
			// all temperature LEDs off
			// button LEDs are on
			newLEDs |= 1 << brew;
			newLEDs |= 1 << steam;
			break;

		case MODE_BREW:
			// steam button is on but steam temperature is off
			newLEDs |= 1 << steam;
			// if brewing is taking place - blink brew button
			if (isActive) {
				// if brewing is taking place - blink brew button
				newLEDs |= blinkOn << brew;
			} else {
				// show that brewing is possible with brew button ON
				newLEDs |= 1 << brew;
			}
			// if temperature is ok, set brew_ok LED on
			if (temperatureOK) {
				// we're 0.1deg close to target temperature, switch brew_ok LED on permanently
				newLEDs |= 1 << brew_ok;
			} else {
				// we're heating, lets blink the LED instead
				newLEDs |= blinkOn << brew_ok;
			}
			break;

		case MODE_STEAM:
		case MODE_WATER:
			// brew button is on but brew temperature is off
			newLEDs |= 1 << brew;
			
			// steam button is on 
			newLEDs |= 1 << steam;

			// if temperature is ok, set steam_ok LED on
			if (temperatureOK) {
				// we're 1deg close to target temperature, switch steam_ok LED on permanently
				newLEDs |= 1 << steam_ok;
			} else {
				// we're either heating or cooling, lets blink the LED instead
				newLEDs |= blinkOn << steam_ok;
			}
			break;
		
		default:
			// only mode left is MODE_GRIND - or some unknown state. Same behaviour as in STANDBY
			// all temperature LEDs off
			newLEDs |= 1 << brew;
			newLEDs |= 1 << steam;
			break;
		}
	
		if (shadowLEDs != newLEDs)
		{ 
			// cycle through our 4 possible LEDs
			for (int i=0; i<4; i++) 
			{
				int mask = 1 << i;
				if (shadowLEDs == -1) {
					// shadowLEDs requests for a complete re-write 
					kmixpanel.digitalWrite(i, (newLEDs & mask) ? 1 : 0);
				} else {
					if ((shadowLEDs & mask) != (newLEDs & mask)) 
					{
						kmixpanel.digitalWrite(i, (newLEDs & mask) ? 1 : 0);

					}
				}	
			}
			// store what was set here in our shadowLEDs register
			shadowLEDs = newLEDs;	
		}
    return;
}


