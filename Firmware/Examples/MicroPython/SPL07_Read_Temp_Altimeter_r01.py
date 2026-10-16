# HYPER S3 ROCKET - SPL07-006 barometer: pressure, temperature, altitude
# STEMFORGE (MicroPython - no library needed)
#
# The SPL07 measures 16 times a second in the background; this script
# prints the latest values 10 times a second.

import time
from machine import I2C, Pin

BARO_ADDR = 0x76
SEA_LEVEL_PRESSURE_HPA = 1020.0   # adjust to your city's local pressure
TEMP_OFFSET_C = -8.5              # board temperature correction (same as the Arduino example)
PRINT_INTERVAL_MS = 100           # 10 Hz

# Scale factors for the oversampling chosen below
KP = 253952.0                     # pressure: 16x oversampling
KT = 524288.0                     # temperature: 1x oversampling

i2c = I2C(0, scl=Pin(17), sda=Pin(18), freq=400000)

def write_reg(reg, val):
    i2c.writeto_mem(BARO_ADDR, reg, bytes((val,)))

def read_regs(reg, n):
    return i2c.readfrom_mem(BARO_ADDR, reg, n)

def sign_extend(v, bits):
    return v - (1 << bits) if v & (1 << (bits - 1)) else v

def init_baro():
    write_reg(0x0C, 0x09)                          # soft reset
    time.sleep_ms(40)
    start = time.ticks_ms()
    while (read_regs(0x08, 1)[0] & 0xC0) != 0xC0:  # COEF_RDY + SENSOR_RDY
        if time.ticks_diff(time.ticks_ms(), start) > 500:
            raise OSError("SPL07 not ready")
        time.sleep_ms(5)

    b = read_regs(0x10, 21)                        # factory calibration coefficients
    c = {}
    c["c0"]  = sign_extend((b[0] << 4) | (b[1] >> 4), 12)
    c["c1"]  = sign_extend(((b[1] & 0x0F) << 8) | b[2], 12)
    c["c00"] = sign_extend((b[3] << 12) | (b[4] << 4) | (b[5] >> 4), 20)
    c["c10"] = sign_extend(((b[5] & 0x0F) << 16) | (b[6] << 8) | b[7], 20)
    c["c01"] = sign_extend((b[8] << 8) | b[9], 16)
    c["c11"] = sign_extend((b[10] << 8) | b[11], 16)
    c["c20"] = sign_extend((b[12] << 8) | b[13], 16)
    c["c21"] = sign_extend((b[14] << 8) | b[15], 16)
    c["c30"] = sign_extend((b[16] << 8) | b[17], 16)
    c["c31"] = sign_extend((b[18] << 4) | (b[19] >> 4), 12)
    c["c40"] = sign_extend(((b[19] & 0x0F) << 8) | b[20], 12)

    write_reg(0x06, 0x44)    # pressure: 16 Hz, 16x oversampling
    write_reg(0x07, 0xC0)    # temperature: 16 Hz, 1x
    write_reg(0x09, 0x04)    # P_SHIFT: required above 8x oversampling
    write_reg(0x08, 0x07)    # continuous pressure + temperature
    time.sleep_ms(100)
    return c

def read_baro(c):
    b = read_regs(0x00, 6)
    pr = sign_extend((b[0] << 16) | (b[1] << 8) | b[2], 24)
    tr = sign_extend((b[3] << 16) | (b[4] << 8) | b[5], 24)
    ps = pr / KP
    ts = tr / KT
    pressure_pa = (c["c00"] + ps * (c["c10"] + ps * (c["c20"] + ps * (c["c30"] + ps * c["c40"])))
                   + ts * c["c01"] + ts * ps * (c["c11"] + ps * (c["c21"] + ps * c["c31"])))
    temp_c = c["c0"] * 0.5 + c["c1"] * ts + TEMP_OFFSET_C
    return pressure_pa, temp_c

print("\n=============================================")
print("  SPL07-006 16Hz INT SAMPLING / 10Hz MONITOR ")
print("=============================================")

try:
    coeffs = init_baro()
    print("🎉 SUCCESS: SPL07-006 connected at address 0x76!")
except OSError:
    coeffs = None
    print("❌ ERROR: SPL07-006 NOT responding at address 0x76!")

print("---------------------------------------------")
print("Timestamp(ms), Pressure(Pa), Temp(C), Altitude(m)")

last = time.ticks_ms()
while coeffs:
    now = time.ticks_ms()
    if time.ticks_diff(now, last) >= PRINT_INTERVAL_MS:
        last = now
        try:
            p, t = read_baro(coeffs)
        except OSError:
            continue
        altitude = 44330.0 * (1.0 - (p / 100.0 / SEA_LEVEL_PRESSURE_HPA) ** 0.1903)
        print("%d, %.2f Pa, %.2f C, %.2f m" % (now, p, t, altitude))
