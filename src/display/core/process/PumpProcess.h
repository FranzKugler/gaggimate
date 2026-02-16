#ifndef PUMPPROCESS_H
#define PUMPPROCESS_H

#include <display/core/constants.h>
#include <display/core/process/Process.h>

class PumpProcess : public Process {
  public:
    int duration;
    unsigned long started;

    explicit PumpProcess(int duration = HOT_WATER_SAFETY_DURATION_MS) : duration(duration) { started = millis(); }

    //@@kuf 20260209 isRelayActive changed to true by default (kMix logic)
    bool isRelayActive() override { return true; };

    bool isAltRelayActive() override { return false; };

    float getPumpValue() override { return isActive() ? 100.f : 0.f; };

    void progress() override {
        // Stateless implementation
    };

    bool isActive() override {
        unsigned long now = millis();
        return now - started < duration;
    };

    bool isComplete() override { return !isActive(); };

    int getType() override { return MODE_WATER; }

    void updateVolume(double volume) override {};
};

#endif // PUMPPROCESS_H
