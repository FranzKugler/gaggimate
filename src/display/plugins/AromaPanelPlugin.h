#ifndef AROMAPANELPLUGIN_H
#define AROMAPANELPLUGIN_H
#include <Arduino.h>

#include <ExtensionIOXL9555.hpp>
#include <display/core/Plugin.h>
#include <display/drivers/LilyGo-T-RGB/utilities.h>

constexpr unsigned long AROMA_UPDATE_INTERVAL = 500;

/**
 * AromaPanelPlugin – LED-Anzeige für die Aroma-Kaffeemaschine.
 *
 * Verwendet den gleichen XL9555 I2C GPIO-Expander wie die kMix-Platine
 * (gleiche I2C-Adresse XL9555_SLAVE_ADDRESS5, gleiche Pins IO0/IO1).
 * Die Erkennung erfolgt automatisch über IO4 (LOW = Aroma, HIGH/floating = kMix).
 *
 * Aktive Pins:
 *   - brew_ok  (IO0) → Brew-Temperatur stabil
 *   - steam_ok (IO1) → Steam-Temperatur stabil
 *
 * Die Aroma-Maschine hat keine Tastenbeleuchtung (brew/steam entfällt)
 * und kein 3-Wege-Magnetventil.
 *
 * Namensgebung brew_ok / steam_ok bewusst analog zu KmixPanelPlugin.
 */
class AromaPanelPlugin : public Plugin {
  public:
    void setup(Controller *controller, PluginManager *pluginManager) override;
    void loop() override;

  private:
    void updateControl();

    unsigned long lastUpdate = 0;
    bool initialized = false;
    bool blinkOn = false;
    int shadowLEDs = 0x03; // IO0 + IO1 both HIGH on startup

    Controller *controller = nullptr;
    ExtensionIOXL9555 aromapanel;

    // Pin assignments – identical to KmixPanelPlugin so the same XL9555
    // footprint can be used for both machine types
    ExtensionIOXL9555::ExtensionGPIO brew_ok  = ExtensionIOXL9555::IO0;
    ExtensionIOXL9555::ExtensionGPIO steam_ok = ExtensionIOXL9555::IO1;
};

#endif // AROMAPANELPLUGIN_H
