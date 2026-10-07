<div align="center">

# HYPER S3 ROCKET

### Hardware Architecture & Specifications

**A 20 × 33 mm ESP32-S3 powerhouse with fail-safe dual-channel battery protection, solar and USB-C charging, and motion-aware data logging**

*by Stem Forge*

</div>

---

The **HYPER S3 ROCKET** is a compact powerhouse: a feature-rich ESP32-S3 telemetry platform with a fail-safe, dual-channel battery management system built in. Available as a castellated SMD module or a breadboard-ready through-hole board, it brings together USB-C and solar charging, peak-power supplementation from the battery, a 2 A buck-boost regulator, a precision onboard real-time clock, and an optional 10-DoF motion sensor array with high-speed MicroSD logging.

**Battery safety you don't have to engineer yourself.** Two independent layers of protection guard every charge and discharge cycle against thermal stress, overcurrent, overvoltage and undervoltage. Comprehensive battery diagnostics give your firmware what it needs for fuel-gauge estimation: VIN and Battery attached and detached detection, voltage and current sensing, and fault-generated interrupts. Whether you're building an ultra-low-power node or a high-performance system, the diagnostics are designed to be accessible in both active and low-power operation, so you can monitor the battery even while the main cores sleep.

**Skip the patchwork of breakout boards.** Loose connections and unprotected cells are where prototypes fail. The HYPER S3 ROCKET puts charging, protection, regulation, timekeeping and sensing on one board. It began in rocketry and high-G flight logging, and this intelligent BMS node now serves medical, automotive, energy, telecom, marine and rail development just as well, and, of course, rocket science (and aerospace).

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
| **GPIO** | 21 external (16 through-hole, 5 SMD); 14 multi-role GPIOs (GPIO 1-14) on the RTC domain |
| **Castellated pins** | 20 (16 GPIO, `B+`, `VIN`, `GND`, `VOUT`) |
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
| **Onboard RTC** | SiTime nano-power MEMS oscillator, 14 RTC-domain GPIOs, ULP coprocessors, deep-sleep wake | Precise timestamps with no network, and months-long standby on a small cell |
| **Double Safety Vault** | BQ25188 charger plus a discrete eFuse, supervisor and comparator, independent of firmware | Over-discharge, overvoltage, overcurrent and over-temperature faults are caught even if your code crashes |
| **Battery telemetry** | Battery voltage and current sensed through one analog line, I²C-configurable charger, dual NTC inputs | Your firmware can report state of charge, load profile and thermal history, and adapt to the battery chemistry |
| **Data logging** | 4-bit MicroSD, 10-DoF sensor array, dual isolated I²C buses | High-rate, vibration-tolerant logs of motion, pressure, temperature and your own external sensors |

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

The **ESP32-S3-MINI-1-N4R2** is a pre-certified module on a 6-layer PCB, which gives a fast track to EMC testing.

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

When the eFuse UVLO trips at **2.63 V**, the protection circuit itself draws a typical **5.62 µA** (TI): 4.4 µA of internal parasitic leakage plus 1.22 µA through the input resistor divider. An exhausted cell therefore isn't drained further during extended storage.

**Recovery:** applying USB (Vusb) or a nominal 5 V solar panel powers the BQ25188, which governs the SYS rail, releases the eFuse and lets charging begin.

### Input protection

- **ESD / TVS:** every incoming USB voltage and data line, incoming `VIN` and outgoing `VOUT` has TVS protection against positive and negative transients. Static discharge from plugging in a USB cable or touching an exposed pin can destroy a chip or cause latent failures.
- **Reverse polarity (power inputs):** two onboard power Schottky diodes handle reverse polarity and the `VIN` / `Vusb` dual-input stage. They cost a small amount of input voltage, which is a worthwhile trade for protecting the regulator, MCU and charger.
- **Reverse polarity (battery):** a battery needs two-way current flow for charging and discharging, so a simple diode isn't suitable. The **CHARGE**, **DATA** and **LOGGER** variants rely on the keyed, locking notch of the JST PH-style connector, so always confirm polarity before connecting. The **MODULE** series has no connector notch on its `B+` pad, so it has a dedicated P-channel and N-channel MOSFET reverse polarity stage built in for the external battery.
- **B+ pin by variant:** on the **CHARGE**, **DATA** and **LOGGER** series the external `B+` pin is disabled for safety and the battery connects through the JST connector. The **MODULE** series has no JST connector, so `B+` is live for an external battery, protected by the onboard PMOS and NMOS reverse polarity stage.
- **One battery only:** disabling `B+` on the JST variants makes it impossible to connect two batteries at once, which would defeat the safety design. The protection circuitry isn't intended to be modified.

---

## Analog Power Sensing

A Diodes Inc. MOSFET array (PMOS + NMOS) switches a single analog line between two measurements:

- **Battery voltage** through a Schottky-protected resistor divider. The Schottky clamps hazardous spikes away from the ESP32-S3.
- **Battery current** from the eFuse's current-sense output.

