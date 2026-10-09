#include <Wire.h>

#define LED_PIN 45
#define I2C_SDA 18
#define I2C_SCL 17

// BQ25188 I2C Address
#define BQ_ADDR 0x6A

// BQ25188 Register Maps
#define REG_STAT0     0x00
#define REG_ICHG_CTRL 0x04 // Corrected from 0x03 to 0x04
#define REG_IC_CTRL2  0x05

// Global Timer Variables
hw_timer_t *timer = NULL;
volatile bool bqCheckFlag = false;          // Raised by ISR, cleared by loop()
volatile uint32_t currentAlarmPeriod = 1000000; // Tracks active toggle rate

// Hardware ISR: Flips the LED state and signals the loop to check the charger
void IRAM_ATTR onTimer() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  bqCheckFlag = true;
}

// Thread-safe I2C Write Helper
void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(BQ_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

// Thread-safe I2C Read Helper
uint8_t readRegister(uint8_t reg) {
  Wire.beginTransmission(BQ_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission() == 0 && Wire.requestFrom(BQ_ADDR, 1)) {
    return Wire.read();
  }
  return 0;
}

void setup() {
  Serial.begin(115200);
  
  // Custom I2C Initialization using your explicit S3 hardware pinout
  Wire.begin(I2C_SDA, I2C_SCL); 
  delay(2000);
  
  Serial.println("\n--- BQ25188 Unified Management Script ---");
  
  // Clean, standard Arduino approach to locking down the pull-down resistor
  pinMode(LED_PIN, INPUT_PULLDOWN);
  pinMode(LED_PIN, OUTPUT);

  // 1. Configure BQ25188 for exactly 200mA Fast Charge Current (using your non-linear formula math!)
  writeRegister(REG_ICHG_CTRL, 0x2F);
  
  // Verify what was written using your parsing logic
  uint8_t ichg_val = readRegister(REG_ICHG_CTRL);
  uint8_t code = ichg_val & 0x7F;
  int ma = (code <= 31) ? (code + 5) : (40 + (code - 31) * 10);
  Serial.printf("BQ25188: Charge Rate configured to %d mA (Disabled Bit: %d)\n", ma, (ichg_val >> 7));

  // 2. Initialize the Hardware Timer at 1MHz base clock 
  timer = timerBegin(1000000); 
  timerAttachInterrupt(timer, &onTimer);
  
  // Start at a default 1Hz flash (500,000us high, 500,000us low)
  timerAlarm(timer, 500000, true, 0); 
}

void loop() {
  // If the interrupt flag hasn't been raised by the hardware timer, skip immediately
  if (!bqCheckFlag) return;
  bqCheckFlag = false; // Reset the execution flag

  // 1. Query charging engine status from register 0x00
  uint8_t stat0 = readRegister(REG_STAT0);
  
  // Official BQ25188 Map: Bit 0 = VIN_PGOOD_STAT, Bits 6-5 = CHG_STAT
  bool vin_pgood = (stat0 & 0x01); 
  uint8_t chg_status = (stat0 >> 5) & 0x03; 

  // --- CASE A: USB / VIN Power is disconnected ---
  if (!vin_pgood) {
    timerAlarm(timer, 0, false, 0);   // Disable background clock interrupts
    pinMode(LED_PIN, INPUT_PULLDOWN); // Terminate pin leakage (LED completely Dark)
    
    currentAlarmPeriod = 0; 
    Serial.print("Power Event: VIN Disconnected. Pin anchored to INPUT_PULLDOWN. ");
    Serial.print("Raw STAT0: 0x");
    Serial.println(stat0, HEX);
    
    // Always service the watchdog so the BQ25188 doesn't force a hard reset
    uint8_t ic_ctrl2 = readRegister(REG_IC_CTRL2);
    writeRegister(REG_IC_CTRL2, ic_ctrl2 | 0x80);
    return; // Exit loop early
  }

  // --- CASE B: VIN is connected ---
  uint32_t targetPeriod = 0;
  bool shouldBlink = true;

  if (chg_status == 0x01) {
    // 01 = Slow Charge (Trickle / Pre-Charge) -> 0.25Hz Blink
    targetPeriod = 2000000; // Toggle pin every 2,000,000us (2s ON / 2s OFF)
    Serial.print("State: Slow / Pre-Charge -> Blinking 0.25Hz. ");
  } else if (chg_status == 0x10) {
    // 10 = Fast Charge / CV Taper Mode -> 1Hz Blink
    targetPeriod = 500000; // Toggle pin every 500,000us (0.5s ON / 0.5s OFF)
    Serial.print("State: Fast Charge / Taper Mode -> Blinking 1Hz. ");
  } else {
    // 00 (Standby/Done) or 11 (Top-off/Termination) -> Blink OFF, LED Solid ON
    shouldBlink = false;
    Serial.print("State: Charging Completed / Standby -> LED Solid ON. ");
  }
  
  Serial.print("Raw STAT0: 0x");
  Serial.println(stat0, HEX);

  // 2. Adjust or handle timer/pin properties based on active state
  if (!shouldBlink) {
    timerAlarm(timer, 0, false, 0); // Stop the background clock interrupts
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);   // Force LED solid ON
    currentAlarmPeriod = 1;        // Flags tracking state out of blinking thresholds
  } else {
    // Re-engage output configuration and update timer if frequency changed
    if (targetPeriod != currentAlarmPeriod) {
      pinMode(LED_PIN, OUTPUT);
      currentAlarmPeriod = targetPeriod;
      timerAlarm(timer, currentAlarmPeriod, true, 0); // Re-engage hardware clock
    }
  }

  // 3. Clear the BQ25188 safety watchdog
  uint8_t ic_ctrl2 = readRegister(REG_IC_CTRL2);
  writeRegister(REG_IC_CTRL2, ic_ctrl2 | 0x80); // Kick watchdog
}
