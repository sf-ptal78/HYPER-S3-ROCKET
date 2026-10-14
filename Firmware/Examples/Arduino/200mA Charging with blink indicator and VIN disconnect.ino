// HYPER S3 ROCKET - BQ25188 charger control + charge LED
// I2C: SDA 18 / SCL 17, BQ25188 at 0x6A
//
// LED patterns:
//   Off         - no USB/VIN
//   Slow blink  - charging (constant current or constant voltage)
//   Steady on   - charge done
//   Fast blink  - VIN present but charger not charging (fault, TS hot/cold, OVP...)
//
// Fully non-blocking (no delay() in the charger code), so chargerBegin() and
// chargerUpdate() can be dropped straight into the main firmware.

#include <Wire.h>

// ---------------- User settings ----------------
#define CHG_LED_PIN         45      // change to the LED you want as the charge indicator
#define CHG_LED_ACTIVE_LOW  false   // true if the LED turns on when the pin is LOW
#define ICHG_CODE           0x2F    // 47 -> 200 mA, bit 7 (CHG_DIS) = 0
#define POLL_MS             250     // how often the charger status is read
#define FAULT_CONFIRM_MS    2000    // "not charging" must last this long before showing a fault
#define SLOW_BLINK_MS       500     // half period -> 1 Hz
#define FAST_BLINK_MS       100     // half period -> 5 Hz

// ---------------- BQ25188 registers ----------------
const uint8_t BQ          = 0x6A;
const uint8_t REG_STAT0   = 0x00;
const uint8_t REG_STAT1   = 0x01;
const uint8_t REG_FLAG0   = 0x02;   // clear-on-read
const uint8_t REG_ICHG    = 0x04;

enum ChgState { CHG_NO_VIN, CHG_CHARGING, CHG_DONE, CHG_FAULT };

ChgState      chgState      = CHG_NO_VIN;
bool          lastVinGood   = false;
uint32_t      lastPoll      = 0;
uint32_t      notChgSince   = 0;     // when "VIN good but not charging" started (0 = not active)
uint8_t       lastStat0     = 0xFF;
uint8_t       lastStat1     = 0xFF;

// ---------------- I2C helpers ----------------
bool bqRead(uint8_t reg, uint8_t &val) {
  Wire.beginTransmission(BQ);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(BQ, (uint8_t)1) != 1) return false;
  val = Wire.read();
  return true;
}

bool bqWrite(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(BQ);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}

int ichgToMa(uint8_t code) {
  code &= 0x7F;
  return (code <= 31) ? (code + 5) : (40 + (code - 31) * 10);
}

// Make sure the charge current register holds our setting (it can reset to default)
void ensureChargeSettings() {
  uint8_t ichg;
  if (bqRead(REG_ICHG, ichg) && ichg == ICHG_CODE) return;
  if (bqWrite(REG_ICHG, ICHG_CODE)) {
    Serial.printf("[CHG] Charge current set to %d mA\n", ichgToMa(ICHG_CODE));
  } else {
    Serial.println("[CHG] Failed to write ICHG");
  }
}

// ---------------- LED ----------------
void ledWrite(bool on) {
  digitalWrite(CHG_LED_PIN, (on != CHG_LED_ACTIVE_LOW) ? HIGH : LOW);
}

void updateLed(uint32_t now) {
  switch (chgState) {
    case CHG_NO_VIN:   ledWrite(false); break;
    case CHG_DONE:     ledWrite(true);  break;
    case CHG_CHARGING: ledWrite((now / SLOW_BLINK_MS) & 1); break;
    case CHG_FAULT:    ledWrite((now / FAST_BLINK_MS) & 1); break;
  }
}

const char *stateName(ChgState s) {
  switch (s) {
    case CHG_NO_VIN:   return "NO VIN";
    case CHG_CHARGING: return "CHARGING";
    case CHG_DONE:     return "DONE";
    default:           return "FAULT";
  }
}

// ---------------- Charger logic ----------------
void chargerBegin() {
  pinMode(CHG_LED_PIN, OUTPUT);
  ledWrite(false);
  uint8_t dummy;
  bqRead(REG_FLAG0, dummy);          // clear old latched flags
  ensureChargeSettings();
}

void chargerUpdate() {
  uint32_t now = millis();

  if (now - lastPoll >= POLL_MS) {
    lastPoll = now;

    uint8_t s0, s1;
    if (!bqRead(REG_STAT0, s0) || !bqRead(REG_STAT1, s1)) {
      chgState = CHG_FAULT;          // charger not answering
    } else {
      bool    vinGood = s0 & 0x01;          // STAT0 bit 0  VIN_PGOOD
      uint8_t chg     = (s0 >> 5) & 0x03;   // STAT0 bits 6:5 CHG_STAT
      uint8_t ts      = (s1 >> 3) & 0x03;   // STAT1 bits 4:3 TS_STAT
      bool    tsOpen  = s0 & 0x80;          // STAT0 bit 7
      bool    vinOvp  = s1 & 0x80;          // STAT1 bit 7

      // Rewrite settings every time USB is plugged in
      if (vinGood && !lastVinGood) ensureChargeSettings();
      lastVinGood = vinGood;

      ChgState next;
      if (!vinGood) {
        next = CHG_NO_VIN;
        notChgSince = 0;
      } else if (chg == 1 || chg == 2) {
        next = CHG_CHARGING;
        notChgSince = 0;
      } else if (chg == 3) {
        next = CHG_DONE;
        notChgSince = 0;
      } else if (ts == 1 || tsOpen || vinOvp) {
        next = CHG_FAULT;              // definite fault, show at once
      } else {
        // VIN good but not charging: can be a short gap at plug-in, so confirm first
        if (notChgSince == 0) notChgSince = now;
        next = (now - notChgSince >= FAULT_CONFIRM_MS) ? CHG_FAULT : chgState;
      }

      // Log only when something changes
      if (next != chgState || s0 != lastStat0 || s1 != lastStat1) {
        uint8_t f0 = 0;
        bqRead(REG_FLAG0, f0);
        Serial.printf("[CHG] %s  STAT0=0x%02X STAT1=0x%02X FLAG0=0x%02X\n",
                      stateName(next), s0, s1, f0);
        lastStat0 = s0;
        lastStat1 = s1;
      }
      chgState = next;
    }
  }

  updateLed(now);
}

// ---------------- Standalone test ----------------
void setup() {
  Serial.begin(115200);
  Wire.begin(18, 17);
  delay(500);                        // only at boot, not in the charger code
  chargerBegin();
}

void loop() {
  chargerUpdate();
  // ...rest of the firmware runs here...
}
