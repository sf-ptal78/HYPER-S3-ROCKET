/*
  HYPER S3 ROCKET - Live Sensor Plotter
  STEMFORGE

  Open Tools > Serial Plotter at 115200 baud. Type one letter in the
  message box at the bottom and press Enter:

    a  accelerometer       ax, ay, az        (g)
    g  gyroscope           gx, gy, gz        (deg/s)
    m  magnetometer        mx, my, mz        (uT)
    b  barometer           height (m) above the zero point
    t  temperatures        IMU and barometer (C)
    p  magnetometer path   pass-through <-> IMU sensor hub
    l  next LED pattern    off > each LED > all on > off
    z  re-zero height and gyro (keep the board still)
    x  text messages on/off (off by default: words in the plotter
       end up as legend labels; turn on when using the Serial Monitor)
    h  help text (Serial Monitor)

  Every mode key also refreshes the graph so it rescales to the new
  sensor; press the same key again for a manual refresh.

  Works with either IMU fitted at 0x6B:
    LSM6DS3TR-C (WHO_AM_I 0x6A) - current prototype
    LSM6DSV32X  (WHO_AM_I 0x70) - next prototype
  The MMC5603NJ magnetometer sits behind the IMU. It starts in
  pass-through mode (the ESP32 reads it directly); press p to have
  the IMU's sensor hub read it instead, and p again to switch back.
*/

#include <Wire.h>

// ---------------- Board settings (check against your schematic) ----------------
const int  I2C_SDA = 18;
const int  I2C_SCL = 17;

const int   LED_PINS[]  = {48, 45, 46};
const char* LED_NAMES[] = {"blue (48)", "red (45)", "LED 46"};
const int   NUM_LEDS    = 3;
const bool  LED_ACTIVE_HIGH = true;

const uint8_t IMU_ADDR  = 0x6B;
const uint8_t MAG_ADDR  = 0x30;
const uint8_t BARO_ADDR = 0x76;

const float    BARO_TEMP_OFFSET = -8.5;   // same offset as the SPL07 example sketch
const uint32_t PLOT_INTERVAL_MS = 50;     // 20 lines per second

// Graph refresh on a mode change: the current reading is sent FLUSH_LINES
// times in one instant burst (no sensor reads, no delays), which pushes the
// old trace out of the plotter window so the Y axis rescales immediately.
// Match this to your plotter's window: 500 for this IDE's plotter.
const uint16_t FLUSH_LINES = 500;

// ---------------- State ----------------
enum ImuType { IMU_NONE, IMU_DS3TRC, IMU_DSV32X };
ImuType imuType = IMU_NONE;
bool magOK = false, baroOK = false;
bool hubMode = false;                     // false = pass-through, true = sensor hub

// Magnetometer lock-up detection
bool     magLocked     = false;
uint8_t  magFailCount  = 0;               // consecutive failed reads
uint16_t magSameCount  = 0;               // consecutive identical readings
uint8_t  magLastRaw[6] = {0};
uint32_t lastLockMsg   = 0;
const uint8_t  MAG_FAIL_LIMIT   = 5;      // 5 failed reads (0.25 s) = locked
const uint16_t MAG_FROZEN_LIMIT = 40;     // 40 identical readings (2 s) = frozen
const uint32_t LOCK_MSG_INTERVAL_MS = 5000;

// Text messages. The Arduino 1.8 Serial Plotter turns any line of words into
// trace labels, which fills the legend with junk ("try", "a", "software"...).
// So text is OFF by default while plotting; press x to turn it on when using
// the Serial Monitor instead. The magnetometer lock-up still shows in the
// legend in m mode.
bool textMessages = false;
void msg(const char* s)   { if (textMessages) Serial.print(s); }
void msgln(const char* s) { if (textMessages) Serial.println(s); }

void setPassThrough();                    // defined further down
bool recoverMagBus();
void refreshGraph();

char mode = 'a';
int  ledPattern = 0;                      // 0 = off, 1..NUM_LEDS = one LED, NUM_LEDS+1 = all

