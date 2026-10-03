# HYPER S3 ROCKET — Hardware Architecture & Specifications

The **HYPER S3 ROCKET** by Stem Forge is a rugged telemetry and power-management node built for high-G environments, aerospace logging and long-term remote deployment. A dual-core 240 MHz ESP32-S3, a 2 A buck-boost power train, a hardware-enforced battery protection chain and an optional 10-DoF sensor array all fit on a **20 mm × 33 mm** castellated board.

The hard analog and power problems are solved on the PCB itself, so you don't need a stack of fragile breakout boards.

![HYPER S3 ROCKET hero shot](https://placeholders.dev)
*Figure 1: The HYPER S3 ROCKET Full Telemetry variant, showing castellated SMT edges and dense component placement.*

---

## At a Glance

| | |
| --- | --- |
| **Size** | 20 mm × 33 mm, castellated SMT / breadboard-friendly |
| **Processor** | Espressif ESP32-S3-MINI-1-N4R2 (dual-core Xtensa LX7 @ 240 MHz, 4 MB flash, 2 MB PSRAM) |
| **Timing** | SiTime SiT1532 nano-power MEMS oscillator for RTC timestamping |
| **Power train** | TI TPS631000 buck-boost, 3.3 V output, up to 2 A |
| **Charging** | TI BQ25188 power-path LiPo charger, USB-C input up to 1.1 A, solar-capable |
| **Battery protection** | TI eFuse + supervisor + comparator, independent of firmware |
| **GPIO** | 16 through-hole multi-role GPIOs on the RTC power domain, plus 5 SMD GPIOs |
| **Buses** | Two isolated I²C buses (internal sensors, external expansion), 4-bit SD bus |
| **Core Power Telemetry** | I²C address 0x6a for Vin and Battery attached/detached, Charging Status, Charging Faults, Charging Voltage Parameters), GPIO15 ADC for Battery Voltage and Current sensing (GPIO0 switched) |
| **Sensors (LOGGER)** | STM LSM6DSV32X 6-DoF IMU, Memsic MMC5603NJ 3-axis magnetometer, Goertek SPL07 barometer/temperature |
| **Storage (DATA, LOGGER)** | MicroSD, 4-bit SDMMC, push-push holder |
| **Switches** | User Button on GPIO0 (Labelled Boot) |
| **Indicators** | 4 programmable LEDs (3 front, 1 back) |

---

## Hardware Variants

The board comes in four series that share the same core design.

| Series | What you get | Best for |
| --- | --- | --- |
| **MODULE** | Core power, charging, safety and ESP32-S3 on a single side with castellated edges. No JST connector. The external `B+` pin is active for an external battery (no reverse-polarity protection on that pin). 10k and 100k NTC thermistors must be soldered in place. | Embedding in your own PCB |
| **CHARGE** | Adds pre-soldered pin headers and a JST-PH battery connector. Supplied with NTC bypass resistors (remove before fitting a real NTC). `B+` pin disabled. | Benchtop prototyping, education |
| **DATA** | CHARGE plus the MicroSD slot. | Data logging |
| **LOGGER** | DATA plus the 10-DoF sensor array on the bottom layer. The flagship. | Rocketry and kinetic telemetry |

**Top layer (all series):** ESP32-S3-MINI-1-N4R2, TI BQ25188, TI TPS631000, SiTime SiT1532, TI TLV809EA26DPWR supervisor, TI TLV4021R1YKAR comparator, Diodes DMC31D5UDA-7B MOSFET array.
**Bottom layer (LOGGER only):** LSM6DSV32X IMU, MMC5603NJ magnetometer, SPL07-003/006 barometer.

---

## Compute & Timing

- **ESP32-S3-MINI-1-N4R2**: a pre-certified module on a 6-layer board, which speeds up EMC testing. Near-field pre-testing has passed.
- Every pin is planned. 16 through-hole and 5 SMD GPIOs are exposed; the rest serve the onboard sensors and SD storage.
- Use the main LX7 cores, or the **ULP RISC-V coprocessor / ULP FSM** in the RTC domain for ultra-low-power sensing.
- The **SiT1532** MEMS oscillator gives precise RTC timestamps, even with no Wi-Fi or network connection.

---

## Power Train: TI TPS631000 Buck-Boost

