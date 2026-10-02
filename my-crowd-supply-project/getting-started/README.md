# Step-by-step Thonny setup guide for backers

# 🛸 Getting Started with HYPER S3 ROCKET

Welcome to the STEMFORGE™ HYPER S3 ROCKET flight computer (development board) ecosystem! Whether you are a beginner or a veteran aerospace engineer, we have tailored an onboarding experience just for you.

---

## 🎉 Great News: MicroPython Comes Pre-Installed!
To give you the smoothest out-of-the-box experience possible, every single HYPER S3 ROCKET comes **pre-flashed with MicroPython firmware**. 

There is no complex software toolchain to install or firmware binaries to upload before you can start coding. Just plug it in, and your flight computer is instantly live!

### 🐍 Option 1: The Quick-Start MicroPython Experience (Recommended)
This is the fastest path to reading telemetry data and testing your flight systems using **Thonny IDE**.

1. Connect the HYPER S3 ROCKET to your computer using a data-capable **USB-C cable**.
2. Download and install **Thonny IDE** from [thonny.org](https://thonny.org).
3. Open Thonny, click on **Tools** > **Options** > **Interpreter**.
4. Select **MicroPython (ESP32)** from the dropdown list, select your board's active COM/Serial Port, and click **OK**.
5. Copy the code from our **`hardware_checkout.py`** script (located in this folder), paste it into Thonny, and click the green **Run (F5)** button. 
6. Watch your onboard diagnostic LEDs cycle and check the terminal to confirm your sensors are online!

---

## 🛠️ Advanced Options: C++ Compilers & Professional IDEs
If your rocketry project demands maximum execution speed, high-frequency flight loops, or low-level memory control, the HYPER S3 ROCKET dual-core ESP32-S3 chip fully supports industry-standard C++ workflows.

*⚠️ Note: Flashing a C++ program will overwrite the pre-installed MicroPython environment. To return to MicroPython later, you will need to re-flash the MicroPython `.bin` file.*

### 🥈 Option 2: The Arduino IDE Compiler
Great for makers who want access to a massive ecosystem of ready-made C++ sensor libraries.
1. Download and install the latest **Arduino IDE** from [arduino.cc](https://arduino.cc).
2. Go to **File** > **Preferences** and add the following URL to your **Additional Boards Manager URLs**:
   ```text
   https://githubusercontent.com
   ```
3. Open the **Boards Manager**, search for **esp32** by Espressif Systems, and hit install.
4. Select **Tools** > **Board** > **ESP32S3 Dev Module** and apply these core hardware settings:
   * **USB CDC On Boot:** Enabled *(Crucial for keeping your Serial Monitor connection active)*
   * **PSRAM:** OPI PSRAM *(Activates your ultra-fast Octal SPIRAM memory cache)*
5. For a complete walkthrough on running your first I2C sensor scan sketch, see our detailed [Arduino IDE Configuration Guide](arduino-setup.md).

### 🥇 Option 3: Professional ESP-IDF or PlatformIO in VS Code
Designed for aerospace developers who want a production-grade, commercial development pipeline.
1. Download and install **Visual Studio Code (VS Code)**.
2. Head to the Extensions marketplace sidebar, search for **PlatformIO IDE** (or the official **Espressif ESP-IDF Extension**), and click install.
3. Create a new project selecting the `Espressif ESP32-S3 Box` or `ESP32S3 Dev Module` as your target profile.
4. Configure your local `platformio.ini` environment file to match the physical board architecture:
   ```ini
   [env:esp32-s3-devkitc-1]
   platform = espressif32
   board = esp32-s3-devkitc-1
   framework = arduino ; Or espidf depending on your preference
   board_build.arduino.memory_type = qio_opi ; Critical for Octal PSRAM configurations
   build_flags = 
       -D ARDUINO_USB_CDC_ON_BOOT=1
   ```
5. Use the terminal task keys inside VS Code to build, optimize, and deploy your custom firmware directly over the native USB JTAG hardware layers.

