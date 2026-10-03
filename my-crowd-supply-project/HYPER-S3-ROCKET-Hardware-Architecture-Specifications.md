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



🚀 Production-Ready Hardware Specification
The HYPER S3 ROCKET by Stem Forge is a compact (20mm x 33mm) telemetry platform and power management node designed for high-G and aerospace logging applications, integrating the dual-core ESP32-S3 mini module, efficient power regulation, and robust hardware protection directly onto a single PCB.

• Integrated Dual-core ESP32-S3-Mini-1-N4R2 is at the heart of the Hyper S3 Rocket. With a certified module on 6-layer PCB, you get the benefit of a fast-track to EMI testing. Near Field pre-testing has passed with flying colors. With 21 external (GPIO 16 through-hole and 5 SMD) the other 17 GPIO pins are used internally for sensors and SD enable storage, every pin has been planned for and utilized to get the most from the main Xtensa LX7 microprocessor and from the ULP RTC domain using the ULP RISC-V Coprocessor or the ULP Finite State Machine.

• Efficient Power Regulation: Powered by the TI TPS631000 2MHz buck-boost regulator, the Hyper S3 Rocket will supply far more current efficiently than any other mini sized controller or development board can ever dream of. Delivering continuous current up to 2A (from 3V) of 3.3V output with low ripple, this regulator is far beyond them all.

• Robust Hardware Power & Protection: 
 *   **Power** Featuring a Power Path (PP) Management LiPo charger - the Texas Instruments BQ25188 can input up to 1.1A through the USB-C connector and pass this to the SYS rail or distribute the power between System and Charging duties. And though batteries are not required for the Hyper S3 Rocket, they are an asset as the BQ25188 can manage 3A of battery supply direct to the system with an internal FET in battery only mode or even supplement the incoming power from VIN with additional current from the battery direct to the system rail. For even more power options, the BQ25188 can connect to a nominal 5V Solar Panel (recommended) and it will intelligently harvest the most power possible using VINDPM and IINDPM technologies). This charger is even compatible with i2c control to update charging voltages for different battery chemistries such as Lithium-Ion (Li-Ion), Lithium-Polymer (Li-Poly), and Lithium Iron Phosphate (LiFePO4).
*   **Protection** - For ultimate power, data and battery protection, the Hyper S3 Rocket has been developed with safety in mind. Safety is crucial whether developing toys, educational projects, industrial solutions or medical devices. The Hyper S3 Rocket has specific hardware protection layers to pass necessary battery safety regulations that other development boards and controllers can't.
    *   **ESD and TVS** - Electrostatic discharge (ESD) can send a massive voltage spike through a circuit when connecting a USB cable or by simple touching an external pin of the pcb, instantly destroying sensitive microchips or causing hidden weaknesses that lead to failure later. Rest assured, on the Ultra S3 Rocket, every incoming USB voltage and data, incoming VIN, and outgoing VOUT has ESD protection to suppress transient voltage Spikes, whether positive or negatively charged.
    *   **Reverse Polarity Protection** - Whether connecting a solar panel or the battery, reverse polarity by connecting power to GND will kill the regualtors, microcontrollers and even the charging chip. These integrated circuits do not natively have any reverse polarity protection. To handle reverse polarity and the dual input stage of VIN verses Vusb, Ultra S3 Rocket has two onboard power schottky diodes. These diodes do reduce the incoming charge slightly, yet the benefit far out ways the loss. The battery is not as easy to implement. Though some eFuse have reverse polarity, these cannot have two way current flow for a battery to charge and discharge. Diodes are also out for the same reason. Ultra S3 Rocket has a JST style indented locking notch on the connector. Still, always check red is on the right. For added protection, the B+ external pin is disabled though on the CHARGE, DATA, and LOGGER series. MODULE series is the only one without the PH sized connector and so the external B+ pin is active for an external battery (without reverse polarity protection).
    *    **Dual Layer Battery Safety** The star defender for safety on the Hyper S3 Rocket is the combined finesse of the TI BQ25188 charger chip and the TI TPS259461ARPWR eFuse. Of utmost importance is Battery Under-Voltage Lockout (BUVLO). This is mandatory to prevent catastrophic and permanent damage caused by **over-discharging** of LiPo cells, yet many charger chips on other brand name controllers and development boards have none **or**, only cover the charging duties and completely ignore the safety during the **discharge stage**. Always check for BUVLO with TI for discharge to make sure its battery related and discharge related. Don't get caught out with anything less. In comparison the Hyper S3 Rocket has been developed with stringent regulation in mind where two-layer battery safety standards are legally mandated or heavily enforced in industries where a battery failure could cause mass casualties, critical infrastructure collapse, or the failure of life-saving equipment. These standards reach into the heart of Aerospace and Aviation with standard RTCA (DO-311A),  into Medical Devices with standards IEC 60601-1 and IEC 62133, into Automotive and Electric Vehicles (EVs) such as ISO 26262 (Functional Safety) and UN 38.3, Consumer Electronics & Wearables with standards like UL 2054 and IEEE 1725, Grid Energy Storage Systems (BESS) such as UL 9540 and NFPA 855, Mining and Hazardous Environments (Ex Areas) to comply with ATEX directives (in Europe) or IECEx standards globally for "Intrinsic Safety" (IEC 60079-11), Marine and Subsea Exploration with classification societies that mandate DNV or ABS, Telecommunications and Data Centres
    *Complete Regulatory Overview
