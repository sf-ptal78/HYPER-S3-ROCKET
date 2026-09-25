# HYPER S3 ROCKET Flight Computer & Power Management Board

The **HYPER S3 ROCKET** by **[STEMFORGE](https://stemforge.com.au)** is a highly integrated, production-grade flight computer and smart power management module. Powered by the dual-core **ESP32-S3 SoC**, this board was meticulously engineered to solve the most frustrating hardware constraints in hobby water rocketry, model aerospace tracking, and remote battery/solar-powered IoT nodes.

Unlike standard development boards that rely on fragile nests of external breakout modules, the HYPER S3 ROCKET bundles a full array of flight-grade tracking components into an ultra-compact form factor with **castellated edges**, allowing it to be used flat as a surface-mount module or with traditional breadboard pin headers.

---

## 🚀 Key Hardware Advantages

*   **Zero-Reset Power Path (Texas Instruments BQ25188):** Standard microcontrollers (like the Seeed Studio XIAO or Raspberry Pi Pico) regularly drop voltage and reset when heavy mechanical parachute servos suddenly fire. The HYPER S3 ROCKET manages high current spikes up to 3A without dropping system logic stability.
*   **Hardware-Accelerated Sensor Hub Passthrough:** The high-precision **SPL07-006 Altimeter** is routed directly via the auxiliary master I2C channels of the **LSM6DSV32XTR 6-Axis IMU**. The IMU automatically polls, aggregates, and stores barometric data into its native 4.5 KB FIFO buffer completely in the background—freeing up the ESP32-S3 processor to maintain high-frequency telemetry loops and long-range wireless streaming.
*   **Built-in Safety 1S LiPo Charging:** Fully integrated, protected over-current battery management right on the board.
*   **Water-Resistant Altimeter Cavity:** Upgraded to the **Goertek SPL07-006**, featuring a waterproof gel-filled cavity built to withstand high-moisture water rocket deployment bays.
*   **Rugged Industrial Layout:** Fabricated on a rigid **6-layer PCB** with advanced **EMI mitigation** trace shielding, comprehensive **ESD protection** on external pins, and **reverse polarity hardware hardening**.

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
| **GPIO 39** | SMD Pad External | Flat Surface-Mount Only Pin (MODULE Footprint Optimized) |
| **GPIO 40** | SMD Pad External | Flat Surface-Mount Only Pin (MODULE Footprint Optimized) |
| **GPIO 41** | SMD Pad External | Flat Surface-Mount Only Pin (MODULE Footprint Optimized) |
| **GPIO 42** | SMD Pad External | Flat Surface-Mount Only Pin (MODULE Footprint Optimized) |
| **GPIO 47** | SMD Pad External | Flat Surface-Mount Only Pin (MODULE Footprint Optimized) |

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

## 📊 Family Ecosystem Variations

The HYPER S3 platform scales dynamically across four production configurations to fit varied industrial requirements and spatial constraints:

1. **Hyper S3 MODULE:** Ultra-narrow **17.78mm (0.7")** layout. Strictly castellated pads without through-holes. Engineered explicitly as an SMT drop-in component for high-density commercial automated pick-and-place lines.
2. **Hyper S3 LOGGER:** Complete **20.00mm** hybrid footprint (castellated edges + 2.54mm pin headers). Outfitted with the full 10DoF ±32g sensor array, micro SD socket, and solar harvesting power path management.
3. **Hyper S3 DATA:** Cost-optimized **20.00mm** hybrid layout. Maintains the high-speed SDMMC MicroSD storage, solar charging, and hardware safety layers but strips the 10DoF sensor array for pure environmental, agricultural, or remote structural data-logging.
4. **Hyper S3 CHARGE:** Dedicated **20.00mm** power node layout. Strips sensory infrastructure to maximize efficiency layouts for advanced remote solar energy harvesting, off-grid battery management arrays, and battery-critical remote power setups.

---

## 💻 Production Firmware Framework

The following production code demonstrates a high-reliability aerospace implementation. It initializes the **HYPER S3 ROCKET** internal bus lines, samples the 10DoF cluster at a high-frequency clock interval over fast I2C, streams data out using a double-transmission **Temporal Redundancy** protocol over ESP-NOW to bypass signal drops, and maintains a local **Wi-Fi Access Point Web Server** that outputs raw logs directly as a `.CSV` file for Mac/Safari or Windows clients upon recovery.

```cpp
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <esp_now.h>
#include <FS.h>
#include <SD_MMC.h> 

// Hardware Layer Register Mappings
#define INTERNAL_I2C_SDA     18
#define INTERNAL_I2C_SCL     17
#define BATTERY_SENSE_PIN    16
#define I2C_BUS_SPEED        400000 

// Standardized 10DoF Data Logging Struct
struct __attribute__((packed)) SensorSample {
  uint32_t ms;         // Master internal clock tracking (millis)
  int16_t ax, ay, az;   // Real-time LSM6DSV32XTR Accel Data (Up to ±32g)
  int16_t gx, gy, gz;   // Real-time LSM6DSV32XTR Gyro Data (Up to ±2000 dps)
  int16_t mx, my, mz;   // Real-time MMC5603NJ Magnetometer Heading
  float pressure;       // Real-time SPL07-006 Atmospheric Pressure Data
  uint16_t battery;     // Real-time Raw ADC Battery Track
};

// Redundant ESP-NOW Structural Payload 
struct __attribute__((packed)) EspNowPayload {
  uint32_t packetId;         // Incremental index used to locate skipped transmission frames
  SensorSample historical;   // Retained RAM frame transmitted for its second pass
  SensorSample current;      // Newly sampled frame transmitted for its first pass
};

const char* ssid = "HyperS3-Rocket";
const char* password = "launchcontrol";
WebServer server(80);

File logFile;
SensorSample lastSample;
bool hasHistory = false;
uint32_t packetCounter = 0;
unsigned long lastSampleTime = 0;
const int sampleInterval = 4; // Configures rigid ~250Hz sampling loop

// Converts binary data stored on the SD card into a text CSV for your browser
void handleCSVDownload() {
  File binFile = SD_MMC.open("/flight_log.bin", FILE_READ);
  if (!binFile) {
    server.send(404, "text/plain", "No flight log discovered on SD architecture.");
    return;
  }

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/csv", "");

  // Print Standard CSV Headings
  server.sendContent("Time_ms,Ax,Ay,Az,Gx,Gy,Gz,Mx,My,Mz,Pressure,Voltage\n");

  SensorSample tempPacket;
  String row = "";
  row.reserve(128); // Optimize system memory allocation to maximize parsing throughput

  while (binFile.read((uint8_t*)&tempPacket, sizeof(SensorSample)) == sizeof(SensorSample)) {
    float voltage = (tempPacket.battery / 4095.0) * 2.0 * 3.3; // Convert raw ADC to true Voltage
    row = String(tempPacket.ms) + "," +
          String(tempPacket.ax) + "," + String(tempPacket.ay) + "," + String(tempPacket.az) + "," +
          String(tempPacket.gx) + "," + String(tempPacket.gy) + "," + String(tempPacket.gz) + "," +
