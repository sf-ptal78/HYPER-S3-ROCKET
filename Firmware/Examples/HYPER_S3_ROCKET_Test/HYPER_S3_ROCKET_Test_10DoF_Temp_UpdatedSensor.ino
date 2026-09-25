#include <Wire.h>

#define INTERNAL_I2C_SDA 18
#define INTERNAL_I2C_SCL 17

const uint8_t IMU_ADDR  = 0x6B;
const uint8_t MAG_ADDR  = 0x30;
const uint8_t BARO_ADDR = 0x76;

// Structural baselines for delta stripping calculations
float ax_base = 0, ay_base = 0, az_base = 0;
float mx_base = 0, my_base = 0, mz_base = 0;
float temp_base = 0, press_base = 0;

void writeRegister(uint8_t devAddr, uint8_t regAddr, uint8_t value) {
  Wire.beginTransmission(devAddr);
  Wire.write(regAddr);
  Wire.write(value);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  while(!Serial) { delay(10); }

  Serial.println("🛸 Initializing HYPER S3 ROCKET C++ Test Environment...");
  Wire.begin(INTERNAL_I2C_SDA, INTERNAL_I2C_SCL, 400000);

  // 1. Configure the LSM6DSV32X Master I2C Interface Passthrough engine
  writeRegister(IMU_ADDR, 0x01, 0x80); // FUNC_CFG_ACCESS: Open feature bank
  delay(10);
  writeRegister(IMU_ADDR, 0x14, 0x04); // MASTER_CONFIG: Initialize Sensor Hub master
  writeRegister(IMU_ADDR, 0x01, 0x00); // FUNC_CFG_ACCESS: Return to main bank
  delay(20);

  // 2. Initialize MMC5603NJ Magnetometer Configs
  writeRegister(MAG_ADDR, 0x20, 0x80); // Reboot
  delay(20);
  writeRegister(MAG_ADDR, 0x1B, 0xA0); // Continuous 100Hz

  // 3. Initialize SPL07-006 Altimeter Configurations
  writeRegister(BARO_ADDR, 0x06, 0x03); // Oversampling parameters
  writeRegister(BARO_ADDR, 0x07, 0x83);
  writeRegister(BARO_ADDR, 0x08, 0x07); // Continuous run profile
  
  Serial.println("🔒 Callibrating baseline levels... Keep board rigid.");
  delay(800);

  // Mock read inputs to lock starting flight coordinates
  ax_base = 0.0; ay_base = 0.0; az_base = 1.0; 
  mx_base = 15.0; my_base = -20.0; mz_base = 5.0;
  temp_base = 24.5; press_base = 1013.25;
  Serial.println("Offsets locked! Activating live serialization stream...");
}

void loop() {
  // Simulating active sensor extractions
  float gx = 0.0, gy = 0.0, gz = 0.0;
  float ax = 0.0, ay = 0.0, az = 1.0;
  float mx = 15.2, my = -19.8, mz = 5.1;
  float temp = 24.55, press = 1013.12;

  // Compute deviation values relative to zeroed starting thresholds
  float ax_delta = ax - ax_base;
  float ay_delta = ay - ay_base;
  float az_delta = az - az_base;

  float mx_delta = mx - mx_base;
  float my_delta = my - my_base;
  float mz_delta = mz - mz_base;

  float temp_delta = temp - temp_base;
  float press_delta = press - press_base;

  // =========================================================================
  // UN-COMMENT ONLY ONE SECTION AT A TIME TO PLOT IN ARDUINO SERIAL PLOTTER
  // =========================================================================
  
  // OPTION A: Magnetometer Delta Trends
  Serial.print(mx_delta); Serial.print(" ");
  Serial.print(my_delta); Serial.print(" ");
  Serial.println(mz_delta);

  // OPTION B: Accelerometer Delta Trends
  // Serial.print(ax_delta); Serial.print(" ");
  // Serial.print(ay_delta); Serial.print(" ");
  // Serial.println(az_delta);

  // OPTION C: Raw Angular Rotations
  // Serial.print(gx); Serial.print(" ");
  // Serial.print(gy); Serial.print(" ");
  // Serial.println(gz);

  delay(50); // 20Hz update loop consistency tracking
}
