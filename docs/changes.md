# Changes vs. Upstream (`jniebuhr/gaggimate`)

Branch: `universal`

## Goal

One firmware binary for all supported machines:

| Machine | LED control | Solenoid valve |
|---|---|---|
| **Gaggia Classic** (default) | LedControlPlugin via controller board | Per profile phase |
| **Kenwood kMix** | KmixPanelPlugin via XL9555 I2C expander | No 3-way valve – relay open during process |
| **Aroma** | AromaPanelPlugin via XL9555 I2C expander | No valve |

Machine type is **auto-detected on every boot** by reading IO4 of the XL9555 GPIO expander:

- No XL9555 found → Gaggia Classic
- XL9555 found, IO4 = HIGH (floating) → kMix
- XL9555 found, IO4 = LOW (pulled to GND on PCB) → Aroma

---

## New Files

### `src/display/plugins/AromaPanelPlugin.h` / `.cpp`

New plugin for the Aroma machine. Uses the same XL9555 I2C GPIO expander as the kMix board (same address `XL9555_SLAVE_ADDRESS5`, same pins IO0/IO1). Only `brew_ok` and `steam_ok` LEDs – no button illumination, no valve. Shadow-register mechanism identical to `KmixPanelPlugin`.

PCB requirement: **IO4 of the XL9555 must be tied to GND** on the Aroma board (kMix leaves IO4 floating).

---

## Modified Files

### `src/display/core/constants.h`

Additions only, no upstream code changed:

- `MACHINE_GAGGIA` / `MACHINE_KMIX` / `MACHINE_AROMA` constants
- `XL9555_MACHINE_DETECT_IO 4` – pin used for hardware detection

### `src/display/core/Controller.h`

Minimal additions:

- `int machineType` private member (set at boot, not persisted to NVS)
- `getMachineType()` public getter

### `src/display/core/Controller.cpp`

Three cohesive change blocks:

1. **Includes** – added `<Wire.h>`, `<ExtensionIOXL9555.hpp>`, `AromaPanelPlugin.h`; removed `@@kuf` comments

2. **`setupPanel()`** – hardware detection appended after display init:  
   Initialise XL9555 at `XL9555_SLAVE_ADDRESS5` → read IO4 → set `machineType` accordingly.  
   No NVS involved, detects fresh from hardware on every boot.

3. **Plugin registration** – `switch(getMachineType())` replaces the previous hardcoded `KmixPanelPlugin` registration

4. **`updateControl()`** – machine-type-specific relay logic:
   - Gaggia: original upstream behaviour (valve driven per profile phase)
   - kMix: relay open during active process; BrewProcess venting pulse still honoured; `lastProcess` used for post-brew venting
   - Aroma: always `true` (no valve present)

   The previous `@@kuf`-annotated commented-out block is fully removed.

### `src/display/core/process/SteamProcess.h` / `PumpProcess.h` / `GrindProcess.h`

`isRelayActive()` reverted to upstream `false`. The kMix-specific logic now lives in `Controller::updateControl()`, not in the process classes.

### `src/display/plugins/KmixPanelPlugin.cpp`

One change: `temperatureOK` now uses `controller->getUI()->isTempStable()` instead of a raw temperature-difference comparison. This aligns LED behaviour with the UI heating indicator (same filtered rolling-average flag).

### `src/display/ui/default/DefaultUI.h`

One addition: `bool isTempStable() const` public getter – exposes the filtered `isTemperatureStable` flag (identical to the green/red heating indicator in the UI) for use by panel plugins.

---

## Unchanged vs. Upstream

`Settings.h/.cpp`, `WebUIPlugin.cpp`, all web UI files – fully compatible, no merge conflicts expected.
