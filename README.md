# HYPER S3 ROCKET Flight Computer & Power Management Board

The **HYPER S3 ROCKET** by **[Stem Forge](https://stemforge.com.au)** is a highly integrated, production-grade flight computer and smart power management module. Powered by the dual-core **ESP32-S3 SoC**, this board was meticulously engineered to solve the most frustrating hardware constraints in hobby water rocketry, model aerospace tracking, and remote battery/solar-powered IoT nodes.

Unlike standard development boards that rely on fragile nests of external breakout modules, the HYPER S3 ROCKET bundles a full array of flight-grade tracking components into an ultra-compact form factor with **castellated edges**, allowing it to be used flat as a surface-mount module or with traditional breadboard pin headers.

---

## 🚀 Key Hardware Advantages

*   **Zero-Reset Power Path (Texas Instruments BQ25188):** Standard microcontrollers (like the Seeed Studio XIAO or Raspberry Pi Pico) regularly drop voltage and reset when heavy mechanical parachute servos suddenly fire. The HYPER S3 ROCKET manages high current spikes up to 3A without dropping system logic stability.
*   **Hardware-Accelerated Sensor Hub Passthrough:** The high-precision **SPL07-006 Altimeter** is routed directly via the auxiliary master I2C channels of the **LSM6DSV32XTR 6-Axis IMU**. The IMU automatically polls, aggregates, and stores barometric data into its native 4.5 KB FIFO buffer completely in the background—freeing up the ESP32-S3 processor to maintain high-frequency telemetry loops and long-range wireless streaming.
*   **Built-in Safety 1S LiPo Charging:** Fully integrated, protected over-current battery management right on the board.
*   **Water-Resistant Altimeter Cavity:** Upgraded to the **Goertek SPL07-006**, featuring a waterproof gel-filled cavity built to withstand high-moisture water rocket deployment bays.

---

## 🗺️ ESP32-S3 Pin Allocation & Hardware Map

### 🎛️ External Header & Castellated Pinout

| ESP32-S3 GPIO | Board Designation | Primary Hardware Function |
| :--- | :--- | :--- |
| **GPIO 1** | External Pin 1 | General-Purpose IO / PWM / Analog Input |
| **GPIO 2** | External Pin 2 | General-Purpose IO / PWM / Analog Input |
| **GPIO 3** | External Pin 3 | General-Purpose IO / PWM / Analog Input |
| **GPIO 4** | External Pin 4 | General-Purpose IO / PWM / Analog Input |
| **GPIO 5** | External Pin 5 | General-Purpose IO / PWM / Analog Input |
| **GPIO 6** | External Pin 6 | General-Purpose IO / PWM / Analog Input |
| **GPIO 7** | External Pin 7 | General-Purpose IO / PWM / Analog Input |
| **GPIO 8** | External Pin 8 | General-Purpose IO / PWM / Analog Input |
| **GPIO 9** | External Pin 11 | General-Purpose IO |
| **GPIO 10** | External Pin 12 | General-Purpose IO |
| **GPIO 11** | External Pin 13 | General-Purpose IO |
| **GPIO 12** | External Pin 14 | General-Purpose IO |
| **GPIO 13** | External Pin 15 | General-Purpose IO |
| **GPIO 14** | External Pin 16 | General-Purpose IO |
| **GPIO 43** | External Pin 17 - TXD | Dedicated Hardware UART TX |
| **GPIO 44** | External Pin 18 - RXD | Dedicated Hardware UART RX |
| **GPIO 39** | SMD Pad External | Flat Surface-Mount Only Pin |
| **GPIO 40** | SMD Pad External | Flat Surface-Mount Only Pin |
| **GPIO 41** | SMD Pad External | Flat Surface-Mount Only Pin |
| **GPIO 42** | SMD Pad External | Flat Surface-Mount Only Pin |
| **GPIO 47** | SMD Pad External | Flat Surface-Mount Only Pin |

### 🧠 System Peripherals & Internal Bus Layout

| ESP32-S3 GPIO | System Connection | Details & Default I2C Addresses |
| :--- | :--- | :--- |
| **GPIO 17** | **I2C SCL Internal** | Master Onboard I2C Clock Rail |
| **GPIO 18** | **I2C SDA Internal** | Master Onboard I2C Data Rail |
| **—** | *Sensor 1 (IMU Hub)* | **STMicroelectronics LSM6DSV32XTR** (`0x6B`) — 6-Axis Motion, Machine Learning Core, Up to ±32g launch scale |
| **—** | *Sensor 2 (Altimeter)*| **Goertek SPL07-006** (`0x76` via IMU hub) — Waterproof gel-filled barometric pressure sensor |
| **—** | *Sensor 3 (Compass)* | **MEMSIC MMC5603NJ** (`0x30`) — 3-Axis AMR Magnetic Orientation Tracking |
| **—** | *Power Management*  | **Texas Instruments BQ25188** (`0x6A`) — Smart charging power path, up to 3A load handling |
| **GPIO 0** | User Switch / BOOT | Hardwired button line for hardware flashing mode |
| **GPIO 15** | Real-Time Clock (RTC) | Hardware time-stamping engine for independent data tracking |
| **GPIO 16** | Voltage Sensing | Analog connection to internal divider for flight battery drop metrics |
| **GPIO 19** | Native USB D- | Hardware USB Data Minus |
| **GPIO 20** | Native USB D+ | Hardware USB Data Plus |
| **GPIO 26** | PSRAM Configuration | High-speed cache optimization line (Supports models with 2MB PSRAM) |

### 💾 High-Speed 4-Bit MicroSD Interface (SDMMC Mode)
*   **GPIO 33:** µSD Card DAT2
*   **GPIO 34:** µSD Card DAT3 / Card Detect (CD)
*   **GPIO 35:** µSD Card CMD
*   **GPIO 36:** µSD Card CLK
*   **GPIO 37:** µSD Card DAT0
*   **GPIO 38:** µSD Card DAT1

### 💡 Visual Status Indicator Mapping
*   **GPIO 21:** Interrupt (INT) / Red LED
*   **GPIO 45:** Green LED — Battery Charging Status Complete
*   **GPIO 46:** Red LED — MicroSD Card Status / Activity Indicator
*   **GPIO 48:** Blue LED — User Program Custom Defined Light

---

## 💻 Sample Code Block: Initializing the Hardware Bus

```cpp
#include <Wire.h>

#define INTERNAL_I2C_SDA 18
#define INTERNAL_I2C_SCL 17
#define BATTERY_SENSE_PIN 16

#define IMU_ADDR  0x6B  // LSM6DSV32XTR Primary Address
#define MAG_ADDR  0x30  // MMC5603NJ Magnetometer Address
#define PMIC_ADDR 0x6A  // BQ25188 PMIC Address

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); } // Wait for native USB connection
  
  Serial.println("\n=======================================================");
  Serial.println("🛸 STEM FORGE HYPER S3 ROCKET: INITIALIZATION");
  Serial.println("=======================================================");

  // Activate the native internal I2C bus
  bool i2c_bus = Wire.begin(INTERNAL_I2C_SDA, INTERNAL_I2C_SCL);
  if (!i2c_bus) {
    Serial.println("❌ CRITICAL: System Master I2C Interface Bus Failed!");
    while(1);
  }
  Serial.println("✅ Master I2C Bus Online (GPIO 18/17).");

  // Ping internal system architectures
  checkDevice("LSM6DSV32XTR IMU", IMU_ADDR);
  checkDevice("MMC5603NJ Magnetometer", MAG_ADDR);
  checkDevice("BQ25188 Battery Charger", PMIC_ADDR);
  
  Serial.println("\n🚀 System Ready for Flight Sensor Hub Programming.");
  Serial.println("=======================================================\n");
}

void loop() {
  int rawVolts = analogRead(BATTERY_SENSE_PIN);
  float voltage = (rawVolts / 4095.0) * 2.0 * 3.3; // Calibrate ratio for internal divider
  Serial.print("🔋 Launchpad Battery System Power: ");
  Serial.print(voltage);
  Serial.println(" V");
  delay(1000);
}

void checkDevice(const char* name, uint8_t addr) {
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    Serial.print("  [✅ ONLINE]  "); Serial.println(name);
  } else {
    Serial.print("  [❌ OFFLINE] "); Serial.println(name);
  }
}
```

---

## ⚖️ License & Copyright

*   **Software / Firmware Examples:** All source code, libraries, and script examples in this repository are licensed under the **MIT License**. You are free to modify, deploy, and distribute this software framework in your own custom flight builds.
*   **Physical Hardware Design:** The circuit layouts, component placement, schematic traces, layer stackups, and physical PCB architecture of the **HYPER S3 ROCKET** are proprietary intellectual property. Copyright © 2026 by **Stem Forge**. All rights reserved. Commercial replication, hardware cloning, or unauthorized reverse engineering of the physical board layout is strictly prohibited.
