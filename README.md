<div align="center">

# HYPER S3 ROCKET

### A 20 × 33.5 mm ESP32-S3 powerhouse with fail-safe dual-channel battery protection, solar and USB-C charging, and motion-aware data logging

*by [STEMFORGE™](https://stemforge.com.au)*

</div>

---

The **HYPER S3 ROCKET™** is a compact, feature-rich ESP32-S3 telemetry platform with a fail-safe, dual-channel battery management system built in. Available as a castellated SMD module or a breadboard-ready through-hole board, it brings together USB-C and solar charging, peak-power supplementation from the battery, a 2 A buck-boost regulator, a precision onboard real-time clock, and an optional 10-DoF motion sensor array with high-speed MicroSD logging.

**Battery safety you don't have to engineer yourself.** Two independent layers of protection guard every charge and discharge cycle against thermal stress, overcurrent, overvoltage and undervoltage. Battery diagnostics give your firmware what it needs for fuel-gauge estimation: VIN and battery attach/detach detection, voltage and current sensing, and fault-generated interrupts, available in both active and low-power operation so you can monitor the battery while the main cores sleep.

**Skip the patchwork of breakout boards.** Charging, protection, regulation, timekeeping and sensing live on one board. It began in rocketry and high-G flight logging, and this intelligent BMS node now serves medical, automotive, energy, telecom, marine and rail development just as well, and of course rocket science and aerospace.

> 📄 Full engineering detail: [Hardware Architecture & Specifications](my-crowd-supply-project/HYPER-S3-ROCKET-Hardware-Architecture-Specifications.md)

---

<p align="center">
  <img src="my-crowd-supply-project/getting-started/images/Hyper_S3_Rocket_Functional_Pinout_Diagram.avif" alt="Hyper S3 Rocket Functional Pinout Diagram" width="600">
</p>


## At a Glance

| | |
| --- | --- |
| **Form factor** | 20 mm × 33.5 mm castellated SMT module, 6-layer PCB |
| **Processor** | Espressif ESP32-S3-MINI-1-N4R2, dual-core Xtensa LX7 @ 240 MHz |
| **Memory** | 4 MB flash, 2 MB PSRAM |
| **Power train** | TI TPS631000 buck-boost, 3.3 V output, up to 2 A continuous (accepts up to 3 A input current) |
| **Charger** | TI BQ25188 power-path charger, USB-C and 5 V solar input, I²C configurable |
| **Battery** | Single-cell (1S) Li-Ion, Li-Poly or LiFePO4 |
| **Battery protection** | TI eFuse + TI supervisor + TI comparator, independent of firmware |
| **Timekeeping** | SiTime SiT1532 nano-power MEMS oscillator for RTC |
| **GPIO** | 21 external (16 through-hole, 5 SMD); 14 multi-role GPIOs (GPIO 1–14) on the RTC domain |
| **I²C** | Two independent buses: internal sensors and external expansion |
| **Storage** | 4-bit MicroSD (DATA and LOGGER variants) |
| **Sensors** | 10-DoF array: IMU, magnetometer, barometer (LOGGER variant) |
| **Indicators** | 4 user LEDs (3 front, 1 back) |

---

## Core Capabilities

| Capability | What you get | Why it matters |
| --- | --- | --- |
| **Onboard RTC** | SiTime nano-power MEMS oscillator, 14 RTC-domain GPIOs, ULP coprocessors, deep-sleep wake | Precise timestamps with no network, and months-long standby on a small cell |
| **Double Safety Vault** | BQ25188 charger plus a discrete eFuse, supervisor and comparator, independent of firmware | Over-discharge, overvoltage, overcurrent and over-temperature faults are caught even if your code crashes |
| **Battery telemetry** | Battery voltage and current sensed through one analog line, I²C-configurable charger, dual NTC inputs | Report state of charge, load profile and thermal history, and adapt to battery chemistry |
| **Data logging** | 4-bit MicroSD, 10-DoF sensor array, dual isolated I²C buses | High-rate, vibration-tolerant logs of motion, pressure, temperature and your own external sensors |

---

## Applications by Industry

Every industry below has the same underlying need: a battery-powered node that records *when* something happened, *what* the battery and environment were doing, and fails safe.

| Industry | Typical need | Features that apply |
| --- | --- | --- |
| **Aerospace and rocketry** | Flight and launch logging under high G and vibration | 10-DoF IMU, magnetometer and barometer; MicroSD holder that stays seated; RTC timestamps; 2 A rail for payloads |
| **Medical and wearables** | Safe, small, long-life battery devices during prototyping and evaluation | Discharge lockout and shelf-safe 5.62 µA UVLO state; independent hardware protection; low-power standby |
| **Automotive and EV** | Auxiliary pack monitoring, shock and vibration recording | Battery voltage and current analytics; dual NTC thermal monitoring; timestamped IMU logs |
| **Consumer electronics** | Compact, USB-C rechargeable products | 20 × 33 mm footprint; power-path charging; TVS and reverse-polarity protection; keyed JST connector |
| **Energy storage and solar** | Solar-powered monitoring and battery health tracking | 5 V solar harvesting (VINDPM/IINDPM); Li-Ion/Li-Poly/LiFePO4 charger; voltage and current telemetry |
| **Telecom and remote infrastructure** | Unattended sites with no reliable connectivity | Offline RTC timestamps; MicroSD logging; low-quiescent power train; hardware cutoff that survives firmware faults |
| **Rail, marine and transport** | Condition monitoring on vibrating or sealed assets | Shock and vibration logging; discharge-side thermal protection; long standby; expansion I²C bus |
| **Mining and heavy industry** | Equipment monitoring in harsh, remote locations | Rugged SMT module; independent hardware lockouts; timestamped environmental and thermal logs |

> **Not certified.** The board is not certified to any industry standard. It gives development teams a platform built on the same protection principles those industries rely on, so a product built on it can pursue its own certification. See [Regulatory Context](my-crowd-supply-project/HYPER-S3-ROCKET-Hardware-Architecture-Specifications.md#regulatory-context).

---

## Variants

| Series | Edition | What's included | Notes |
| --- | --- | --- | --- |
| **MODULE** | Core SMT Module | All core charging, power and safety circuitry plus the ESP32-S3, on a single side with castellated edges | 10k and 100k NTCs must be soldered in place. `B+` is live, with onboard PMOS/NMOS reverse-polarity protection |
| **CHARGE** | Breadboard Ready | Adds pre-soldered male headers and a JST PH-style battery connector | NTC chip thermistors supplied; remove before fitting custom thin-film NTCs. `B+` disabled |
| **DATA** | Data Logger Edition | Adds the 4-bit MicroSD slot | Localized data acquisition |
| **LOGGER** | Full Telemetry | MicroSD slot plus the 10-DoF sensor array | Flagship configuration |

---
<p align="center">
  <img src="my-crowd-supply-project/getting-started/images/Hyper_S3_Rocket_Board_Anatomy_Diagram.avif" alt="Hyper S3 Rocket Board Anatomy and Component Guide" width="600">
</p>


## Power & Battery Protection

### Power train (TI TPS631000)

| Input voltage | Mode | Continuous output |
| --- | --- | --- |
| V<sub>IN</sub> ≥ 3.0 V | Buck / stabilizer | **2.0 A** |
| V<sub>IN</sub> ≥ 2.7 V | Boost | **1.5 A** |

Output ripple stays below 20 mV through a seamless 3-cycle buck-boost crossover. True load disconnect in shutdown, and 8 µA quiescent current.

### Charger and power path (TI BQ25188)

- USB-C input up to **1.1 A**, passed to the system or split between load and charging
- Battery-only mode up to **3 A** to the system, plus supplement mode when the input can't keep up
- Nominal 5 V solar input with VINDPM/IINDPM maximum-power harvesting
- I²C-configurable for Li-Ion, Li-Poly and LiFePO4; a battery is optional

### The "Double Safety Vault"

A discrete **TI TPS259461A eFuse**, **TLV809EA26 supervisor** and **TLV4021 comparator** run in parallel with the charger as a hard-wired cutoff that does not depend on ESP32-S3 firmware.

| Fault | Protects against |
| --- | --- |
| **UVLO** | Permanent damage from over-discharging a cell (enforced on charge *and* discharge) |
| **OVLO** | Faulty, fluctuating or noisy charge inputs |
| **OCLO** | Dead shorts and motor stalls |

- **Dual-zone thermal control:** the BQ25188 monitors a 10k NTC while charging; the eFuse, supervisor and comparator monitor an independent 100k NTC during discharge and shut the rail down on over-temperature.
- **Micropower shelf safety:** at the **2.63 V** UVLO trip point the protection circuit draws only **5.62 µA**, so an exhausted cell isn't drained further in storage. Applying USB or 5 V solar releases the eFuse and charging resumes.
- **Input protection:** TVS on USB, `VIN` and `VOUT`; Schottky reverse-polarity protection on the dual `VIN`/`Vusb` stage.
- **Battery polarity:** CHARGE, DATA and LOGGER use the keyed JST PH connector (always confirm polarity); the `B+` pin is disabled so two batteries can't be connected at once. MODULE has a PMOS + NMOS reverse-polarity stage on `B+` (absolute maximum +20 V / −12 V).

### Analog power sensing

A Diodes Inc. PMOS + NMOS array switches **GPIO 16** between two measurements, selected by **GPIO 0**:

- **GPIO 0 high:** battery voltage, read through the NMOS and a Schottky-protected resistor divider
- **GPIO 0 low:** battery current, read through the PMOS from the eFuse's current-sense output

---

## Dual Independent I²C Buses

| Bus | Pins | Purpose |
| --- | --- | --- |
| **Internal sensor bus** | GPIO 17 (SDA), GPIO 18 (SCL) | Onboard 10-DoF array and internal subsystems, with onboard pull-ups |
| **External expansion bus** | GPIO 1 (SDA), GPIO 2 (SCL) | Third-party sensors with dedicated pull-ups. Matches the ESP32-S3 ULP hardware I²C, so the ULP can read expansion sensors while the main cores sleep |

---

## Pinout

### Castellated pin map (20 pins)

16 GPIOs plus `B+`, `VIN`, `GND` and `VOUT`, numbered from the top left down the left edge, then up the right edge.

| Left pin | Left signal | Right pin | Right signal |
| :---: | --- | :---: | --- |
| **1** | GPIO 1 (external I²C SDA) | **20** | GPIO 9 |
| **2** | GPIO 2 (external I²C SCL) | **19** | GPIO 10 |
| **3** | GPIO 3 | **18** | GPIO 11 |
| **4** | GPIO 4 | **17** | GPIO 12 |
| **5** | GPIO 5 | **16** | GPIO 13 |
| **6** | GPIO 6 | **15** | GPIO 14 |
| **7** | GPIO 7 | **14** | GPIO 43 (Tx) |
| **8** | GPIO 8 | **13** | GPIO 44 (Rx) |
| **9** | `B+` | **12** | `VOUT` |
| **10** | `VIN` | **11** | `GND` |

GPIO 1–14 sit on the RTC domain, giving up to **12 touch channels**, **12 ADC channels** and deep-sleep wake. GPIO 43/44 (UART) are outside the RTC domain. All 16 can output PWM from the main cores (not the ULP). GPIO 1 and 2 give up touch and ADC because they carry the external I²C pull-ups; GPIO 13 and 14 have no pull-ups and are free for any use.

<p align="center">
  <img src="my-crowd-supply-project/getting-started/images/Hyper_S3_Rocket_Pinout_GPIO-Allocation.avif" alt="Hyper S3 Rocket Pinout GPIO Allocation" width="600">
</p>

### Internal and system signals

| Signal | Pin | Notes |
| --- | --- | --- |
| Button / sense select | GPIO 0 | BOOT switch; also selects the GPIO 16 measurement (high = voltage via NMOS, low = current via PMOS from the eFuse) |
| 32.768 kHz RTC clock | GPIO 15 | Hardwired to the onboard SiTime oscillator |
| Battery voltage / current | GPIO 16 | Analog, via the switched divider and eFuse sense |
| USB D− / D+ | GPIO 19 / 20 | Native USB & JTAG, length-matched, 90 Ω |
| Fault interrupt + front red LED | GPIO 21 | Hardwired to charger and IMU (suggested: fault indicator) |
| Front green LED | GPIO 45 | Suggested: charge indicator with brightness control |
| Rear red LED | GPIO 46 | Suggested: SD card activity |
| Front blue LED | GPIO 48 | User defined |
| Not available | GPIO 22–32 | Consumed by module flash and PSRAM |

### MicroSD, 4-bit SDMMC (DATA and LOGGER)

GPIO 33–38, hardwired to the push-push holder, with pull-ups on all but GPIO 36 (CLK).

### SMD pads

GPIO 39–42 and GPIO 47 are exposed as pads for additional GPIO. They may interfere with the push-push holder mechanism if used carelessly. GPIO 39–42 can be used as JTAG with eFuse burning (permanent).

### Power and ground pins

| Pin | Rating / notes |
| --- | --- |
| `VOUT` (pin 12) | 3.3 V, up to 2 A, same on every variant |
| `VIN` (pin 10) | 5 V nominal (most efficient and thermally friendly); 18.5 V capable at low current; 23 V tolerant |
| `B+` (pin 9) | Live on MODULE only; eFuse limited to 4.6 V |
| `GND` (pin 11) | Unbroken ground planes on layers 2 and 5 provide return paths and EMI shielding |

### Thermal inputs (NTC)

Two 2.0 mm through-hole ports for thin-film, flexible beta 3435 NTCs (not supplied): **10k** for charge regulation and **100k** for charge and discharge limit/lockout.

### Onboard sensors (LOGGER variant, internal I²C)

| Part | Function | I²C address |
| --- | --- | --- |
| STMicroelectronics LSM6DSV32X | 6-DoF accelerometer and gyroscope | `0x6B` |
| Memsic MMC5603NJ | 3-DoF magnetometer | `0x30` |
| Goertek SPL07-003 / 006 | Barometer with temperature (the 006 has a waterproof gel-filled cavity), routed via the LSM6DSV32X sensor hub | `0x76` |
| TI BQ25188 (all variants) | Charger / power path | `0x6A` |

---

## Sample Code: Initializing the Hardware Bus

```cpp
#include <Wire.h>

#define INTERNAL_I2C_SDA  17
#define INTERNAL_I2C_SCL  18
#define SENSE_SELECT_PIN  0    // HIGH = battery voltage, LOW = battery current
#define BATTERY_SENSE_PIN 16

#define IMU_ADDR  0x6B  // LSM6DSV32X (LOGGER only)
#define MAG_ADDR  0x30  // MMC5603NJ  (LOGGER only)
#define PMIC_ADDR 0x6A  // BQ25188

// Calibrate for your board's divider ratio. GPIO 16 is ADC2, so analog
// reads are unavailable while Wi-Fi is active.
#define DIVIDER_RATIO 2.0

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }  // wait for native USB

  Serial.println("\n=======================================================");
  Serial.println("STEMFORGE HYPER S3 ROCKET: INITIALIZATION");
  Serial.println("=======================================================");

  if (!Wire.begin(INTERNAL_I2C_SDA, INTERNAL_I2C_SCL)) {
    Serial.println("CRITICAL: internal I2C bus failed!");
    while (1);
  }
  Serial.println("Internal I2C bus online (SDA 17 / SCL 18).");

  checkDevice("LSM6DSV32X IMU", IMU_ADDR);
  checkDevice("MMC5603NJ Magnetometer", MAG_ADDR);
  checkDevice("BQ25188 Battery Charger", PMIC_ADDR);

  pinMode(SENSE_SELECT_PIN, OUTPUT);
  digitalWrite(SENSE_SELECT_PIN, HIGH);  // select battery voltage
  Serial.println("\nSystem ready.");
  Serial.println("=======================================================\n");
}

void loop() {
  float volts = analogReadMilliVolts(BATTERY_SENSE_PIN) / 1000.0 * DIVIDER_RATIO;
  Serial.print("Battery: ");
  Serial.print(volts);
  Serial.println(" V");
  delay(1000);
}

void checkDevice(const char* name, uint8_t addr) {
  Wire.beginTransmission(addr);
  Serial.print(Wire.endTransmission() == 0 ? "  [ONLINE]  " : "  [OFFLINE] ");
  Serial.println(name);
}
```

> GPIO 0 is also the BOOT button, so don't hold it low during reset unless you intend to enter the bootloader.

---

## Repository Guide

| Folder | Contents |
| --- | --- |
| [`Firmware/Examples`](Firmware/Examples) | Example firmware and sketches |
| [`my-crowd-supply-project`](my-crowd-supply-project) | Crowd Supply campaign material, including the [Hardware Architecture & Specifications](my-crowd-supply-project/HYPER-S3-ROCKET-Hardware-Architecture-Specifications.md) |
| [`manufacturing`](manufacturing) | Footprint for the **MODULE** (castellated SMT) edition, so you can surface-mount it on your own baseboard like any other SoC module |
| [`fault-finding`](fault-finding) | Troubleshooting and fault-finding guides |
| [`support`](support) | Support resources |
| [`licensing`](licensing) | Licensing details |
| [`LICENSE`](LICENSE) | Apache-2.0 license text for software and firmware |

---

## ⚖️ License & Copyright

- **Software / Firmware Examples:** Licensed under the **Apache License, Version 2.0**. You may not use these files except in compliance with the License. You may obtain a copy at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations.
- **Physical Hardware Design:** The circuit layouts, component placement, schematics, layer stackups and PCB architecture of the **HYPER S3 ROCKET** are proprietary intellectual property. Copyright © 2026 by **STEMFORGE**. All rights reserved. Commercial replication, hardware cloning or unauthorized reverse engineering of the physical board layout is strictly prohibited. **Exception:** the MODULE footprint published in [`manufacturing`](manufacturing) may be used in your own PCB designs to mount HYPER S3 ROCKET modules. It does not grant any right to copy or manufacture the module itself.
- **Trademarks:** **STEMFORGE™**, **HYPER S3 ROCKET™** and associated logos are proprietary trademarks of **STEMFORGE**. Unauthorized commercial use of these marks to market unauthorized derivative hardware is strictly prohibited.