// Latest readings
float ax, ay, az;                         // g
float gx, gy, gz;                         // deg/s
float mx, my, mz;                         // uT
float imuTemp, baroTemp;                  // C
float pressure;                           // Pa

float gBiasX = 0, gBiasY = 0, gBiasZ = 0;
float pressureBase = 101325.0;

const float ACC_SCALE  = 0.000488;        // g per LSB at +-16 g (both IMUs)
const float GYRO_SCALE = 0.070;           // dps per LSB at +-2000 dps (both IMUs)

// SPL07-006 calibration coefficients
int32_t c0, c1, c00, c10, c01, c11, c20, c21, c30, c31, c40;
const float BARO_KP = 7864320.0;          // 8x pressure oversampling
const float BARO_KT = 524288.0;           // 1x temperature oversampling

// ---------------- I2C helpers ----------------
bool writeReg(uint8_t dev, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(dev);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}

bool readRegs(uint8_t dev, uint8_t reg, uint8_t* buf, uint8_t len) {
  Wire.beginTransmission(dev);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(dev, len) != len) return false;
  for (uint8_t i = 0; i < len; i++) buf[i] = Wire.read();
  return true;
}

uint8_t readReg(uint8_t dev, uint8_t reg) {
  uint8_t v = 0xFF;
  readRegs(dev, reg, &v, 1);
  return v;
}

int32_t signExtend(int32_t v, int bits) {
  if (v & (1L << (bits - 1))) v -= (1L << bits);
  return v;
}

// ---------------- IMU ----------------
bool initIMU() {
  uint8_t id = readReg(IMU_ADDR, 0x0F);

  if (id == 0x6A) {
    imuType = IMU_DS3TRC;
    writeReg(IMU_ADDR, 0x12, 0x01); delay(50);   // CTRL3_C: software reset
    writeReg(IMU_ADDR, 0x12, 0x44);              // BDU + auto-increment
    writeReg(IMU_ADDR, 0x10, 0x44);              // CTRL1_XL: 104 Hz, +-16 g
    writeReg(IMU_ADDR, 0x11, 0x4C);              // CTRL2_G: 104 Hz, +-2000 dps
    writeReg(IMU_ADDR, 0x1A, 0x00);              // MASTER_CONFIG: hub off
    delay(5);
    writeReg(IMU_ADDR, 0x1A, 0x04);              // MASTER_CONFIG: pass-through on
  }
  else if (id == 0x70) {
    imuType = IMU_DSV32X;
    writeReg(IMU_ADDR, 0x12, 0x01); delay(50);   // CTRL3: software reset
    writeReg(IMU_ADDR, 0x12, 0x44);              // BDU + auto-increment
    writeReg(IMU_ADDR, 0x17, 0x02);              // CTRL8: +-16 g
    writeReg(IMU_ADDR, 0x15, 0x04);              // CTRL6: +-2000 dps
    writeReg(IMU_ADDR, 0x10, 0x06);              // CTRL1: accel 120 Hz, high performance
    writeReg(IMU_ADDR, 0x11, 0x06);              // CTRL2: gyro 120 Hz, high performance
    writeReg(IMU_ADDR, 0x01, 0x40);              // FUNC_CFG_ACCESS: sensor hub bank
    writeReg(IMU_ADDR, 0x14, 0x00);              // MASTER_CONFIG: hub off
    delay(5);
    writeReg(IMU_ADDR, 0x14, 0x10);              // MASTER_CONFIG: pass-through on
    writeReg(IMU_ADDR, 0x01, 0x00);              // back to main bank
  }
  else {
    imuType = IMU_NONE;
    return false;
  }
  delay(20);
  return true;
}

