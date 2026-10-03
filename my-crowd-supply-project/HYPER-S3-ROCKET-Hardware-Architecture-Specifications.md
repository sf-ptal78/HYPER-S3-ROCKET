# HYPER S3 ROCKET — Hardware Architecture & Specifications

The **HYPER S3 ROCKET** by Stem Forge is a tough, tiny board for logging data and managing power. It is built for fast, shaky, high-G flights, long outdoor deployments and any job where a sensor board must keep working when conditions get rough.

It packs a dual-core 240 MHz processor, a 2 A power supply, battery protection and (on some versions) a full sensor set into a **20 mm × 33 mm** board. You don't need a stack of loose breakout boards.

![HYPER S3 ROCKET hero shot](https://placeholders.dev)
*Figure 1: The HYPER S3 ROCKET LOGGER version, with castellated edges and tightly packed parts.*

---

## At a Glance

| | |
| --- | --- |
| **Size** | 20 mm × 33 mm, castellated edges (solder it down) or pin headers (use it on a breadboard) |
| **Processor** | Espressif ESP32-S3-MINI-1-N4R2: dual-core 240 MHz, 4 MB flash, 2 MB PSRAM |
| **Clock** | SiTime SiT1532 low-power MEMS oscillator for accurate timestamps |
| **Power supply** | TI TPS631000 buck-boost, 3.3 V output, up to 2 A |
| **Charging** | TI BQ25188 LiPo charger. Works from USB-C or a 5 V solar panel |
| **Battery safety** | TI eFuse, supervisor and comparator that work without any firmware |
| **GPIO** | 21 exposed: 16 through-hole pins (GPIO 1–14, 43, 44) plus 5 SMD pads (GPIO 39, 40, 41, 42, 47). GPIO 1–14 are on the RTC power domain |
| **Buses** | Two separate I²C buses (one internal, one external), 4-bit SD bus |
| **Sensors (LOGGER)** | STM LSM6DSV32X 6-axis motion sensor, Memsic MMC5603NJ magnetometer, Goertek SPL07 pressure and temperature sensor |
| **Storage (DATA, LOGGER)** | MicroSD card, 4-bit bus, push-push holder |
| **Lights** | 4 programmable LEDs (3 on the front, 1 on the back) |

---

## Versions

There are four versions. They all share the same core design.

| Version | What you get | Good for |
| --- | --- | --- |
| **MODULE** | The core: charging, power, safety and the ESP32-S3, on one side with castellated edges. No battery connector. The `B+` pin is active so you can wire in your own battery, but it has no reverse-polarity protection. You must solder your own 10k and 100k NTC thermistors. | Building into your own PCB |
| **CHARGE** | MODULE plus pin headers and a JST-PH battery connector. Comes with bypass resistors in place of the thermistors (remove them when you fit real NTCs). The `B+` pin is switched off. | Breadboards, classrooms, bench testing |
| **DATA** | CHARGE plus the MicroSD slot. | Data logging |
| **LOGGER** | DATA plus the sensor set on the bottom layer. The top-of-the-range board. | Rocketry and motion tracking |

**Top layer (all versions):** ESP32-S3-MINI-1-N4R2, TI BQ25188, TI TPS631000, SiTime SiT1532, TI TLV809EA26DPWR supervisor, TI TLV4021R1YKAR comparator, Diodes DMC31D5UDA-7B MOSFET array.
**Bottom layer (LOGGER only):** LSM6DSV32X, MMC5603NJ, SPL07-003/006.

---

## Brain and Clock

- **ESP32-S3-MINI-1-N4R2** is a ready-made, pre-certified module on a 6-layer board. That should shorten radio and EMC testing. Near-field pre-testing has passed.
- 21 GPIO pins are exposed: 16 through-hole pins (GPIO 1–14, 43 and 44) and 5 SMD pads (GPIO 39, 40, 41, 42 and 47). The rest are used inside the board for the sensors and the SD card.
- Use the two main cores, or let the low-power **ULP** coprocessor run while the main cores sleep.
- The **SiT1532** clock keeps good time, so your data has accurate timestamps even with no Wi-Fi or mobile signal.

---

## Power Supply: TI TPS631000 Buck-Boost

![Double Safety Vault flow](https://placeholders.dev)
*Figure 2: How the safety circuit protects the battery without help from the main processor.*

A buck-boost can step a voltage up or down. That keeps the output steady at 3.3 V as the battery drains.

- **3.3 V output at up to 2 A** when boosting from a 3.0 V input, and **1.5 A** when boosting from a 2.7 V input.
- **Smooth switching** between step-down, step-up and a short crossover mode. Ripple stays under **20 mV**, so your sensor readings stay clean.
- **True load disconnect.** It can fully switch off the load. Standby current is about **8 µA**.
- **Voltage and current sensing.** A small MOSFET array lets one analog pin measure either battery voltage or battery current.

---

## Charging and Power Sources

The **TI BQ25188** charger decides where power comes from:

- **USB-C or solar input**, up to 1.1 A in total. That 1.1 A is shared between running the board and charging the battery. Charging alone is limited to 1 A.
- **Battery only**, up to 3 A to the board.
- **Battery top-up**, when USB or solar can't supply enough on their own. The battery adds current to the board's supply, up to the same 3 A limit.
- **Solar.** Connect a 5 V panel to `VIN`. The charger adjusts to get the most power the panel can give.
- **Settable battery type** over I²C: Li-Ion, Li-Poly or LiFePO₄.

A battery is optional. The board can run from USB or solar alone.

---

## The "Double Safety Vault"

The battery protection **works without the ESP32-S3 firmware**. If your code crashes, the battery and power rails still look after themselves.

### Two protection layers

1. **BQ25188 charger.** Handles charging safety, voltage limits you can change in software, overcurrent, overvoltage (it reduces or stops charging) and charge temperature through an NTC thermistor input.
2. **TPS259461ARPWR eFuse.** A hard-wired switch on the battery line that works in both directions. If a firmware bug leaves the charger set up wrongly, the eFuse still cuts the battery off during a short circuit or overcurrent.

### What it protects against

| Fault | What it stops |
| --- | --- |
| **UVLO** (under-voltage lockout) | Deep discharge, which can permanently damage a LiPo cell |
| **OVLO** (over-voltage lockout) | Faulty, unstable or noisy charging inputs |
| **OCLO** (over-current lockout) | Short circuits and stalled motors |

Many chargers only protect the battery while charging. Here, an eFuse and a TI supervisor chip also protect it while discharging.

### Very low leakage after a shutdown

When the supervisor cuts power at **2.63 V**, the circuit draws only **5.62 µA** (4.4 µA inside the eFuse plus 1.22 µA through the voltage divider). It won't drain a flat battery further while it sits on a shelf. Once USB or a 5 V solar panel is connected, the BQ25188 powers the board and releases the eFuse, so charging can begin again.

### Two temperature checks

- **While charging:** the BQ25188 reads its own NTC thermistor.
- **While running on battery:** the charger can't see the battery, so the eFuse, supervisor and TI **TLV4021R1YKAR** comparator watch a second, separate NTC. They switch off the main rail if the cell overheats.

Thermistors are not included (except as bypass resistors on CHARGE, DATA and LOGGER). Choose one that suits your battery.

### Protection at the board edge

| Pin | Protection |
| --- | --- |
| `Vusb`, `D+`, `D-`, `VIN`, `VOUT`, `B+` | ESD/TVS diodes against static and voltage spikes (positive and negative) |
| Analog voltage divider | A Schottky diode clamps spikes so they can't reach the ESP32-S3 |
| `Vusb` and `VIN` inputs | Two power Schottky diodes block reverse polarity (they cost a small voltage drop) |
| Battery connector | Keyed JST-PH plug. Always check the polarity before you connect a battery |
| `B+` pin | Switched off on CHARGE, DATA and LOGGER. Active but unprotected on MODULE |
| Other GPIO pads | Rely on the ESP32-S3's built-in ESD protection. Handle the board with normal anti-static care |

Why is the battery connector keyed instead of diode-protected? A battery has to charge and discharge, so it needs current to flow both ways. A diode or standard eFuse would block one direction.

<details>
<summary><strong>Why two layers? Battery safety in other industries</strong></summary>

Many industries ask for more than one independent layer of battery protection. This table explains why we built the board this way. It does **not** say the HYPER S3 ROCKET is certified to any of these standards.

| Industry | Main standards | Main danger | Common backup safeguard |
| --- | --- | --- | --- |
| Aerospace | RTCA DO-311A / FAA | Loss of flight control, fire | Burst discs, independent cell-isolation switches |
| Medical | IEC 62133 / FDA guidance | Patient harm, life-support shutdown | Thermal or chemical fuses in the battery pack |
| Automotive | ISO 26262 / UN 38.3 / UL 2580 | Crash shock, thermal runaway | Pyro-fuses set off by separate crash sensors |
| Consumer electronics | UL 2054 / IEEE 1725 | Burns, house fires | Second overvoltage chip or PTC device |
| Grid storage | UL 9540 / NFPA 855 | Large explosions, toxic gas | Shunt-trips, gas and fire suppression |
| Hazardous areas | IEC 60079-11 / ATEX / IECEx | Igniting gas or dust | Redundant current-limiting resistors, sealed fuses |
| Marine | DNV-CG-0157 / ABS | Toxic gas in tight spaces, subsea shorts | Hardwired overvoltage trips |
| Telecom and data | UL 1973 | Outages, data-centre fires | Hardware breakers independent of software |
| Rail | EN 50126 / EN 50155 | Tunnel fires, loss of brakes | Contactors on independent safety loops |
| Defence | MIL-STD-810 | Impact, extreme environments | Redundant isolators independent of firmware |

</details>

---

## Pins and Buses

### GPIO pins

**GPIO 1–14** are on the ESP32-S3's **RTC power domain**, so any of them can wake the chip from deep sleep. **GPIO 1–12** can also work as touch inputs and analog (ADC) inputs: up to 12 of each. **GPIO 13 and 14** carry the external I²C bus, and their onboard pull-up resistors make them unsuitable for touch or analog use.

**GPIO 43 and 44** are the chip's default serial (UART0) pins, used for the console and programming. They are not on the RTC domain and have no touch or ADC function.

**The 5 SMD pads** (GPIO 39, 40, 41, 42 and 47) are general-purpose pins, free for your own use.

UART, SPI and I²C signals can be routed to almost any of these pins.

### Two separate I²C buses

I²C is a two-wire connection used by many sensors. We use two of them, kept apart:

| Bus | Pins | Used for |
| --- | --- | --- |
| **Internal** | SDA GPIO 17, SCL GPIO 18 | The onboard sensors. Has its own pull-up resistors. |
| **External** | SDA GPIO 13, SCL GPIO 14 | Your own sensors. Has its own pull-up resistors. |

The buses are separate, so a faulty or "hung" device on the external bus can't stall your onboard sensors. Two sensors with the same address can also sit on different buses without clashing.

**Tips for the external bus:** keep wires short, and don't plug sensors in or out while the board is powered. If you use long wires, slow the bus down from 400 kHz to 100 kHz.

### 4-bit MicroSD (DATA and LOGGER)

- Uses the ESP32-S3's fast **4-bit SD bus**, not slower SPI, so logging keeps up with your data.
- A **push-push holder** helps stop the card popping out during vibration or a hard launch.

### Status LEDs

Four LEDs you can program yourself (3 on the front, 1 on the back) for checks and debugging.

---

## Pinout

![HYPER S3 ROCKET graphical pinout](https://placeholders.dev)
*Figure 3: Colour-coded map of what each pin does.*

| Group | Pins | Feature | Notes |
| --- | --- | --- | --- |
| **External I²C SDA** | GPIO 13 | Own pull-up | For your sensors |
| **External I²C SCL** | GPIO 14 | Own pull-up | For your sensors |
| **Internal I²C SDA** | GPIO 17 | Own pull-up | Wired to the onboard sensors |
| **Internal I²C SCL** | GPIO 18 | Own pull-up | Wired to the onboard sensors |
| **Through-hole GPIO (×16)** | GPIO 1–14, 43, 44 | GPIO 1–14 on the RTC domain | GPIO 1–12: touch, ADC and deep-sleep wake. GPIO 13 and 14: external I²C (also deep-sleep wake). GPIO 43 and 44: UART0 serial |
| **SMD pad GPIO (×5)** | GPIO 39, 40, 41, 42, 47 | General purpose | Free for your own use |
| **Power** | Internal rail | 2 A buck-boost | Same on every version |
| **Safety circuit** | Internal | TI supervisor and comparator | Handles UVLO, OVLO and OCLO |
| **Temperature** | Analog inputs | Two NTC ports | One for charging, one for discharging |

---

## Built for the Water Rocket Challenge

The HYPER S3 ROCKET was designed with water-rocket flight logging in mind. It is small and light. It survives launch shock, logs quickly to MicroSD, keeps accurate time without a network, and protects its own battery.

---

## Short Glossary

- **eFuse:** an electronic fuse that can switch off and reset itself.
- **ESD / TVS:** parts that absorb static electricity and voltage spikes.
- **NTC thermistor:** a small sensor whose resistance changes with temperature.
- **RTC:** the real-time clock part of the chip that keeps running during sleep.
- **UVLO / OVLO / OCLO:** lockouts that switch power off if voltage is too low, voltage is too high, or current is too high.
