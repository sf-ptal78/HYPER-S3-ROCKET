# HYPER S3 ROCKET Flight Computer & Power Management Board

The **HYPER S3 ROCKET™** by **[STEMFORGE™](https://stemforge.com.au)** is both a highly capable production-grade development board, flight computer and smart power management module. Powered by the dual-core **ESP32-S3 SoC**, this board was meticulously engineered to solve the most frustrating hardware constraints in hobby water rocketry, model aerospace tracking, and remote battery/solar-powered IoT nodes.

Unlike standard development boards that rely on fragile nests of external breakout modules, the HYPER S3 ROCKET bundles a full array of motion tracking components into an ultra-compact form factor with **castellated edges**, allowing it to be used flat as a surface-mount module or with traditional breadboard pin headers and breadboards.

---

## 🚀 Key Hardware Advantages

*   **Zero-Reset Power Path (Texas Instruments BQ25188):** Standard microcontrollers (like the Seeed Studio XIAO or Raspberry Pi Pico) regularly drop voltage and reset when heavy current draw from servos suddenly activate. The HYPER S3 ROCKET manages high current spikes up to 3A without dropping system logic stability.
*   **Hardware-Accelerated Sensor Hub Passthrough:** The high-precision **Goertek SPL07-006 Altimeter** is routed directly via the auxiliary master I2C channels of the **STMicroelectronics LSM6DSV32XTR 6-Axis IMU**. The IMU automatically polls, aggregates, and stores barometric data into its native 4.5 KB FIFO buffer completely in the background—freeing up the ESP32-S3 processor to maintain high-frequency telemetry loops and long-range wireless streaming.
*   **Built-in Safety 1S LiPo Charging:** Fully integrated, protected over-current battery management right on the board.
*   **Water-Resistant Altimeter Cavity:** Upgraded to the **Goertek SPL07-006**, featuring a waterproof gel-filled cavity built to withstand high-moisture water rocket deployment bays.

---

## 🗺️ ESP32-S3 Pin Allocation & Hardware Map

The HYPER S3 ROCKET isolates external connectivity from internal operations using a highly organized GPIO routing schema. Use this official reference layout to configure your firmware environments:

### 1. 🎛️ External Header Connections (Thru-Hole / Castellated Rim)
These primary 16 pins are fully exposed on the edge of the board via standard 2.54mm thru-holes and castellated rims. **Every single external GPIO pin supports native hardware PWM**, allowing unlimited mapping configuration flexibility for multi-servo arrays, brushless motor drivers, alert buzzers, or LEDs.

*   **GPIO 1** (External Pin 1): **General IO / PWM / Analog & Touch** -> ADC1_CH0 / TOUCH1 *(Safe during Wi-Fi streaming)*
*   **GPIO 2** (External Pin 2): **General IO / PWM / Analog & Touch** -> ADC1_CH1 / TOUCH2 *(Safe during Wi-Fi streaming)*
*   **GPIO 3** (External Pin 3): **General IO / PWM / Analog & Touch** -> ADC1_CH2 / TOUCH3 *(Safe during Wi-Fi streaming / Strapping Pin)*
*   **GPIO 4** (External Pin 4): **General IO / PWM / Analog & Touch** -> ADC1_CH3 / TOUCH4 *(Safe during Wi-Fi streaming)*
*   **GPIO 5** (External Pin 5): **General IO / PWM / Analog & Touch** -> ADC1_CH4 / TOUCH5 *(Safe during Wi-Fi streaming)*
*   **GPIO 6** (External Pin 6): **General IO / PWM / Analog & Touch** -> ADC1_CH5 / TOUCH6 *(Safe during Wi-Fi streaming)*
*   **GPIO 7** (External Pin 7): **General IO / PWM / Analog & Touch** -> ADC1_CH6 / TOUCH7 *(Safe during Wi-Fi streaming)*
*   **GPIO 8** (External Pin 8): **General IO / PWM / Analog & Touch** -> ADC1_CH7 / TOUCH8 *(Safe during Wi-Fi streaming)*
*   **GPIO 9** (**External Pin 11**): **General IO / PWM / Analog & Touch** -> **ADC1_CH8 / TOUCH9** *(Safe during Wi-Fi streaming)*
*   **GPIO 10** (**External Pin 12**): **General IO / PWM / Analog & Touch** -> **ADC1_CH9 / TOUCH10** *(Safe during Wi-Fi streaming)*
*   **GPIO 11** (**External Pin 13**): **General IO / PWM / Analog & Touch** -> **ADC2_CH0 / TOUCH11** *⚠️ Analog blocked during active Wi-Fi*
*   **GPIO 12** (**External Pin 14**): **General IO / PWM / Analog & Touch** -> **ADC2_CH1 / TOUCH12** *⚠️ Analog blocked during active Wi-Fi*
*   **GPIO 13** (**External Pin 15**): **General IO / PWM** -> ADC2_CH2 / TOUCH13 *⚠️ Onboard I2C Pull-up / Analog & Touch not used*
*   **GPIO 14** (**External Pin 16**): **General IO / PWM** -> ADC2_CH3 / TOUCH14 *⚠️ Onboard I2C Pull-up / Analog & Touch not used*
*   **GPIO 43** (External Pin 17 - TXD): **General IO / PWM / Hardware UART TX** -> U0TXD / Default System Serial Console
*   **GPIO 44** (External Pin 18 - RXD): **General IO / PWM / Hardware UART RX** -> U0RXD / Default System Serial Console

---

### 2. 🧠 Internal Peripheral & System Connections
These tracks manage internal communication to onboard coprocessors, sensors, high-speed storage buses, and visual diagnostic indicators. They do not require any external physical wiring.

#### Onboard Dedicated Master I2C Bus
*   **GPIO 17:** **I2C SCL Internal** (Master Serial Clock Line / ADC2_CH6)
*   **GPIO 18:** **I2C SDA Internal** (Master Serial Data Line / ADC2_CH7)

#### Integrated System Components
*   **STMicroelectronics LSM6DSV32XTR IMU**: Primary I2C Address **`0x6B`** (6-Axis Motion IMU)
*   **Goertek SPL07-006 Altimeter**: Primary I2C Address **`0x76`** (Gel-Filled Altimeter routed via LSM6DSV32X Hub)
*   **MEMSIC MMC5603NJ Compass**: Primary I2C Address **`0x30`** (3-Axis Digital Compass)
*   **Texas Instruments BQ25188 PMIC**: Primary I2C Address **`0x6A`** (Power Management IC)
*   **GPIO 0**: User Switch / BOOT -> Hardware Bootloader Selection Trigger / ADC2_CH1
*   **GPIO 15**: Real-Time Clock (RTC) -> Independent System Hardware Timestamping / ADC2_CH4
*   **GPIO 16**: Voltage Sensing -> Analog Input tied to internal battery divider / ADC2_CH5
*   **GPIO 19**: Native USB D- -> Native USB differential data line minus / ADC2_CH8
*   **GPIO 20**: Native USB D+ -> Native USB differential data line plus / ADC2_CH9
*   **GPIO 26**: **PSRAM Configuration** -> **Dedicated High-Speed External PSRAM Caching Rail**

#### Native 4-Bit MicroSD Interface (SDMMC Mode)
*   **GPIO 33:** µSD Card DAT2
*   **GPIO 34:** µSD Card DAT3 / Card Detect (CD)
*   **GPIO 35:** µSD Card CMD
*   **GPIO 36:** µSD Card CLK
*   **GPIO 37:** µSD Card DAT0
*   **GPIO 38:** µSD Card DAT1

#### Visual Status Diagnostic LEDs
*   **GPIO 21:** Top Layer Red LED / Hardware Interrupt — User Programmable LED (Suggested Battery Charging Progress Status) / IMU & Battery Charger Notification of Data/Error
*   **GPIO 45:** Top Layer Green LED — User Programmable LED (Suggested Battery Charging Complete Status)
*   **GPIO 46:** Bottom Layer Red LED — User Programmable LED (Suggested MicroSD Card Activity Indicator)
*   **GPIO 48:** Top Layer Blue LED — User Program LED

---

### 3. 🗺️ Flat SMD Pads (Bottom-Side External Only)
These specialized external pins are located exclusively as flat surface-mount pads on the bottom of the board footprint. They are optimized for custom motherboards, daughterboards, or tight permanent solder connections where physical header pins would consume too much vertical space.

*   **GPIO 39:** SMD Pad External
*   **GPIO 40:** SMD Pad External
*   **GPIO 41:** SMD Pad External
*   **GPIO 42:** SMD Pad External
*   **GPIO 47:** SMD Pad External

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
  Serial.println("🛸 STEMFORGE HYPER S3 ROCKET: INITIALIZATION");
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

*   **Software / Firmware Examples:** Licensed under the **Apache License, Version 2.0** (the "License"); you may not use these files except in compliance with the License. You may obtain a copy of the License at `http://www.apache.org/licenses/LICENSE-2.0`. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
*   **Physical Hardware Design:** The circuit layouts, component placement, schematic traces, layer stackups, and physical PCB architecture of the **HYPER S3 ROCKET** are proprietary intellectual property. Copyright © 2026 by **STEMFORGE**. All rights reserved. Commercial replication, hardware cloning, or unauthorized reverse engineering of the physical board layout is strictly prohibited.
*   **Trademarks:** **STEMFORGE™**, **HYPER S3 ROCKET™**, and their associated product brand logos are proprietary trademarks of **STEMFORGE**. Unauthorized commercial use of these brand marks or names to market unauthorized derivative hardware modules is strictly prohibited.