void readIMU() {
  if (imuType == IMU_NONE) return;
  uint8_t b[14];                                 // temp, gyro xyz, accel xyz
  if (!readRegs(IMU_ADDR, 0x20, b, 14)) return;
  int16_t t  = (int16_t)(b[1]  << 8 | b[0]);
  int16_t rx = (int16_t)(b[3]  << 8 | b[2]);
  int16_t ry = (int16_t)(b[5]  << 8 | b[4]);
  int16_t rz = (int16_t)(b[7]  << 8 | b[6]);
  int16_t qx = (int16_t)(b[9]  << 8 | b[8]);
  int16_t qy = (int16_t)(b[11] << 8 | b[10]);
  int16_t qz = (int16_t)(b[13] << 8 | b[12]);

  imuTemp = 25.0 + t / 256.0;
  gx = rx * GYRO_SCALE - gBiasX;
  gy = ry * GYRO_SCALE - gBiasY;
  gz = rz * GYRO_SCALE - gBiasZ;
  ax = qx * ACC_SCALE;
  ay = qy * ACC_SCALE;
  az = qz * ACC_SCALE;
}

// ---------------- Magnetometer (MMC5603NJ) ----------------
bool magAnswers() {
  return readReg(MAG_ADDR, 0x39) == 0x10;              // product ID
}

bool initMag() {
  bool found = magAnswers();
  if (!found) found = recoverMagBus();   // try to unstick it without a power cycle
  if (!found) {
    // The IMU answers but the mag doesn't: almost always the lock-up
    magLocked = (imuType != IMU_NONE);
    return false;
  }
  magLocked = false;
  magFailCount = 0;
  magSameCount = 0;

  writeReg(MAG_ADDR, 0x1C, 0x80); delay(20);           // software reset
  writeReg(MAG_ADDR, 0x1B, 0x08); delay(1);            // SET pulse
  writeReg(MAG_ADDR, 0x1A, 100);                       // ODR 100 Hz
  writeReg(MAG_ADDR, 0x1B, 0xA0);                      // Cmm_freq_en + Auto_SR_en
  writeReg(MAG_ADDR, 0x1D, 0x10);                      // Cmm_en: continuous mode
  return true;
}

// ---------------- Magnetometer lock-up recovery (no power cycle) ----------------
// A lock-up usually happens when the ESP32 resets (new upload, reset button)
// in the middle of a magnetometer transfer. The ESP32 restarts but the IMU and
// magnetometer stay powered: the mag is left half-way through a byte, waiting
// for clock pulses that never come, and ignores everything else.
//
// The standard I2C cure is to send it those clock pulses: 9 or more SCL pulses
// finish whatever byte it was sending, then a STOP returns it to idle. The
// ESP32 can only reach the mag's private bus through the IMU's pass-through
// switch, so the IMU is rebooted and put in pass-through first.

void i2cBusRecovery() {
  Wire.end();                                   // take the pins back from the I2C driver
  pinMode(I2C_SDA, OUTPUT_OPEN_DRAIN); digitalWrite(I2C_SDA, HIGH);
  pinMode(I2C_SCL, OUTPUT_OPEN_DRAIN); digitalWrite(I2C_SCL, HIGH);
  delayMicroseconds(10);

  for (int i = 0; i < 16; i++) {                // clock out any half-finished byte
    digitalWrite(I2C_SCL, LOW);  delayMicroseconds(10);
    digitalWrite(I2C_SCL, HIGH); delayMicroseconds(10);
  }
  // STOP condition: SDA goes low -> high while SCL is high
  digitalWrite(I2C_SCL, LOW);  delayMicroseconds(10);
  digitalWrite(I2C_SDA, LOW);  delayMicroseconds(10);
  digitalWrite(I2C_SCL, HIGH); delayMicroseconds(10);
  digitalWrite(I2C_SDA, HIGH); delayMicroseconds(10);

  Wire.begin(I2C_SDA, I2C_SCL, 400000);         // hand the pins back to the driver
}

bool recoverMagBus() {
  if (imuType == IMU_NONE) return false;
  for (int attempt = 1; attempt <= 3; attempt++) {
    msgln("Mag recovery: rebooting IMU, clocking the bus");
    writeReg(IMU_ADDR, 0x12, 0x80);             // CTRL3: BOOT - reload IMU, stops the hub
    delay(30);
    initIMU();                                  // software reset, sensors on, pass-through on
    hubMode = false;
    i2cBusRecovery();                           // pulses reach the mag through pass-through
    delay(10);
    if (magAnswers()) {
      msgln("Mag recovery: magnetometer answering again");
      return true;
    }
  }
  return false;
}

