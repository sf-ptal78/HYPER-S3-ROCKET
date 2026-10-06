<div align="center">

# HYPER S3 ROCKET

### Hardware Architecture & Specifications

**A 20 × 33 mm ESP32-S3 powerhouse with fail-safe dual-channel battery protection, solar and USB-C charging, and motion-aware data logging  **

*by Stem Forge*

</div>

---

The **HYPER S3 ROCKET** is a compact powerhouse: a feature-rich ESP32-S3 telemetry platform with a fail-safe Battery Management System (BMS) built in. Two independent hardware protection layers guard every charge and discharge, and keep working even if your firmware doesn't. Available as a castellated SMD module or a breadboard-ready through-hole board, it brings together USB-C and solar charging, peak-power supplementation from the battery, a 2 A buck-boost regulator, a precision onboard real-time clock, and an optional 10-DoF motion sensor array with high-speed MicroSD logging.

Battery safety you don't have to engineer yourself. Two independent layers of protection guard every charge and discharge cycle against thermal stress, overcurrent, overvoltage and undervoltage. Comprehensive battery diagnostics give your firmware what it needs for fuel-gauge estimation: VIN and B+ power-state detection, voltage and current sensing, and fault-generated interrupts. Whether you're building an ultra-low-power node or a high-performance system, the diagnostics are designed to be accessible in both active and low-power operation, so you can monitor the battery even while the main cores sleep.

Skip the patchwork of breakout boards. Loose connections and unprotected cells are where prototypes fail. The HYPER S3 ROCKET puts charging, protection, regulation, timekeeping and sensing on one board. It began in rocketry and high-G flight logging, and this intelligent BMS node now serves medical, automotive, energy, telecom, marine and rail development just as well, and, of course, rocket science.