Industry	Primary Regulatory Standards	Core Danger Addressed	Typical Secondary Safeguard
Aerospace	RTCA DO-311A / FAA	Flight control loss, uncontainable high-altitude fire	Physical burst discs, independent hardware cell-isolation switches
Medical	IEC 62133 / FDA Guidelines	Patient injury, life-support system shutdown	Chemical/thermal fuses embedded directly into the cell configuration
Automotive	ISO 26262 / UN 38.3 / UL 2580	High-voltage crash shock, highway thermal runaway	Pyro-fuses (explosive disconnects) triggered by independent crash sensors
Consumer Electronics	UL 2054 / IEEE 1725	User burns, facial injuries, residential property fires	Secondary overvoltage IC or internal PTC (Positive Temperature Coefficient) device
Grid Storage (BESS)	UL 9540 / NFPA 855	Megawatt-scale explosions, toxic gas plumes	Independent hardware shunt-trips, automated gas/fire suppression interlocks
Mining / Hazardous	IEC 60079-11 (Intrinsic Safety)	Igniting atmospheric gases or combustible dust	Triple-redundant current-limiting resistors, encapsulated safety fuses
Marine	DNV-CG-0157 / ABS	Confined-space toxic gas, deep-sea pressure shorts	Independent hardwired overvoltage trip-shunts
Telecom / Data	UL 1973	Infrastructure blackout, localized data centre fires	Hardware-managed circuit breakers entirely independent of the control software
Rail & Transit	EN 50126 / EN 50155	Tunnel fires, passenger trapping, brake power loss	Mechanical contactors tied to independent analog backup safety loops
Defence / Military	MIL-STD-810 / Mil-Specs	Ballistic puncture, extreme combat environments	Redundant hardware isolators completely isolated from the firmware layer
    *
    *   The BQ25188 also handles Battery Overcurrent protection and manages the any overvoltage by reducing or shutting down the charging. It also has charging temperature regulation with an NTC thermistor (though not supplied, NTCs can be bought readily according to your own needs). Adding to the TI charger chip, the Hyper S3 Rocket also has a Texas Instruments eFuse TPS259461ARPWR. An eFuse on the battery rail gives unique two-way control over the battery safety for both charging and discharging lockout conditions, such as OVLO, OCLO, and UVLO - UVLO also needed a Texas Instruments Supervisor for true lockout control. The Hyper S3 Rocket also has an additional NTC for Temperature regulated **charging and discharging** via a TI high-speed comparator TLV4021R1YKAR. You can tell, safety at STEM FORGE is not a game.

• Compute & Timing: Built around the ESP32-S3-MINI-1-N4R2 with 4MB Flash and 2MB PSRAM, paired with the SiTime SIT1532AI nano-power MEMS oscillator for RTC precise timestamping.

• Chip Layout:
	• Top Layer (MODULE, CHARGE, DATA & LOGGER series) : Espressif ESP32-S3-MINI-1-N4R2, TI BQ25188, TI TPS631000, SiTime SiT1532 MEMS OSC, TI TLV809EA26DPWR, TI TLV4021R1YKAR, and Diodes DMC31D5UDA-7B.
	• Bottom Layer (LOGGER series only): STM LSM6DSV32XTR (6-DoF IMU), Memsic MMC5603NJ (3-Dof Magnetometer), and Goertek SPL07-003/006 (1-Dof Barometric sensor with Temperature sensing).
   
• I/O & Expansion: Isolated sensors across dual independent I2C buses (internal sensor bus and external expansion bus) with 16 exposed multi-role GPIOs tied to the RTC power domain 


