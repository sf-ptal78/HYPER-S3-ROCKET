# HYPER S3 ROCKET — Hardware Architecture & Specifications

The **HYPER S3 ROCKET** by Stem Forge is an industrial-grade, ultra-rugged telemetry platform and power management sensor node. Engineered to survive high-G kinetic environments, aerospace logging, and long-term remote deployments, it consolidates a dual-core 240MHz processor, space-optimized analog switching, a high-efficiency power train, and an uncompromised, hardware-enforced protection topology into a microscopic 20mm x 33mm footprint.

By solving complex discrete analog engineering challenges directly on the PCB, it entirely eliminates the need for fragile, bulky multi-board breakout stacks.

---

## 📸 System Overview

<!-- PLACE HERO SHOT HERE -->
![HYPER S3 ROCKET Hero Shot](https://placeholders.dev)
*Figure 1: The HYPER S3 ROCKET Full Telemetry Spec variant displaying castellated SMT edges and high-density component packing.*

---

## 🛠️ Core Hardware Highlights

### 🛡️ Hardware-Enforced "Double Safety Vault" Power Path
Most development boards rely on basic linear charging ICs with zero automated discharge defense. The HYPER S3 ROCKET features a completely autonomous, hardware-level protection layer that guards your system **independently of the ESP32-S3 firmware**. If your code freezes during an active operation, your battery cells and system rails remain perfectly safe.

*   **Integrated eFuse & TI Analytics:** Utilizes a dedicated **Texas Instruments Supervisor and High-Speed Comparator** array to enforce microsecond-level hardware cutoffs.
*   **Complete Discrete Fault Protection:** Instantly isolates the system during critical events:
    *   **UVLO** (Under-Voltage Lockout to prevent permanent LiPo cell degradation)
    *   **OVLO** (Over-Voltage Lockout against faulty, fluctuating, or noisy charging inputs)
    *   **OCLO** (Over-Current Lockout to instantaneously isolate dead shorts or motor stalls)
*   **Dual-Zone Thermal Management:** Exposes dual hardware inputs for external **NTC thin-film thermistors**, allowing developers to map direct thermal profiles of the battery cell and the PCB layout simultaneously.

<!-- PLACE DOUBLE SAFETY DIAGRAM HERE -->
![Double Safety Vault Flow](https://placeholders.dev)
*Figure 2: Hardware-enforced protection topology bypassing the primary MCU firmware layer.*

### ⚡ 2A Buck-Boost Regulation & Smart Power Routing
*   **2A Continuous Buck-Boost:** Equipped with an industrial-grade, ultra-low quiescent current Buck-Boost regulator capable of delivering up to **2A of clean, stable power**. Whether your LiPo cell drops below 3.0V under load or spikes during charging, your system rail never sags.
*   **Dynamic Analog Power Sensing:** Features an integrated **PMOS + NMOS dynamic switching circuit**. This matrix allows the system to seamlessly cycle between high-precision voltage sensing and in-line current tracking across a single analog configuration line without introducing cross-talk or signal degradation.

### 🎛️ Dual Independent I2C Buses (Zero-Conflict Expansion)
Most developer boards force internal and external sensors to share a single I2C bus, introducing massive address conflict risks and bus-stalling hazards. The HYPER S3 ROCKET completely isolates your telemetry stream:
*   **Internal Sensor Bus (GPIO 17 & GPIO 18):** Dedicated exclusively to the onboard 10-DoF IMU and internal sub-systems. Features native, onboard hardware pull-up resistors.
*   **External Expansion Bus (GPIO 13 & GPIO 14):** A completely isolated external bus equipped with its own dedicated hardware pull-ups. Developers can plug in any third-party sensor without risking address overlap, bus stalling, or signal impedance.

### 📡 Uncompromised RTC-Centric I/O & Sensor Density
Every single millimetre of the layout is optimized for high-reliability interaction, specifically tailored for the **Water Rocket Challenge** and rigorous kinetic logging.
*   **16 Multi-Role GPIOs:** Every single exposed pin is routed directly to the ESP32-S3's **RTC (Real-Time Clock) power domain**, enabling rapid wakeups from ultra-deep sleep. 
*   **High-Density Analog & Touch:** The 16 I/O lines are internally mapped to handle up to **12 capacitive touch channels**, **16 high-speed Analog-to-Digital Converter (ADC) channels**, alongside dedicated UART, SPI, and I2C buses.
*   **High-Precision Local Timestamping:** Features an internal, dedicated Real-Time Clock (RTC) architecture allowing precise timestamping on high-speed data logs even when completely disconnected from Wi-Fi or cellular networks.

---

## 📋 Pinout & Bus Architecture

<!-- PLACE INTERACTIVE PINOUT OVERLAY HERE -->
![HYPER S3 ROCKET Graphical Pinout](https://placeholders.dev)
*Figure 3: Color-coded functional pin mapping layout.*

| Pin / Peripheral Group | GPIO Pin Allocation | Native Bus / Hardware Feature | Functional Notes & Interface Requirements |
| :--- | :--- | :--- | :--- |
| **External I2C SDA** | **GPIO 13** | External Bus (Onboard Pull-up) | Dedicated for user expansion sensors; prevents address clashing. |
| **External I2C SCL** | **GPIO 14** | External Bus (Onboard Pull-up) | Dedicated for user expansion sensors; prevents address clashing. |
| **Internal I2C SDA** | **GPIO 17** | Isolated Internal Bus (Pull-up) | Hardwired directly to the Onboard 10-DoF IMU sensor node. |
| **Internal I2C SCL** | **GPIO 18** | Isolated Internal Bus (Pull-up) | Hardwired directly to the Onboard 10-DoF IMU sensor node. |
| **16x System GPIO** | *Exposed Pins* | **All 16 on RTC Domain** | Natively routes 12x Touch Channels, 16x ADC lines, and Deep Sleep wake hooks. |
| **Power Management**| *Internal Rail* | **2A Buck-Boost Regulator** | Main system rail stabilization; standard across all hardware variants. |
| **Safety Analytics** | *Discrete Circuit*| **TI Supervisor + Comparator**| Enforces autonomous discrete hardware protection (UVLO, OVLO, OCLO). |
| **Thermal Profiling**| *Analog Inputs* | **Dual Thin-Film NTC Ports** | Support for 2x external thermistors to monitor cell and layout thermals. |

---

## 📦 Hardware Configurations & Modularity

The HYPER S3 ROCKET is designed as a castellated SMT module, ready to be utilized as a breadboard-friendly prototyping board or surface-mounted directly as a core sub-assembly onto a larger application baseboard.

1. **Basic SMT Module:** Core safe power block, ESP32-S3 processor, and castellated edge pins. Optimized for embedding directly into custom PCB designs.
2. **Breadboard Ready:** Adds pre-soldered male pin headers for instant benchtop prototyping and educational laboratory deployments.
3. **Data Logger Edition:** Integrates an onboard high-speed **MicroSD logging slot** for localized data acquisition.
4. **Full Telemetry Spec:** The flagship configuration. Includes **both** the MicroSD slot and a high-grade **10-DoF IMU sensor array** for complete, uncompromised spatial and kinetic telemetry tracking.