// ---------------- Magnetometer path: pass-through or sensor hub ----------------
// The two modes must never be on together: always turn one off, wait, then
// turn the other on.

void setPassThrough() {
  if (imuType == IMU_DS3TRC) {
    writeReg(IMU_ADDR, 0x1A, 0x00);   // MASTER_CONFIG: hub master off
    delay(20);                        // let any hub transaction finish (2+ ODR periods)
    writeReg(IMU_ADDR, 0x19, 0x00);   // CTRL10_C: embedded functions off
    writeReg(IMU_ADDR, 0x1A, 0x04);   // MASTER_CONFIG: pass-through on
  }
  else if (imuType == IMU_DSV32X) {
    writeReg(IMU_ADDR, 0x01, 0x40);   // sensor hub bank
    writeReg(IMU_ADDR, 0x14, 0x00);   // MASTER_CONFIG: master off
    delay(20);
    writeReg(IMU_ADDR, 0x14, 0x10);   // MASTER_CONFIG: pass-through on
    writeReg(IMU_ADDR, 0x01, 0x00);   // main bank
    writeReg(IMU_ADDR, 0x03, readReg(IMU_ADDR, 0x03) & ~0x40);  // IF_CFG: hub pull-ups off
  }
  delay(5);
  hubMode = false;
}

void setHub() {
  if (imuType == IMU_DS3TRC) {
    writeReg(IMU_ADDR, 0x1A, 0x00);   // MASTER_CONFIG: pass-through off
    delay(5);
    writeReg(IMU_ADDR, 0x01, 0x80);   // FUNC_CFG_ACCESS: embedded bank A
    writeReg(IMU_ADDR, 0x02, (MAG_ADDR << 1) | 1);  // SLV0_ADD: 0x61 (read)
    writeReg(IMU_ADDR, 0x03, 0x00);   // SLV0_SUBADD: start at Xout0
    writeReg(IMU_ADDR, 0x04, 0x06);   // SLAVE0_CONFIG: read 6 bytes
    writeReg(IMU_ADDR, 0x01, 0x00);   // main bank
    writeReg(IMU_ADDR, 0x19, 0x3C);   // CTRL10_C: embedded functions on (stage 3b value)
    writeReg(IMU_ADDR, 0x1A, 0x09);   // MASTER_CONFIG: master on + internal pull-ups
  }
  else if (imuType == IMU_DSV32X) {
    writeReg(IMU_ADDR, 0x03, readReg(IMU_ADDR, 0x03) | 0x40);   // IF_CFG: hub pull-ups on
    writeReg(IMU_ADDR, 0x01, 0x40);   // sensor hub bank
    writeReg(IMU_ADDR, 0x14, 0x00);   // MASTER_CONFIG: pass-through off
    delay(5);
    writeReg(IMU_ADDR, 0x15, (MAG_ADDR << 1) | 1);  // SLV0_ADD: 0x61 (read)
    writeReg(IMU_ADDR, 0x16, 0x00);   // SLV0_SUBADD: start at Xout0
    writeReg(IMU_ADDR, 0x17, 0x86);   // SLV0_CONFIG: 120 Hz, read 6 bytes
    writeReg(IMU_ADDR, 0x14, 0x04);   // MASTER_CONFIG: master on, one sensor
    writeReg(IMU_ADDR, 0x01, 0x00);   // main bank
  }
  delay(50);                          // give the hub a few cycles to fill its registers
  hubMode = true;
  magFailCount = 0;
  magSameCount = 0;
}

void printLockMessage() {
  msgln("!! MAGNETOMETER LOCKED UP !!");
  msgln("   Disconnect the battery AND the USB cable for 10 seconds, then reconnect.");
  msgln("   (Press p to try a software recovery first.)");
  lastLockMsg = millis();
}