### 🛡️ Hardware-Enforced "Double Safety Vault" Power Path
The board features an autonomous, dual hardware-level protection layers guarding the system independently of the ESP32-S3 firmware, utilizing a dedicated Texas Instruments Battery Charger, Supervisor and High-Speed Comparator array for microsecond-level cutoffs against Charging and Discharging UVLO, OVLO, and OCLO faults, alongside dual hardware inputs for NTC thermistors (one each for charge and for discharge).
*   **Complete Discrete Fault Protection:** Instantly isolates the system during critical events:
    *   **UVLO** (Under-Voltage Lockout to prevent permanent LiPo cell degradation)
    *   **OVLO** (Over-Voltage Lockout against faulty, fluctuating, or noisy charging inputs)
    *   **OCLO** (Over-Current Lockout to instantaneously isolate dead shorts or motor stalls)

- **Micropower Preservation (The Ultimate Safety Net):** During UVLO triggering of the eFuse, the combined parasitic leakage internally **4.4µA** and from the input resistor divider **1.22µA** is a tiny **5.62µA** at the exact moment the hardware-enforced UVLO triggers at **2.63V**, the board ensures the circuit itself won't drain an exhausted cell to death on the shelf. This low-power footprint is a critical safety feature that keeps the battery alive during extended standby after a low-voltage shutdown. After a undervoltage lockout event, power from the Vusb or a 5V nominal Solar panel will power the BQ25188 that governs the SYS rail to release the eFuse and let charging begin.

- *   **Integrated analog sensing for Battery Voltage and Current Analytics:** Utilizes a dedicated Diodes Inc. Mosfet array to toggle between a schottky protected resistor divider network and the current sensing output of the TI eFuse to enable accurate battery management features in esp32-s3 firmware.


### ☀️ Solar-Ready TI Charging & 2A Regulation
*   **TI Solar-Compatible Charging:** Includes a variable solar charging path tied to a **`VIN`** pin breakout.
*   **Dedicated Battery Breakout:** Features a robust **`B+`** pin breakout for direct battery integration.
*   **2A Continuous Buck-Boost:** Delivers up to **2A of stable power** despite input fluctuations.
*   **Dynamic Analog Power Sensing:** Uses a PMOS + NMOS switching circuit for voltage and current tracking.

### 📡 Uncompromised RTC-Centric I/O & Sensor Density
Every single millimetre of the layout is optimized for high-reliability interaction, specifically tailored for the **Water Rocket Challenge** and rigorous kinetic logging.
*   **16 Multi-Role GPIOs:** Every single exposed pin is routed directly to the ESP32-S3's **RTC (Real-Time Clock) power domain**, enabling rapid wakeups from ultra-deep sleep. 
*   **High-Density Analog & Touch:** The 16 I/O lines are internally mapped to handle up to **12 capacitive touch channels**, **16 high-speed Analog-to-Digital Converter (ADC) channels**, alongside dedicated UART, SPI, and I2C buses.
*   **High-Precision Local Timestamping:** Features an internal, dedicated Real-Time Clock (RTC) architecture allowing precise timestamping on high-speed data logs even when completely disconnected from Wi-Fi or cellular networks.

### 🎛️ Dual Independent I2C Buses (Zero-Conflict Expansion)
The HYPER S3 ROCKET isolates your telemetry stream into two separate buses to prevent address conflicts:
*   **Internal Sensor Bus (GPIO 17 & GPIO 18):** Dedicated to the onboard 10-DoF IMU and internal sub-systems with native hardware pull-ups.
*   **External Expansion Bus (GPIO 13 & GPIO 14):** An isolated external bus with dedicated pull-ups for conflict-free third-party sensor integration.


### 🛡️ Industrial Boundary Protection Array
*   **Multi-Rail ESD Protection:** TVS diodes shield critical boundary lines (`Vusb`, `D+`, `D-`, `VIN`, and `B+` pins) against static discharge.
*   **Schottky Gatekeeping:** An inline Schottky diode protects the analog voltage sensing resistor divider by clamping hazardous voltage spikes away from the ESP32-S3 silicon.

### 🚦 Diagnostic User Indicators
*   **Quad Programmable LEDs:** Features four user-addressable status LEDs (3 on the front, 1 on the back) for hardware verification and diagnostics.
## 📋 Pinout Architecture & Bus Mapping

### 🛡️ Hardware-Enforced "Double Safety Vault" Power Path
Most development boards rely on basic linear charging ICs with zero automated discharge defense. The HYPER S3 ROCKET features a completely autonomous, hardware-level protection layer that guards your system **independently of the ESP32-S3 firmware**. If your code freezes during an active operation, your battery cells and system rails remain perfectly safe.


