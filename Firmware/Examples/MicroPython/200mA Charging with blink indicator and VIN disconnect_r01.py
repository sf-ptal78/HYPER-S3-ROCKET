# HYPER S3 ROCKET - BQ25188 charger control + charge LED (MicroPython)
# STEMFORGE
# I2C: SDA 18 / SCL 17, BQ25188 at 0x6A
#
# LED patterns:
#   Off         - no USB/VIN
#   Slow blink  - charging (constant current or constant voltage)
#   Steady on   - charge done
#   Fast blink  - VIN present but charger not charging (fault, TS hot/cold, OVP...)
#
# Fully non-blocking (no sleep() in the charger code), so charger_begin() and
# charger_update() can be dropped straight into a bigger program.

import time
from machine import I2C, Pin

# ---------------- User settings ----------------
CHG_LED_PIN        = 45      # change to the LED you want as the charge indicator
CHG_LED_ACTIVE_LOW = False   # True if the LED turns on when the pin is LOW
ICHG_CODE          = 0x2F    # 47 -> 200 mA, bit 7 (CHG_DIS) = 0
POLL_MS            = 250     # how often the charger status is read
FAULT_CONFIRM_MS   = 2000    # "not charging" must last this long before showing a fault
SLOW_BLINK_MS      = 500     # half period -> 1 Hz
FAST_BLINK_MS      = 100     # half period -> 5 Hz

# ---------------- BQ25188 registers ----------------
BQ        = 0x6A
REG_STAT0 = 0x00
REG_STAT1 = 0x01
REG_FLAG0 = 0x02             # clear-on-read
REG_ICHG  = 0x04

CHG_NO_VIN, CHG_CHARGING, CHG_DONE, CHG_FAULT = 0, 1, 2, 3
STATE_NAMES = ("NO VIN", "CHARGING", "DONE", "FAULT")

i2c = I2C(0, scl=Pin(17), sda=Pin(18), freq=400000)
led = Pin(CHG_LED_PIN, Pin.OUT)

chg_state     = CHG_NO_VIN
last_vin_good = False
last_poll     = time.ticks_ms()
not_chg_since = None          # when "VIN good but not charging" started (None = not active)
last_stat0    = 0xFF
last_stat1    = 0xFF

# ---------------- I2C helpers ----------------
def bq_read(reg):
    try:
        return i2c.readfrom_mem(BQ, reg, 1)[0]
    except OSError:
        return None

def bq_write(reg, val):
    try:
        i2c.writeto_mem(BQ, reg, bytes((val,)))
        return True
    except OSError:
        return False

def ichg_to_ma(code):
    code &= 0x7F
    return code + 5 if code <= 31 else 40 + (code - 31) * 10

# Make sure the charge current register holds our setting (it can reset to default)
def ensure_charge_settings():
    if bq_read(REG_ICHG) == ICHG_CODE:
        return
    if bq_write(REG_ICHG, ICHG_CODE):
        print("[CHG] Charge current set to %d mA" % ichg_to_ma(ICHG_CODE))
    else:
        print("[CHG] Failed to write ICHG")

# ---------------- LED ----------------
def led_write(on):
    led.value(1 if on != CHG_LED_ACTIVE_LOW else 0)

def update_led(now):
    if chg_state == CHG_NO_VIN:
        led_write(False)
    elif chg_state == CHG_DONE:
        led_write(True)
    elif chg_state == CHG_CHARGING:
        led_write((now // SLOW_BLINK_MS) & 1)
    else:
        led_write((now // FAST_BLINK_MS) & 1)

# ---------------- Charger logic ----------------
def charger_begin():
    led_write(False)
    bq_read(REG_FLAG0)                 # clear old latched flags
    ensure_charge_settings()

def charger_update():
    global chg_state, last_vin_good, last_poll, not_chg_since, last_stat0, last_stat1
    now = time.ticks_ms()

    if time.ticks_diff(now, last_poll) >= POLL_MS:
        last_poll = now

        s0 = bq_read(REG_STAT0)
        s1 = bq_read(REG_STAT1)
        if s0 is None or s1 is None:
            chg_state = CHG_FAULT      # charger not answering
        else:
            vin_good = bool(s0 & 0x01)         # STAT0 bit 0  VIN_PGOOD
            chg      = (s0 >> 5) & 0x03        # STAT0 bits 6:5 CHG_STAT
            ts       = (s1 >> 3) & 0x03        # STAT1 bits 4:3 TS_STAT
            ts_open  = bool(s0 & 0x80)         # STAT0 bit 7
            vin_ovp  = bool(s1 & 0x80)         # STAT1 bit 7

            # Rewrite settings every time USB is plugged in
            if vin_good and not last_vin_good:
                ensure_charge_settings()
            last_vin_good = vin_good

            if not vin_good:
                nxt = CHG_NO_VIN
                not_chg_since = None
            elif chg in (1, 2):
                nxt = CHG_CHARGING
                not_chg_since = None
            elif chg == 3:
                nxt = CHG_DONE
                not_chg_since = None
            elif ts == 1 or ts_open or vin_ovp:
                nxt = CHG_FAULT                # definite fault, show at once
            else:
                # VIN good but not charging: can be a short gap at plug-in, so confirm first
                if not_chg_since is None:
                    not_chg_since = now
                nxt = CHG_FAULT if time.ticks_diff(now, not_chg_since) >= FAULT_CONFIRM_MS else chg_state

            # Log only when something changes
            if nxt != chg_state or s0 != last_stat0 or s1 != last_stat1:
                f0 = bq_read(REG_FLAG0) or 0
                print("[CHG] %s  STAT0=0x%02X STAT1=0x%02X FLAG0=0x%02X"
                      % (STATE_NAMES[nxt], s0, s1, f0))
                last_stat0 = s0
                last_stat1 = s1
            chg_state = nxt

    update_led(now)

# ---------------- Standalone test ----------------
time.sleep_ms(500)                     # only at start-up, not in the charger code
charger_begin()

while True:
    charger_update()
    # ...rest of your program runs here...
