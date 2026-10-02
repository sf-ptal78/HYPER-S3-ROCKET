🛸 Getting to Know the HYPER S3 ROCKET
The STEMFORGE™ HYPER S3 ROCKET is a production-grade development board and smart power management flight computer powered by the dual-core Espressif ESP32-S3 SoC, designed for aerospace tracking, drone automation, and remote sensor deployments.
🗺️ Visual Hardware Tour
The board features castellated edges for surface-mounting or standard 2.54mm headers, integrating a complete navigation array, PMIC, and status LEDs on the top and bottom layers.
🚀 Versatile Core Applications
• 🚀 Water Rocketry & Aerospace Trackers: Withstands high-moisture launches and 32G acceleration tracking.
• 🛰️ Autonomous Drone Flight Computers: Offloads sensor processing from main processor loops.
• 🔋 Off-Grid IoT & Solar Powered Nodes: Features ultra-low-power Ship Mode via the onboard PMIC.
🧠 Internal Peripherals Architecture
Key onboard sensors and interfaces communicate via a dedicated I2C bus and native connections:
• LSM6DSV32XTR IMU (0x6B) for 6-Axis motion tracking up to 32G.
• SPL07-006 Altimeter (0x76) via auxiliary IMU hub.
• BQ25188 PMIC (0x6A) for power management.
• MMC5603NJ Compass (0x30) for digital heading.
• MicroSD Storage and OPI PSRAM Cache for high-speed logging.
🎛️ External User Input/Output (I/O) Mapping
Includes 16 primary edge pins supporting hardware PWM and ADC channels, alongside specialized flat bottom SMD expansion pads (GPIO 39, 40, 41, 42, and 47) optimized for compact daughterboard layouts.
