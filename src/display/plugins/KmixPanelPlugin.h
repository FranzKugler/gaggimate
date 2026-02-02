#ifndef KMIXPANELPLUGIN_H
#define KMIXPANELPLUGIN_H
#include <Arduino.h>

#include <ExtensionIOXL9555.hpp>
#include <display/core/Plugin.h>
#include <display/drivers/LilyGo-T-RGB/utilities.h>

constexpr unsigned long KMIX_UPDATE_INTERVAL = 500;

class KmixPanelPlugin : public Plugin {
  public:
    void setup(Controller *controller, PluginManager *pluginManager) override;
    void loop();

  private:
    void updateControl();
    
    unsigned long lastUpdate = 0;
    bool initialized = false;
    bool blinkOn = false;
    int shadowLEDs = 0x0F;

    Controller *controller = nullptr;
    ExtensionIOXL9555 kmixpanel;

    ExtensionIOXL9555::ExtensionGPIO brew_ok = ExtensionIOXL9555::IO0;
    ExtensionIOXL9555::ExtensionGPIO steam_ok = ExtensionIOXL9555::IO1;
    ExtensionIOXL9555::ExtensionGPIO brew = ExtensionIOXL9555::IO2;
    ExtensionIOXL9555::ExtensionGPIO steam = ExtensionIOXL9555::IO3;
};

#endif // KMIXPANELPLUGIN_H
