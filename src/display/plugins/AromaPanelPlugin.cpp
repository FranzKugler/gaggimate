#ifndef GAGGIMATE_HEADLESS
#include "AromaPanelPlugin.h"
#include <display/core/Controller.h>
#include <display/core/Event.h>

void AromaPanelPlugin::setup(Controller *controller, PluginManager *pluginManager) {
    this->controller = controller;

    if (!aromapanel.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, XL9555_SLAVE_ADDRESS5)) {
        Serial.println(F("External GPIO expansion chip for Aroma Panel does not exist."));
    } else {
        aromapanel.pinMode(brew_ok, OUTPUT);
        aromapanel.digitalWrite(brew_ok, HIGH);
        aromapanel.pinMode(steam_ok, OUTPUT);
        aromapanel.digitalWrite(steam_ok, HIGH);
        pluginManager->on("controller:ready", [this](Event const) { initialized = true; });
    }
}

void AromaPanelPlugin::loop() {
    if (!initialized) {
        return;
    }
    if (lastUpdate + AROMA_UPDATE_INTERVAL < millis()) {
        lastUpdate = millis();
        blinkOn = (bool)((unsigned int)(lastUpdate / AROMA_UPDATE_INTERVAL) % 2);
        // reset shadowLEDs every 10 sec to re-sync if something went wrong
        shadowLEDs = (unsigned int)(lastUpdate / AROMA_UPDATE_INTERVAL) % 20 ? shadowLEDs : -1;
        updateControl();
    }
}

void AromaPanelPlugin::updateControl() {
    int mode = this->controller->getMode();
    // Use the same filtered temperature-stable flag as the UI heating indicator
    // (computed from rolling average in DefaultUI::updateTempStableFlag)
    bool temperatureOK = this->controller->getUI() != nullptr && this->controller->getUI()->isTempStable();
    int newLEDs = 0;

    switch (mode) {
        case MODE_STANDBY:
            // all temperature LEDs off
            break;

        case MODE_BREW:
        case MODE_GRIND:
            // steam_ok off – not relevant in brew mode
            // brew_ok: ON when temperature stable, blink while heating up
            if (temperatureOK) {
                newLEDs |= 1 << brew_ok;
            } else {
                newLEDs |= blinkOn << brew_ok;
            }
            break;

        case MODE_STEAM:
        case MODE_WATER:
            // brew_ok off – not relevant in steam/water mode
            // steam_ok: ON when temperature stable, blink while heating/cooling
            if (temperatureOK) {
                newLEDs |= 1 << steam_ok;
            } else {
                newLEDs |= blinkOn << steam_ok;
            }
            break;

        default:
            // unknown state – all LEDs off
            break;
    }

    if (shadowLEDs != newLEDs) {
        for (int i = 0; i < 2; i++) { // only IO0 (brew_ok) and IO1 (steam_ok)
            int mask = 1 << i;
            if (shadowLEDs == -1) {
                aromapanel.digitalWrite(i, (newLEDs & mask) ? 1 : 0);
            } else {
                if ((shadowLEDs & mask) != (newLEDs & mask)) {
                    aromapanel.digitalWrite(i, (newLEDs & mask) ? 1 : 0);
                }
            }
        }
        shadowLEDs = newLEDs;
    }
}

#endif // GAGGIMATE_HEADLESS