void declareMagLocked(const char* reason) {
  magLocked = true;
  msg("Magnetometer problem: "); msgln(reason);
  printLockMessage();
}

bool tryRecoverMag() {
  msgln("Trying to recover the magnetometer...");
  setPassThrough();                   // stop the hub, back to direct access
  delay(50);
  if (initMag()) {
    magOK = true;
    msgln("Magnetometer recovered - mode: PASS-THROUGH");
    return true;
  }
  printLockMessage();
  return false;
}

void toggleMagPath() {
  if (magLocked) { tryRecoverMag(); return; }
  if (imuType == IMU_NONE || !magOK) {
    msgln("Magnetometer not available - cannot switch mode");
    return;
  }
  if (hubMode) {
    setPassThrough();
    bool ok = (readReg(MAG_ADDR, 0x39) == 0x10);
    msgln(ok ? "Mag mode: PASS-THROUGH (ESP32 reads 0x30 directly)"
                      : "Mag mode: PASS-THROUGH - mag not answering, try a power cycle");
  } else {
    setHub();
    msgln("Mag mode: SENSOR HUB (IMU reads the mag every cycle)");
  }
}

// Fetch the 6 raw magnetometer bytes from whichever path is active
bool readMagRaw(uint8_t* b) {
  if (!hubMode) return readRegs(MAG_ADDR, 0x00, b, 6);

  if (imuType == IMU_DS3TRC) return readRegs(IMU_ADDR, 0x2E, b, 6);  // SENSORHUB1..6_REG

  // LSM6DSV32X: SENSOR_HUB_1..6 live in the sensor hub bank
  writeReg(IMU_ADDR, 0x01, 0x40);
  bool ok = readRegs(IMU_ADDR, 0x02, b, 6);
  writeReg(IMU_ADDR, 0x01, 0x00);
  return ok;
}

void readMag() {
  if (!magOK || magLocked) return;
  uint8_t b[6];

  // 1. No reply on the bus (pass-through lock-up)
  if (!readMagRaw(b)) {
    if (++magFailCount >= MAG_FAIL_LIMIT) declareMagLocked("no reply on I2C");
    return;
  }
  magFailCount = 0;

  // 2. Frozen data: a live sensor always flickers by a few counts.
  //    Identical bytes for 2 s means the mag (or the hub copy) has stopped.
  if (memcmp(b, magLastRaw, 6) == 0) {
    if (++magSameCount >= MAG_FROZEN_LIMIT) {
      bool allZero = !(b[0] | b[1] | b[2] | b[3] | b[4] | b[5]);
      declareMagLocked(hubMode && allZero ? "hub data all zero (check hub pull-ups)"
                                          : "readings frozen");
      return;
    }
  } else {
    magSameCount = 0;
    memcpy(magLastRaw, b, 6);
  }
  // 16-bit, offset binary, 1024 counts per gauss, 1 gauss = 100 uT
  mx = ((int32_t)((b[0] << 8) | b[1]) - 32768) / 1024.0 * 100.0;
  my = ((int32_t)((b[2] << 8) | b[3]) - 32768) / 1024.0 * 100.0;
  mz = ((int32_t)((b[4] << 8) | b[5]) - 32768) / 1024.0 * 100.0;
}