This gives firmware accurate battery-management data over one ADC pin, without cross-talk or signal degradation. Firmware reads the line on GPIO 16 and selects the measurement with GPIO 0 (high = voltage, low = current).

---

## I/O and Expansion

### GPIO

The 14 multi-role GPIOs (GPIO 1-14) are routed to the ESP32-S3 **RTC power domain**, enabling fast wake-up from deep sleep and ULP access. Across these lines you can use up to:

- **12** capacitive touch channels
- **12** ADC channels
- dedicated SPI and I²C buses

Two further GPIOs (GPIO 43 and 44) carry the UART Tx and Rx. They are standard GPIOs outside the RTC domain, so they aren't available to the ULP or for deep-sleep wake.

### Dual independent I²C buses

Most boards put internal and external sensors on one I²C bus, which risks address conflicts and bus stalls. The HYPER S3 ROCKET separates them:

| Bus | Pins | Purpose |
| --- | --- | --- |
| **Internal sensor bus** | GPIO 17 (SDA), GPIO 18 (SCL) | Onboard 10-DoF sensor array and internal subsystems, with onboard pull-ups |
| **External expansion bus** | GPIO 1 (SDA), GPIO 2 (SCL) | Third-party sensors, with its own dedicated pull-ups. Pins match the ESP32-S3 ULP hardware I²C, so the ULP can read expansion sensors while the main cores sleep |

### Status LEDs

Four user-addressable LEDs (three on the front, one on the back) for hardware verification and diagnostics.

---

## Storage

**Internal memory (all variants)**

- **4 MB flash** and **2 MB PSRAM** on the ESP32-S3-MINI-1-N4R2 module.

**MicroSD logging (DATA and LOGGER variants)**

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
| **Bottom** | CHARGE, DATA, LOGGER | JST PH battery connector |
| **Bottom** | DATA, LOGGER | MicroSD card push-push holder |
| **Bottom** | LOGGER only | LSM6DSV32X, MMC5603NJ, SPL07-003/006 |

---

## Pinout and Bus Mapping

