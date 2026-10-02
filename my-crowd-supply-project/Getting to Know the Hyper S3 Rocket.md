# 🛸 Getting to Know the HYPER S3 ROCKET

The **STEMFORGE™ HYPER S3 ROCKET** is a production-grade development board, smart power management hub, and flight computer powered by the dual-core **Espressif ESP32-S3 SoC**, designed for water rocketry, aerospace tracking, and remote IoT nodes.

---
![STEMFORGE HYPER S3 ROCKET Front Layout](getting-started/images/Hyper_s3_Rocket_pinout_LOGGER_DATA_CHARGE.avif)

## 📸 Visual Hardware Tour

*   **Board Front View:** `getting-started/images/front-view.png`
*   **Board Back View:** `getting-started/images/back-view.png`

---

## 🚀 Versatile Core Applications

*   **Water Rocketry & Aerospace Trackers:** Handles high-moisture launch environments and high-G deployment forces.
*   **Autonomous Drone Flight Computers:** Offloads heavy background sensor math loops.
*   **Off-Grid IoT & Solar Powered Nodes:** Features ultra-low-power **Ship Mode** to prevent battery drain during storage.

---

## 🧠 Internal Peripherals Architecture

The board integrates key navigational components directly onto the PCB (refer to the full repository documentation for complete I2C addresses and hardware pin mappings):
*   **STMicroelectronics LSM6DSV32XTR:** 6-Axis Motion IMU (up to 32G acceleration).
*   **Goertek SPL07-006 Altimeter:** Waterproof gel-filled cavity for relative flight height.
*   **Texas Instruments BQ25188 PMIC:** Power management supporting software Ship Mode.
*   **MEMSIC MMC5603NJ Compass:** 3-Axis Digital Magnetometer.

---

## 🎛️ External User Input/Output (I/O) Mapping

*   **Edge Pin Connections:** 16 primary pins exposed via standard 2.54mm holes, supporting native hardware PWM across every external GPIO pin. Includes standard ADC/Touch channels and default UART serial console lines on GPIO 43/44.
*   **Flat SMD Pads:** Bottom-side expansion pads (GPIO 39, 40, 41, 42, and 47) optimized for daughterboards and compact permanent solder connections.