// ---------------- Barometer (SPL07-006) ----------------
bool initBaro() {
  Wire.beginTransmission(BARO_ADDR);
  if (Wire.endTransmission() != 0) return false;

  writeReg(BARO_ADDR, 0x0C, 0x09);                     // soft reset
  delay(40);
  uint32_t start = millis();
  while ((readReg(BARO_ADDR, 0x08) & 0xC0) != 0xC0) {  // COEF_RDY + SENSOR_RDY
    if (millis() - start > 500) return false;
    delay(5);
  }

  uint8_t b[21];
  if (!readRegs(BARO_ADDR, 0x10, b, 21)) return false;
  c0  = signExtend(((int32_t)b[0] << 4) | (b[1] >> 4), 12);
  c1  = signExtend(((int32_t)(b[1] & 0x0F) << 8) | b[2], 12);
  c00 = signExtend(((int32_t)b[3] << 12) | ((int32_t)b[4] << 4) | (b[5] >> 4), 20);
  c10 = signExtend(((int32_t)(b[5] & 0x0F) << 16) | ((int32_t)b[6] << 8) | b[7], 20);
  c01 = (int16_t)(b[8]  << 8 | b[9]);
  c11 = (int16_t)(b[10] << 8 | b[11]);
  c20 = (int16_t)(b[12] << 8 | b[13]);
  c21 = (int16_t)(b[14] << 8 | b[15]);
  c30 = (int16_t)(b[16] << 8 | b[17]);
  c31 = signExtend(((int32_t)b[18] << 4) | (b[19] >> 4), 12);
  c40 = signExtend(((int32_t)(b[19] & 0x0F) << 8) | b[20], 12);

  writeReg(BARO_ADDR, 0x06, 0x43);   // pressure: 16 Hz, 8x oversampling
  writeReg(BARO_ADDR, 0x07, 0xC0);   // temperature: 16 Hz, 1x
  writeReg(BARO_ADDR, 0x09, 0x00);   // no result shift needed at 8x
  writeReg(BARO_ADDR, 0x08, 0x07);   // continuous pressure + temperature
  delay(100);
  return true;
}

void readBaro() {
  if (!baroOK) return;
  uint8_t b[6];
  if (!readRegs(BARO_ADDR, 0x00, b, 6)) return;
  int32_t pr = signExtend(((int32_t)b[0] << 16) | ((int32_t)b[1] << 8) | b[2], 24);
  int32_t tr = signExtend(((int32_t)b[3] << 16) | ((int32_t)b[4] << 8) | b[5], 24);
  float ps = pr / BARO_KP;
  float ts = tr / BARO_KT;

  pressure = c00 + ps * (c10 + ps * (c20 + ps * (c30 + ps * c40)))
           + ts * c01 + ts * ps * (c11 + ps * (c21 + ps * c31));
  baroTemp = c0 * 0.5 + c1 * ts + BARO_TEMP_OFFSET;
}

float heightMetres() {
  return 44330.0 * (1.0 - pow(pressure / pressureBase, 0.1903));
}

// ---------------- Zeroing ----------------
void zeroSensors() {
  msgln("Zeroing - keep the board still");
  float sx = 0, sy = 0, sz = 0, sp = 0;
  const int N = 50;
  gBiasX = gBiasY = gBiasZ = 0;
  for (int i = 0; i < N; i++) {
    readIMU();
    readBaro();
    sx += gx; sy += gy; sz += gz; sp += pressure;
    delay(20);
  }
  gBiasX = sx / N; gBiasY = sy / N; gBiasZ = sz / N;
  if (baroOK) pressureBase = sp / N;
}

// ---------------- LEDs ----------------
void setLED(int i, bool on) {
  digitalWrite(LED_PINS[i], (on == LED_ACTIVE_HIGH) ? HIGH : LOW);
}

void applyLedPattern() {
  for (int i = 0; i < NUM_LEDS; i++) {
    bool on = (ledPattern == NUM_LEDS + 1) || (ledPattern == i + 1);
    setLED(i, on);
  }
  msg("LED: ");
  if (ledPattern == 0)                 msgln("all off");
  else if (ledPattern == NUM_LEDS + 1) msgln("all on");
  else                                 msgln(LED_NAMES[ledPattern - 1]);
}

// ---------------- Commands ----------------
void printHelp() {
  msgln("Keys: a accel, g gyro, m mag, b baro, t temp, p mag path, l LEDs, z zero, x text, h help");
  msg("Mag mode: "); msgln(hubMode ? "SENSOR HUB" : "PASS-THROUGH");
  msg("IMU: ");
  msg(imuType == IMU_DS3TRC ? "LSM6DS3TR-C" : imuType == IMU_DSV32X ? "LSM6DSV32X" : "not found");
  msg("  Mag: ");  msg(magLocked ? "LOCKED UP" : magOK ? "OK" : "not found");
  msg("  Baro: "); msgln(baroOK ? "OK" : "not found");
}