<!-- TODO: replace placeholder image with the final graphical pinout -->
![HYPER S3 ROCKET graphical pinout](https://placeholders.dev)

*Figure 3: Colour-coded functional pin map.*

### Castellated Pin Map

The module has **20 castellated pins**: 16 GPIOs plus `B+`, `VIN`, `GND` and `VOUT`. Pins are numbered 1 to 20 from the top left, down the left edge and then up the right edge.

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

Of the 16 exposed GPIOs, GPIO 1-14 sit on the RTC domain and give up to 12 touch channels, 12 ADC lines and deep-sleep wake. GPIO 43 and 44 (UART Tx/Rx) are outside the RTC domain. All 16 can output PWM from the main cores, but not from the ULP. GPIO 1 and 2 give up touch and ADC because they carry the external I²C pull-ups; this gives you hardware I²C on the ULP. Removing the pull-ups would restore touch and ADC on those pins, at the cost of ULP hardware I²C.

### Control and Timing

| Group | Pins | Hardware feature | Notes |
| --- | --- | --- | --- |
| **Onboard Button/Switch** | GPIO 0 | User Button/Switching of GPIO16 sensing (High=Voltage, Low=Current) | Hardwired to Boot switch and to Diodes Inc. Dual Mosfet |
| **Internal Timing** | GPIO 15 | Square Wave 32.768 kHz Clock | Hardwired to the onboard SiTime chip |
| **Internal ADC** | GPIO 16 | System Voltage (Battery Mode) and Battery Current | Hardwired to Voltage Divider circuit and eFuse (switched at GPIO0) |

### External GPIO and UART

| Group | Pins | Hardware feature | Notes |
| --- | --- | --- | --- |
| **External GPIO** | GPIO 3 - 14 | Up to 12 of Touch / ADC / deep-sleep wake / PWM | Hardwired to external pins 3-8 and 15-20. GPIO 13 and 14 have no pull-ups and are free for any use |
| **External Tx Pad** | GPIO 43 | UART Transmit Control Pin / PWM (outside the RTC domain) | Hardwired to external pin 14 |
| **External Rx Pad** | GPIO 44 | UART Receive Control Pin / PWM (outside the RTC domain) | Hardwired to external pin 13 |

### Buses

| Group | Pins | Hardware feature | Notes |
| --- | --- | --- | --- |
| **External I²C SDA** | GPIO 1 | External bus, onboard pull-up, ULP hardware I²C compatible | For expansion sensors; no address clashes with onboard parts (pin 1) |
| **External I²C SCL** | GPIO 2 | External bus, onboard pull-up, ULP hardware I²C compatible | As above (pin 2) |
| **Internal I²C SDA** | GPIO 17 | Isolated internal bus, pull-up | Hardwired to the onboard sensors |
| **Internal I²C SCL** | GPIO 18 | Isolated internal bus, pull-up | As above |
| **Internal D-** | GPIO 19 | Isolated internal USB | Hardwired to the ESP32-S3 internal USB & JTAG (length matched) 90 Ohm Impedance |
| **Internal D+** | GPIO 20 | Isolated internal USB | As above |

### Indicators

| Group | Pins | Hardware feature | Notes |
| --- | --- | --- | --- |
| **Internal Interrupt / Front Red LED** | GPIO 21 | Fault Interrupts / Red Indicator LED | Hardwired to Battery Charger, IMU and Internal Front Red Indicator LED (Suggested use Fault Indicator) |
| **Front Green LED** | GPIO 45 | Green Indicator LED | Hardwired to Internal Front Green Indicator LED (Suggested use Charge Indicator with Brightness control) |
| **Rear Red LED** | GPIO 46 | Red Indicator LED | Hardwired to Internal Rear Red Indicator LED (Suggested use SD Card Activity LED) |
| **Front Blue LED** | GPIO 48 | Blue Indicator LED | Hardwired to Internal Front Blue Indicator LED (Suggested use User Defined) |

### Storage, Internal Pads and Unavailable Pins

| Group | Pins | Hardware feature | Notes |
| --- | --- | --- | --- |
| **Not Exposed** | GPIO 22 to 32 | Not exposed or Unavailable | Module Flash and PSRAM consumed GPIO Unavailable for use |
| **MicroSD, 4-bit SDMMC** | GPIO 33 to 38 | Isolated internal bus, pull-ups on all but 36 | Hardwired to Micro SD/MMC Push/Pull Holder (DATA & LOGGER) |
| **SMD PADs** | GPIO 39 to 42 | Exposed pads for additional GPIO | Internal Pads. May restrict Push/Pull Holder mechanism if not careful. Can be used as JTAG with eFuse burning (permanent) |
| **SMD PAD** | GPIO 47 | Exposed pad for additional GPIO | Internal Pad. May restrict Push/Pull Holder mechanism if not careful. |

### Power, Ground and Safety

<!-- TODO: confirm the B+ MOSFET ratings against the SSM6J771G and DMN31D5UFZ datasheets, and decide whether to publish absolute maximum or recommended operating voltages -->
| Group | Pin | Hardware feature | Notes |
| --- | --- | --- | --- |
| **Regulated Voltage Output** | Internal rail / `VOUT` (pin 12) | 2 A Max Buck-Boost | Same on every variant |
| **Regulated Voltage Input** | Internal rail / `VIN` (pin 10) | 18.5 V max, 5 V Nominal | tolerant to 23 V but 5 V is the most efficient and thermally non-restrictive option |
| **Battery External Input** | Internal rail / `B+` (pin 9) | Connected for MODULE series only | eFuse limited to 4.6 V for different battery chemistries. The reverse polarity stage (PMOS SSM6J771G, NMOS DMN31D5UFZ-7B in X2-DFN0606-3) sets the voltage tolerance on this pin: the drain-source ratings (PMOS 20 V, NMOS 30 V) limit normal polarity, and the gate-source ratings (±12 V on both parts) set the reverse polarity maximum. **Onboard PMOS + NMOS reverse polarity protection absolute maximum is +20 V and -12 V**, substantially beyond 1s batteries voltages. |
| **Regulated Ground** | Internal rail / `GND` (pin 11) | Ground connection | Unbroken Ground Planes on PCB layers 2 and 5 are essential ground return paths that also provide EMI shielding and protect sensors from switching currents |
| **Safety Limits** | Discrete circuit | TI supervisor + comparator | Autonomous UVLO / OVLO / OCLO protection |

### Thermal Profiling (NTC Inputs)

Two 2.0 mm through-hole NTC ports accept custom-length, thin-film, flexible beta 3435 NTC sensors (sensors not provided).

| NTC | Purpose |
| --- | --- |
| **10k** | Charge regulation |
| **100k** | Charge and discharge limit and lockout |

- **CHARGE, DATA and LOGGER:** if thin film thermistors are required, remove the supplied 0402 chip thermistors at NTC1 and NTC2 before installing your custom NTCs.
- **MODULE:** NTCs are required for charging & discharging, and bypassing is not available.

---

## Variants

The HYPER S3 ROCKET is a castellated SMT module that can be soldered directly onto a baseboard or used as a breadboard-friendly prototyping board.

| Series | Edition | What's included | Notes |
| --- | --- | --- | --- |
| **MODULE** | Core SMT Module | All core charging, power and safety circuitry plus the ESP32-S3, on a single side with castellated edges | A 10k NTC and a 100k NTC must be soldered in place. `B+` is live, with onboard PMOS and NMOS reverse polarity protection |
| **CHARGE** | Breadboard Ready | Adds pre-soldered male pin headers and a JST-style battery connector | NTC chip thermistors are supplied; remove them before fitting custom thin-film thermistors |
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
