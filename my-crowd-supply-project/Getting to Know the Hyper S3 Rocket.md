# 🛸 Getting to Know the HYPER S3 ROCKET

The **STEMFORGE™ HYPER S3 ROCKET** is a production-grade development board, smart power management hub, and flight computer powered by the dual-core **Espressif ESP32-S3 SoC**, designed for water rocketry, aerospace tracking, and remote IoT nodes.

---
<p align="center">
  <img src="getting-started/images/Hyper_S3_Rocket_Functional_Pinout_Diagram.avif" alt="Hyper S3 Rocket Functional Pinout Diagram" width="900">
</p>

## 📸 Visual Hardware Tour

*   **Board Front & Back Views:** 
<p align="center">
  <img src="getting-started/images/Hyper_S3_Rocket_Board_Anatomy_Diagram.avif" alt="Hyper S3 Rocket Board Anatomy and Component Guide" width="900">
</p>

---

## 🚀 Versatile Core Applications

*   **Commercial Aerospace & Space Flight:** Handles high-moisture launch environments and high-G deployment forces.
*   **Autonomous Drone Flight Computers:** Includes Avionics, Data Logging and considerably more power with a Buck Boost regulator than any other tiny controller.
*   **Off-Grid IoT & Solar Powered Nodes:** Features ultra-low-power **Ship Mode** to prevent battery drain during storage.
*   **Life-Critical Medical Devices**: Dual-layer hardware temperature protection, undervoltage lockout (UVLO), overvoltage and overcurrent safeguards, and firmware-integrated fuel gauge telemetry.

---

## 🧠 Internal Peripherals Architecture

The board integrates core peripheral functionality on every board:
*   **Real Time Clock SIT1532AI-J4-DCC:** SiTime 32.768kHz Real Time Clock enables precision timing on any project.
*   **Texas Instruments Battery Charger BQ25188:** Provides i2c configuration for battery chemistries including Li-ion, Li-Poly, and LiFePO4.
*   **External i2c pullups for ULP:** Will not conflict with internal i2c peripherals, and can even be hardware peripheral controlled from the Ultra Low Power RTC Domain.

The board integrates key navigational components directly onto the PCB (LOGGER - refer to the full repository documentation for complete I2C addresses and hardware pin mappings):
*   **STMicroelectronics LSM6DSV32XTR:** 6-Axis Motion IMU (up to 32G acceleration).
*   **Goertek SPL07-006 Altimeter:** Waterproof gel-filled cavity for relative flight height.
*   **Texas Instruments BQ25188 PMIC:** Power management supporting software Ship Mode.
*   **MEMSIC MMC5603NJ Compass:** 3-Axis Digital Magnetometer.

---

## 🎛️ External User Input/Output (I/O) Mapping

*   **Edge Pin Connections:** 16 primary pins exposed via standard 2.54mm holes, supporting native hardware PWM across every external GPIO pin. Includes standard ADC/Touch channels and default UART serial console lines on GPIO 43/44.
*   **Flat SMD Pads:** Bottom-side expansion pads (GPIO 39, 40, 41, 42, and 47) optimized for daughterboards and compact permanent solder connections.

<p align="center">
  <img src="getting-started/images/Hyper_S3_Rocket_Pinout_GPIO-Allocation.avif" alt="Hyper S3 Rocket Pinout GPIO Allocation" width="900">
</p>