*   **Co-Equal Dual Defense Topology:** Primary charging safety and software-customizable voltage cutoffs are handled via the **Texas Instruments BQ25188** charging management IC. Crucially, the discrete **eFuse** runs in parallel as a vital, hard-wired safety system. If a software glitch misconfigures the BQ25188 registers, or if a firmware crash occurs, the independent eFuse acts as a non-negotiable hardware cutoff that instantaneously isolates the battery during short-circuits or over-current events.
*   **Dual-Zone Dedicated Thermal Control:** 
    *   **Charge-Cycle Monitoring:** The **TI BQ25188** utilizes its own thin-film NTC thermistor input to manage thermal thresholds exclusively while the battery is actively charging.
    *   **Discharge-Cycle Protection:** Because the charger chip is functionally blind when the board is running on battery power, the **eFuse, TI Supervisor, and TI Comparator work in conjunction** to drive the vital temperature control during discharge. This discrete hardware network monitors its own independent NTC thermistor to shut down the main power rail instantly if the cell overheats under high operational loads or rapid depletion.
 
    *   The architecture organizes GPIO allocations, native bus features, and functional notes across dedicated peripherals, system GPIOs, power inputs, and analytics. For the complete and detailed pinout allocation table, please refer to the referenced documentation.

---




<!-- PLACE DOUBLE SAFETY DIAGRAM HERE -->
![Double Safety Vault Flow](https://placeholders.dev)
*Figure 2: Hardware-enforced protection topology bypassing the primary MCU firmware layer.*


*   ### ⚡ High-Density Power Train: TI TPS631000 Buck-Boost Platform
*   **Dynamic Variable Output Delivery:** Utilizing a high-efficiency **Texas Instruments TPS631000** 2MHz switching regulator, the main power rail scales output delivery dynamically based on real-time cell state:
    *   **2.0A Continuous Output:** Delivered seamlessly in buck/stabilizer mode while the battery cell is highly saturated or charging ($V_{IN} \geq 3.0V$).
    *   **1.5A Continuous Output:** Maintained reliably deep into boost mode even as the battery profile sags under operational loads ($V_{IN} \geq 2.7V$).
*   **3-Cycle Seamless Mode Transitions:** The constant frequency peak current control loop automatically modulates between buck, boost, and a highly optimized **3-cycle buck-boost crossover window**. This keeps output voltage ripple tightly bounded below **<20mV**, entirely removing analog noise artifacts from the high-G telemetry data lines.
*   **True Load Disconnect & 8µA Deep Sleep:** Features a true shutdown configuration that physically isolates the load from the power train, paired with a minuscule **8µA quiescent current ($I_Q$)** footprint during low-power standby cycles.

*   **Dynamic Analog Power Sensing:** Features an integrated **PMOS + NMOS dynamic switching circuit**. This matrix allows the system to seamlessly cycle between high-precision voltage sensing and in-line current tracking across a single analog configuration line without introducing cross-talk or signal degradation.

### 🎛️ Dual Independent I2C Buses (Zero-Conflict Expansion)
Most developer boards force internal and external sensors to share a single I2C bus, introducing massive address conflict risks and bus-stalling hazards. The HYPER S3 ROCKET completely isolates your telemetry stream:
*   **Internal Sensor Bus (GPIO 17 & GPIO 18):** Dedicated exclusively to the onboard 10-DoF IMU and internal sub-systems. Features native, onboard hardware pull-up resistors.
*   **External Expansion Bus (GPIO 13 & GPIO 14):** A completely isolated external bus equipped with its own dedicated hardware pull-ups. Developers can plug in any third-party sensor without risking address overlap, bus stalling, or signal impedance.


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

# HYPER S3 ROCKET — Hardware Architecture & Specifications

The **HYPER S3 ROCKET** by Stem Forge is an industrial-grade telemetry platform and power management sensor node designed for high-G environments and remote deployments, featuring a dual-core 240MHz processor in a 20mm x 33mm footprint.


### 💾 4-Bit MicroSD Storage Matrix
*   **4-Bit High-Speed Bus:** Uses the rapid 4-bit native SD bus architecture of the ESP32-S3 instead of slow SPI lines for fast telemetry logging.
*   **Push-Push Enclosure:** A secure, spring-loaded MicroSD holder designed to prevent ejection during high-vibration or high-G launches.

---

The HYPER S3 ROCKET is designed as either an SMT module with castellated pins for embedding or with pin headers for prototyping:

1. **Core SMT Module:** ALL Core features - charging, power and safety, ESP32-S3 processor, on a single side with castellated edges (10k NTC and 100k NTC must be soldered in place).
2. **Breadboard Ready:** Adds pre-soldered male pin headers and a JST style connector for benchtop prototyping (supplied with NTC bypass resistors - remove before adding NTC)
3. **Data Logger Edition:** Integrates an onboard high-speed **MicroSD logging slot** for localized data acquisition.
4. **Full Telemetry Spec:** The flagship configuration. Includes **both** the MicroSD slot and a high-grade **10-DoF IMU sensor array** for complete, uncompromised spatial and kinetic telemetry tracking.