void handleSerial() {
  while (Serial.available()) {
    char c = tolower(Serial.read());
    switch (c) {
      case 'a': case 'g': case 'm': case 'b': case 't':
        mode = c;
        refreshGraph();                 // same key again = manual refresh
        break;
      case 'l':
        ledPattern = (ledPattern + 1) % (NUM_LEDS + 2);
        applyLedPattern(); break;
      case 'p':
        toggleMagPath(); break;
      case 'z':
        zeroSensors(); break;
      case 'x':
        textMessages = !textMessages;
        if (textMessages) Serial.println("Text messages ON (Serial Monitor mode)");
        break;
      case 'h': {                       // help always prints, even with text off
        bool was = textMessages;
        textMessages = true;
        printHelp();
        textMessages = was;
        break;
      }
      default:
        break;   // ignore Enter, spaces and unknown keys
    }
  }
}

// ---------------- Plot output ----------------
// Every mode sends exactly 3 columns. This plotter tracks traces by column
// position (1st = blue, 2nd = red, 3rd = green), so a mode with fewer columns
// would leave the old 3rd trace frozen on screen during a refresh.
char line[128];                           // one plotter line, built once and reused

void buildLine() {
  switch (mode) {
    case 'a':
      snprintf(line, sizeof(line), "ax_g:%.3f,ay_g:%.3f,az_g:%.3f\n", ax, ay, az);
      break;
    case 'g':
      snprintf(line, sizeof(line), "gx_dps:%.3f,gy_dps:%.3f,gz_dps:%.3f\n", gx, gy, gz);
      break;
    case 'm':
      if (magLocked)   // the labels show in the plotter legend, so the instruction is on screen
        snprintf(line, sizeof(line), "MAG_LOCKED:0,unplug_battery_and_USB:0,wait_10s_then_reconnect:0\n");
      else
        snprintf(line, sizeof(line), "mx_uT:%.3f,my_uT:%.3f,mz_uT:%.3f\n", mx, my, mz);
      break;
    case 'b':
      // Height only. Columns 2 and 3 are a ground line at 0 m (drawn on top of
      // each other, so they look like one line) to keep the 3-column rule.
      snprintf(line, sizeof(line), "height_m:%.3f,ground:0,ground:0\n", heightMetres());
      break;
    case 't':
      // third column: average of the two, so it sits between them and doesn't stretch the scale
      snprintf(line, sizeof(line), "imu_C:%.3f,baro_C:%.3f,avg_C:%.3f\n", imuTemp, baroTemp, (imuTemp + baroTemp) / 2.0);
      break;
  }
}

void readAll() {
  readIMU();
  readMag();
  readBaro();
}

// Instant refresh: one fresh reading, repeated FLUSH_LINES times as fast as USB allows
void refreshGraph() {
  readAll();
  buildLine();
  for (uint16_t i = 0; i < FLUSH_LINES; i++) Serial.print(line);
}

// ---------------- Setup / loop ----------------
void setup() {
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && millis() - t0 < 3000) delay(10);   // don't hang on battery

  for (int i = 0; i < NUM_LEDS; i++) { pinMode(LED_PINS[i], OUTPUT); setLED(i, false); }

  Wire.begin(I2C_SDA, I2C_SCL, 400000);

  initIMU();
  magOK  = initMag();     // needs pass-through, so after the IMU
  baroOK = initBaro();

  printHelp();
  if (magLocked) printLockMessage();
  zeroSensors();
}

void loop() {
  handleSerial();

  if (magLocked && millis() - lastLockMsg >= LOCK_MSG_INTERVAL_MS) printLockMessage();

  static uint32_t lastPlot = 0;
  if (millis() - lastPlot >= PLOT_INTERVAL_MS) {
    lastPlot = millis();
    readAll();
    buildLine();
    Serial.print(line);
  }
}