<!-- TODO: replace placeholder image with the final hero shot -->
![HYPER S3 ROCKET hero shot](https://placeholders.dev)

*Figure 1: The HYPER S3 ROCKET Full Telemetry variant, showing castellated SMT edges and dense component placement.*

---

## At a Glance

| | |
| --- | --- |
| **Built for** | Safe, timestamped, battery-powered telemetry and data logging across aerospace, medical, automotive, energy, telecom, marine, rail and industrial projects |
| **Form factor** | 20 mm × 33 mm castellated SMT module, 6-layer PCB |
| **Processor** | Espressif ESP32-S3-MINI-1-N4R2, dual-core Xtensa LX7 @ 240 MHz |
| **Memory** | 4 MB flash, 2 MB PSRAM |
| **Power train** | TI TPS631000 buck-boost, 3.3 V output, up to 2 A continuous |
| **Charger** | TI BQ25188 power-path charger, USB-C and 5 V solar input, I²C configurable |
| **Battery** | Single-cell (1S) Li-Ion, Li-Poly or LiFePO4 |
| **Battery protection** | TI eFuse + TI supervisor + TI comparator, independent of firmware |
| **Timekeeping** | SiTime SiT1532 nano-power MEMS oscillator for RTC |
| **GPIO** | 21 external (16 through-hole, 5 SMD); 16 multi-role GPIOs on the RTC domain |
| **I²C** | Two independent buses: internal sensors and external expansion |
| **Storage** | 4-bit MicroSD (DATA and LOGGER variants) |
| **Sensors** | 10-DoF array: IMU, magnetometer, barometer (LOGGER variant) |
| **Indicators** | 4 user LEDs (3 front, 1 back) |

---

## Contents

- [Core Capabilities](#core-capabilities)
- [Applications by Industry](#applications-by-industry)
- [Compute and Timing](#compute-and-timing)
- [Power Train](#power-train)
- [Power Path and Battery Protection](#power-path-and-battery-protection)
- [Analog Power Sensing](#analog-power-sensing)
- [I/O and Expansion](#io-and-expansion)
- [Storage](#storage)
- [Sensor Array](#sensor-array)
- [Component Placement](#component-placement)
- [Pinout and Bus Mapping](#pinout-and-bus-mapping)
- [Variants](#variants)
- [Regulatory Context](#regulatory-context)

---

## Core Capabilities

Four features work together to make the board useful well beyond a single sector.

| Capability | What you get | Why it matters |
| --- | --- | --- |
| **Onboard RTC** | SiTime nano-power MEMS oscillator, 16 RTC-domain GPIOs, ULP coprocessors, deep-sleep wake | Precise timestamps with no network, and months-long standby on a small cell |
| **Double Safety Vault** | BQ25188 charger plus a discrete eFuse, supervisor and comparator, independent of firmware | Over-discharge, overvoltage, overcurrent and over-temperature faults are caught even if your code crashes |
| **Battery telemetry** | Battery voltage and current sensed through one analog line, I²C-configurable charger, dual NTC inputs | Your firmware can report state of charge, load profile and thermal history, and adapt to the battery chemistry |
| **Data logging** | 4-bit MicroSD, 10-DoF sensor array, dual non-conflicting I²C buses | High-rate, vibration-tolerant logs of motion, pressure, temperature and your own external sensors |

---

## Applications by Industry

Every industry below has the same underlying need: a battery-powered node that records *when* something happened, *what* the battery and environment were doing, and fails safe. The table shows which board features address each need.

| Industry | Typical need | Features that apply |
| --- | --- | --- |
| **Aerospace and rocketry** | Flight and launch logging under high G and vibration | 10-DoF IMU, magnetometer and barometer; 4-bit SD holder that stays seated; RTC timestamps; 2 A rail for payloads |
| **Medical and wearables** | Safe, small, long-life battery devices during prototyping and evaluation | Discharge lockout and shelf-safe 5.62 µA UVLO state; independent hardware protection; low-power standby; battery telemetry |
| **Automotive and EV** | Auxiliary pack monitoring, shock and vibration recording | Battery voltage and current analytics; dual NTC thermal monitoring (charge and discharge); timestamped IMU logs |
| **Consumer electronics** | Compact, USB-C rechargeable products | 20 × 33 mm footprint; power-path charging; TVS and reverse-polarity protection; keyed JST battery connector |
| **Energy storage and solar** | Solar-powered monitoring and battery health tracking | 5 V solar harvesting (VINDPM/IINDPM); chemistry-configurable charger (Li-Ion, Li-Poly, LiFePO4); voltage and current telemetry; deep-sleep wake |
| **Telecom and remote infrastructure** | Unattended sites with no reliable connectivity | Offline RTC timestamps; MicroSD logging; low-quiescent power train; hardware cutoff that survives firmware faults |
| **Rail, marine and transport** | Condition monitoring on vibrating or sealed assets | Shock and vibration logging; discharge-side thermal protection; long standby; expansion I²C bus for extra sensors |
| **Mining and heavy industry** | Equipment monitoring in harsh, remote locations | Rugged SMT module; independent hardware lockouts; timestamped environmental and thermal logs |

> The board is not certified to any industry standard (see [Regulatory Context](#regulatory-context)). It gives development teams a platform built on the same protection principles those industries rely on, so a product built on it can pursue its own certification.

---

## Compute and Timing

The **ESP32-S3-MINI-1-N4R2** is a pre-certified module on a 6-layer PCB, which gives a fast track to EMC testing. Near-field pre-compliance scans have passed.

- **Cores:** dual-core Xtensa LX7 at 240 MHz.
- **Ultra-low-power domain:** the RTC domain can run on either the ULP RISC-V coprocessor or the ULP finite state machine, so the board can sample sensors and wake on events while the main cores sleep.
- **Memory:** 4 MB flash and 2 MB PSRAM.
- **Timekeeping:** a SiTime SiT1532 nano-power MEMS oscillator drives the RTC for precise timestamps, even with no network connection.
- **Pin budget:** 21 GPIOs are exposed externally (16 through-hole, 5 SMD). The remaining pins are allocated internally to the sensors and SD storage, so every pin has a defined job.

---

## Power Train

The main rail is a **Texas Instruments TPS631000** 2 MHz buck-boost regulator producing a low-ripple 3.3 V output across the full battery range.

| Input voltage | Mode | Continuous output current |
| --- | --- | --- |
| V<sub>IN</sub> ≥ 3.0 V | Buck / stabilizer (cell full or charging) | **2.0 A** |
| V<sub>IN</sub> ≥ 2.7 V | Boost (cell sagging under load) | **1.5 A** |

- **Seamless mode transitions:** the constant-frequency, peak-current-mode loop moves between buck, boost and a 3-cycle buck-boost crossover, keeping output ripple below **20 mV** and noise off the telemetry lines.
- **True load disconnect:** shutdown mode physically isolates the load from the power train.
- **Low standby draw:** **8 µA** quiescent current.

---

## Power Path and Battery Protection

Power and safety are handled by two independent layers: the **BQ25188** charger and a discrete **eFuse + supervisor + comparator** network. Neither depends on ESP32-S3 firmware, so a frozen or misconfigured MCU can't leave the battery unprotected.

### Charger and power path (TI BQ25188)

- **USB-C input** up to 1.1 A, passed to the system rail or split between system load and charging.
- **Battery-only mode:** up to 3 A from the battery to the system through the internal FET.
- **Supplement mode:** adds battery current to the system rail when the input alone can't keep up.
- **Solar input:** accepts a nominal 5 V panel (recommended) and harvests the maximum available power using VINDPM and IINDPM.
- **I²C configurable:** charge voltage can be set for Li-Ion, Li-Poly or LiFePO4 cells.
- **Charge protection:** overcurrent and overvoltage handling (reduces or stops charging), plus charge-temperature regulation through an NTC input. NTC thermistors are not supplied; choose one to suit your cell.
- A battery is optional. The board runs from USB or solar alone.

### The "Double Safety Vault"

Most development boards use a basic linear charger with no automated discharge defense. The HYPER S3 ROCKET adds a second, hard-wired layer.

<!-- TODO: replace placeholder image with the final protection-topology diagram -->
![Double Safety Vault flow diagram](https://placeholders.dev)

*Figure 2: Protection topology operating independently of the MCU firmware layer.*

**Co-equal dual defense.** Charging safety and software-adjustable cutoffs live in the BQ25188. In parallel, a discrete **TI TPS259461A eFuse** acts as a non-negotiable hardware cutoff. If a firmware crash or bad register write misconfigures the charger, the eFuse still isolates the battery on a short circuit or overcurrent event.

**Faults handled in hardware:**

| Fault | Protects against |
| --- | --- |
| **UVLO** (under-voltage lockout) | Permanent damage from over-discharging a cell |
| **OVLO** (over-voltage lockout) | Faulty, fluctuating or noisy charge inputs |
| **OCLO** (over-current lockout) | Dead shorts and motor stalls |

Many charger ICs cover charging only and ignore the discharge stage. Here, battery under-voltage lockout is enforced on **both** charge and discharge.

**Dual-zone thermal control** (two NTC inputs, one per zone):

| Zone | Monitored by | Behavior |
| --- | --- | --- |
| **Charging** | BQ25188 with its own NTC | Manages thermal thresholds while the battery charges |
| **Discharging** | eFuse, TI TLV809EA26 supervisor and TI TLV4021 comparator, with an independent NTC | Shuts the main rail down if the cell overheats under load or rapid depletion (the charger is blind in this state) |

### Micropower shelf safety

When the eFuse UVLO trips at **2.63 V**, the protection circuit itself draws only **5.62 µA**: 4.4 µA of internal parasitic leakage plus 1.22 µA through the input resistor divider. An exhausted cell therefore isn't drained further during extended storage.

**Recovery:** applying USB (Vusb) or a nominal 5 V solar panel powers the BQ25188, which governs the SYS rail, releases the eFuse and lets charging begin.

### Input protection

- **ESD / TVS:** every incoming USB voltage and data line, incoming `VIN` and outgoing `VOUT` has TVS protection against positive and negative transients. Static discharge from plugging in a USB cable or touching an exposed pin can destroy a chip or cause latent failures.
- **Reverse polarity (power inputs):** two onboard power Schottky diodes handle reverse polarity and the `VIN` / `Vusb` dual-input stage. They cost a small amount of input voltage, which is a worthwhile trade for protecting the regulator, MCU and charger.
- **Reverse polarity (battery):** a battery needs two-way current flow, so diodes and typical eFuse reverse protection don't apply. Instead, the JST PH-style connector has a locking, keyed notch. Always confirm polarity before connecting.
- **B+ pin by variant:** on the **CHARGE**, **DATA** and **LOGGER** series the external `B+` pin is disabled and the battery connects through the JST connector. The **MODULE** series has no JST connector, so `B+` is live for an external battery, **without reverse polarity protection**.

---

## Analog Power Sensing

A Diodes Inc. MOSFET array (PMOS + NMOS) switches a single analog line between two measurements:

- **Battery voltage** through a Schottky-protected resistor divider. The Schottky clamps hazardous spikes away from the ESP32-S3.
- **Battery current** from the eFuse's current-sense output.

This gives firmware accurate battery-management data over one ADC pin, without cross-talk or signal degradation.

---

## I/O and Expansion

### GPIO

All 16 multi-role GPIOs are routed to the ESP32-S3 **RTC power domain**, enabling fast wake-up from deep sleep. Across these lines you can use up to:

- **12** capacitive touch channels
- **16** high-speed ADC channels
- dedicated UART, SPI and I²C buses

### Dual independent I²C buses

Most boards put internal and external sensors on one I²C bus, which risks address conflicts and bus stalls. The HYPER S3 ROCKET separates them:

| Bus | Pins | Purpose |
| --- | --- | --- |
| **Internal sensor bus** | GPIO 17 (SDA), GPIO 18 (SCL) | Onboard 10-DoF sensor array and internal subsystems, with onboard pull-ups |
| **External expansion bus** | GPIO 13 (SDA), GPIO 14 (SCL) | Third-party sensors, with its own dedicated pull-ups |

### Status LEDs

Four user-addressable LEDs (three on the front, one on the back) for hardware verification and diagnostics.

---

## Storage

*DATA and LOGGER variants*

- **4-bit SD bus:** uses the ESP32-S3's native 4-bit SD interface rather than SPI for faster telemetry logging.
- **Push-push MicroSD holder:** spring-loaded and designed to stay seated through high-vibration and high-G launches.

---

## Sensor Array

*LOGGER variant, mounted on the bottom layer*

| Sensor | Part | Function |
| --- | --- | --- |
| IMU | STMicroelectronics LSM6DSV32X | 6-DoF accelerometer and gyroscope |
| Magnetometer | Memsic MMC5603NJ | 3-DoF |
| Barometer | Goertek SPL07-003 / SPL07-006 | Pressure plus temperature |

Together these form a 10-DoF array on the internal I²C bus.

---

## Component Placement

| Side | Series | Components |
| --- | --- | --- |
| **Top** | MODULE, CHARGE, DATA, LOGGER | ESP32-S3-MINI-1-N4R2, TI BQ25188, TI TPS631000, SiTime SiT1532, TI TLV809EA26DPWR, TI TLV4021R1YKAR, Diodes DMC31D5UDA-7B |
| **Bottom** | LOGGER only | LSM6DSV32X, MMC5603NJ, SPL07-003/006 |

---

## Pinout and Bus Mapping

<!-- TODO: replace placeholder image with the final graphical pinout -->
![HYPER S3 ROCKET graphical pinout](https://placeholders.dev)

*Figure 3: Color-coded functional pin map.*

| Group | GPIO | Hardware feature | Notes |
| --- | --- | --- | --- |
| **External I²C SDA** | GPIO 13 | External bus, onboard pull-up | User expansion sensors; avoids address clashes |
| **External I²C SCL** | GPIO 14 | External bus, onboard pull-up | User expansion sensors; avoids address clashes |
| **Internal I²C SDA** | GPIO 17 | Isolated internal bus, pull-up | Wired to the onboard sensor array |
| **Internal I²C SCL** | GPIO 18 | Isolated internal bus, pull-up | Wired to the onboard sensor array |
| **System GPIO (×16)** | Exposed pins | All on RTC domain | Up to 12 touch channels, 16 ADC lines, deep-sleep wake |
| **Power management** | Internal rail | 2 A buck-boost regulator | Common to every variant |
| **Safety analytics** | Discrete circuit | TI supervisor + comparator | Autonomous UVLO, OVLO and OCLO protection |
| **Thermal profiling** | Analog inputs | Dual NTC ports | Two thermistors: one for charge, one for discharge |

<!-- TODO: link the complete pin-by-pin allocation table here -->
For the complete pin-by-pin allocation, see the full pinout documentation.

---

## Variants

The HYPER S3 ROCKET is a castellated SMT module that can be soldered directly onto a baseboard or used as a breadboard-friendly prototyping board.

| Series | Edition | What's included | Notes |
| --- | --- | --- | --- |
| **MODULE** | Core SMT Module | All core charging, power and safety circuitry plus the ESP32-S3, on a single side with castellated edges | A 10k NTC and a 100k NTC must be soldered in place. `B+` is live and unprotected |
| **CHARGE** | Breadboard Ready | Adds pre-soldered male pin headers and a JST-style battery connector | NTC bypass resistors supplied; remove them before fitting an NTC |
| **DATA** | Data Logger Edition | Adds the 4-bit MicroSD slot | Localized data acquisition |
| **LOGGER** | Full Telemetry | MicroSD slot plus the 10-DoF sensor array | Flagship configuration |

---

## Regulatory Context

> **Note:** This section explains why independent, hardware-level battery protection is standard practice in safety-critical industries. It does not claim that the HYPER S3 ROCKET is certified to any of these standards. Products built on the board need their own assessment and certification.

Battery failures in some industries can cause serious harm, so two-layer, firmware-independent protection is mandated or heavily enforced there. The HYPER S3 ROCKET follows the same design principle, which is why it suits development work across all of these sectors.

<details>
<summary><b>Industry standards and typical secondary safeguards</b></summary>

<br>

| Industry | Primary standards | Core danger addressed | Typical secondary safeguard |
| --- | --- | --- | --- |
| Aerospace | RTCA DO-311A / FAA | Flight-control loss, uncontainable high-altitude fire | Burst discs, independent cell-isolation switches |
| Medical | IEC 60601-1, IEC 62133 / FDA | Patient injury, life-support shutdown | Thermal or chemical fuses inside the cell configuration |
| Automotive | ISO 26262, UN 38.3, UL 2580 | Crash shock, thermal runaway | Pyro-fuses triggered by independent crash sensors |
| Consumer electronics | UL 2054, IEEE 1725 | User burns, residential fires | Secondary overvoltage IC or PTC device |
| Grid storage (BESS) | UL 9540, NFPA 855 | Large-scale explosions, toxic gas | Hardware shunt-trips, gas and fire-suppression interlocks |
| Mining / hazardous areas | IEC 60079-11, ATEX, IECEx | Ignition of gases or combustible dust | Redundant current-limiting resistors, encapsulated fuses |
| Marine | DNV-CG-0157, ABS | Confined-space toxic gas, subsea shorts | Hardwired overvoltage trip-shunts |
| Telecom / data centres | UL 1973 | Infrastructure outage, data-centre fires | Hardware breakers independent of control software |
| Rail / transit | EN 50126, EN 50155 | Tunnel fires, brake power loss | Mechanical contactors on independent analog safety loops |
| Defence | MIL-STD-810 | Ballistic puncture, extreme environments | Redundant isolators independent of firmware |

</details>

---

<div align="center">

**HYPER S3 ROCKET** · Stem Forge

</div>