![Double Safety Vault flow](https://placeholders.dev)
*Figure 2: Hardware protection topology that works independently of the MCU firmware.*

- **Up to 2 A at 3.3 V** when the input is ≥ 3.0 V, and **1.5 A** deep into boost mode with the input down to 2.7 V.
- **Seamless mode transitions.** Constant-frequency peak-current control moves between buck, boost and a 3-cycle buck-boost window, keeping output ripple under **20 mV**. That keeps analog noise out of your telemetry data.
- **True load disconnect** with about **8 µA** quiescent current in standby.
- **Dynamic analog power sensing.** A PMOS + NMOS switching circuit lets a single analog line alternate between battery voltage sensing and current sensing, without cross-talk.

---

## Charging & Power Sources

The **TI BQ25188** power-path charger gives you several ways to run the board:

- **USB-C** input up to 1.1 A, shared between system load and battery charging.
- **Battery only**: up to 3 A direct to the system through the internal FET.
- **Supplemental mode**: the battery tops up the system rail when the input can't keep up.
- **Solar**: connect a nominal 5 V panel to `VIN`. VINDPM and IINDPM input regulation harvests as much power as the panel can give with thermal limiting. NOTE: this linear regulator cannot handle high voltages and current at the same time. DO NOT connect Vin to a battery or supply over 6 V.
- **I²C configurable** charge voltage for Li-Ion, Li-Poly and LiFePO₄ cells.

Batteries are optional. The board runs from USB or solar alone.

---

## Hardware-Enforced "Double Safety Vault"

Protection runs **independently of the ESP32-S3 firmware**. If your code freezes, the battery and power rails still protect themselves.

### Two co-equal layers

1. **TI BQ25188** handles charging safety, software-configurable voltage limits, battery overcurrent, overvoltage (it reduces or stops charging) and charge-temperature regulation through an NTC input.
2. **TI TPS259461ARPWR eFuse** sits on the battery rail as a hard-wired cutoff in both directions. If a firmware crash or bad register setting leaves the charger misconfigured, the eFuse still isolates the battery on short circuits and overcurrent.

### Protection against

| Fault | Protects against |
| --- | --- |
| **UVLO**: under-voltage lockout | Permanent LiPo damage from over-discharge |
| **OVLO**: over-voltage lockout | Faulty, fluctuating or noisy charging inputs |
| **OCLO**: over-current lockout | Dead shorts and motor stalls |

Many charger ICs only protect during charging and ignore the discharge side. Here, discharge-side under-voltage lockout is handled by hardware, using the eFuse together with a TI supervisor.

### Micropower preservation

When the hardware UVLO trips at **2.63 V**, the eFuse's parasitic leakage (**4.4 µA**) and the input divider (**1.22 µA**) total only **5.62 µA**. The circuit won't drain an exhausted cell during long shelf storage. Once USB or a 5 V solar panel is present, the BQ25188 powers the SYS rail and releases the eFuse so charging can resume.

### Dual-zone thermal control

- **While charging:** the BQ25188 uses its own NTC input.
- **While discharging:** the charger can't see the battery, so the eFuse, TI supervisor and TI **TLV4021R1YKAR** comparator monitor a second, independent NTC and shut the main rail if the cell overheats.

Thermistors are not supplied. Use the type that suits your cell (10k and 100k NTC support is built in).

### Boundary protection

- **ESD / TVS** on `Vusb`, `D+`, `D-`, `VIN` and `B+`, against positive and negative transients from cable insertion or touching exposed pins.
- **Reverse-polarity protection** on the USB and `VIN` inputs, using two onboard power Schottky diodes. They cost a small voltage drop in exchange for solid protection.
- **Battery connector:** diodes and typical eFuses can't allow two-way battery current, so the JST-PH connector is keyed. Always check polarity before connecting. On CHARGE, DATA and LOGGER the external `B+` pin is disabled; on MODULE it is active and unprotected.
- **Schottky clamp** on the analog divider, keeping voltage spikes away from the ESP32-S3.

### Battery analytics

A Diodes Inc. MOSFET array switches between a Schottky-protected resistor divider (battery voltage) and the eFuse current-monitor output (battery current), so firmware can report both.

<summary><strong>Why two layers? Industry battery-safety context</strong></summary>

Many industries require more than one independent layer of battery protection. This table is background on why the board is designed this way. **NOTE:** It is **not** a statement that the HYPER S3 ROCKET is certified to any of these standards. And though at STEM FORGE we aim to get you close, true understanding and determination is up to the end-user.

| Industry | Primary standards | Core danger addressed | Typical secondary safeguard |
| --- | --- | --- | --- |
| Aerospace | RTCA DO-311A / FAA | Flight-control loss, uncontainable fire | Burst discs, independent cell-isolation switches |
| Medical | IEC 62133 / FDA guidance | Patient injury, life-support shutdown | Thermal or chemical fuses in the cell pack |
| Automotive | ISO 26262 / UN 38.3 / UL 2580 | Crash shock, thermal runaway | Pyro-fuses triggered by independent crash sensors |
| Consumer electronics | UL 2054 / IEEE 1725 | Burns, property fires | Secondary overvoltage IC or PTC device |
| Grid storage (BESS) | UL 9540 / NFPA 855 | Large-scale explosion, toxic gas | Shunt-trips, gas/fire suppression interlocks |
| Hazardous areas | IEC 60079-11 (Intrinsic Safety) / ATEX / IECEx | Igniting gas or dust | Redundant current-limiting resistors, encapsulated fuses |
| Marine | DNV-CG-0157 / ABS | Confined-space toxic gas, subsea shorts | Hardwired overvoltage trip-shunts |
| Telecom / data | UL 1973 | Outages, data-centre fires | Hardware breakers independent of software |
| Rail | EN 50126 / EN 50155 | Tunnel fires, brake power loss | Contactors on independent analog safety loops |
| Defence | MIL-STD-810 | Ballistic puncture, extreme environments | Redundant isolators independent of firmware |


---

## I/O & Expansion

### 16 multi-role GPIOs on the RTC domain

Every exposed through-hole GPIO is on the ESP32-S3 **RTC power domain**, so any of them can wake the chip from deep sleep. Between them they cover up to **12 capacitive-touch channels** and **16 ADC channels**, plus UART, SPI and I²C.

### Two isolated I²C buses

Separate buses prevent address conflicts and bus stalls between your sensors and the onboard ones.

| Bus | Pins | Purpose |
| --- | --- | --- |
| **Internal sensor bus** | SDA GPIO 17, SCL GPIO 18 | Onboard 10-DoF sensor array and internal subsystems, with onboard pull-ups |
| **External expansion bus** | SDA GPIO 13, SCL GPIO 14 | Third-party sensors, with its own pull-ups |

### 4-bit MicroSD storage (DATA & LOGGER)

- Uses the ESP32-S3's native **4-bit SD bus** instead of slow SPI, for fast logging.
- **Push-push holder** designed to keep the card seated through high-vibration and high-G launches.

### Status LEDs

Four user-programmable LEDs (3 front, 1 back) for diagnostics and hardware checks.

---

## Pinout & Bus Architecture

![HYPER S3 ROCKET graphical pinout](https://placeholders.dev)
*Figure 3: Colour-coded functional pin map.*

| Group | Pins | Hardware feature | Notes |
| --- | --- | --- | --- |
| **External I²C SDA** | GPIO 13 | External bus, onboard pull-up | For expansion sensors; no address clashes with onboard parts |
| **External I²C SCL** | GPIO 14 | External bus, onboard pull-up | As above |
| **Internal I²C SDA** | GPIO 17 | Isolated internal bus, pull-up | Hardwired to the onboard sensors |
| **Internal I²C SCL** | GPIO 18 | Isolated internal bus, pull-up | As above |
| **System GPIO (×16)** | Exposed pins | All on RTC domain | Up to 12 touch channels, 16 ADC lines, deep-sleep wake |
| **Power management** | Internal rail | 2 A buck-boost | Same on every variant |
| **Safety analytics** | Discrete circuit | TI supervisor + comparator | Autonomous UVLO / OVLO / OCLO protection |
| **Thermal profiling** | Analog inputs | Two NTC ports | One for charge, one for discharge |

---

## Built for the Water Rocket Challenge

The HYPER S3 ROCKET was designed with water-rocket flight logging in mind: a small, light board that survives launch shock, logs fast to MicroSD, timestamps accurately offline, and protects its own battery, but maybe this should be called The Little Rocket that Could - there's no excuse - don't settle for less. 
