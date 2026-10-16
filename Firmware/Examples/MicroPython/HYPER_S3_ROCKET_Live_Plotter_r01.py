"""
  HYPER S3 ROCKET - Live Sensor Plotter (MicroPython)
  STEMFORGE

  In Thonny: View > Plotter. Run this script, then type one letter in the
  Shell and press Enter:

    a  accelerometer       ax, ay, az        (g)
    g  gyroscope           gx, gy, gz        (deg/s)
    m  magnetometer        mx, my, mz        (uT)
    h  height              height (m) above the zero point
    b  barometer           pressure (hPa) and the zero-point pressure
    t  temperatures        IMU and barometer (C)
    p  magnetometer path   pass-through <-> IMU sensor hub
    l  next LED pattern    off > each LED > all on > off
    z  re-zero height and gyro (keep the board still)
    x  text messages on/off (off by default so the plot stays clean)
    ?  help text

  Every mode key also refreshes the graph so it rescales to the new
  sensor; press the same key again for a manual refresh.

  Works with either IMU fitted at 0x6B:
    LSM6DS3TR-C (WHO_AM_I 0x6A) - current prototype
    LSM6DSV32X  (WHO_AM_I 0x70) - next prototype
  The MMC5603NJ magnetometer sits behind the IMU. It starts in
  pass-through mode (the ESP32 reads it directly); press p to have
  the IMU's sensor hub read it instead, and p again to switch back.
"""

import sys
import time
import select
import struct
from machine import I2C, Pin

# ---------------- Board settings ----------------
I2C_SDA = 18
I2C_SCL = 17

LED_PINS  = (48, 45, 46)
LED_NAMES = ("blue (48)", "green (45)", "bottom red (46)")
LED_ACTIVE_HIGH = True

IMU_ADDR  = 0x6B
MAG_ADDR  = 0x30
BARO_ADDR = 0x76

BARO_TEMP_OFFSET = -8.5     # same offset as the SPL07 example
PLOT_INTERVAL_MS = 50       # 20 lines per second

# Graph refresh on a mode change: the current reading is sent FLUSH_LINES
# times in one burst, pushing the old trace out of the plotter window so the
# Y axis rescales straight away. Thonny's plotter shows fewer points than the
# Arduino one, so a smaller number is enough.
FLUSH_LINES = 200

ACC_SCALE  = 0.000488       # g per LSB at +-16 g (both IMUs)
GYRO_SCALE = 0.070          # dps per LSB at +-2000 dps (both IMUs)
BARO_KP = 7864320.0         # 8x pressure oversampling
BARO_KT = 524288.0          # 1x temperature oversampling

MAG_FAIL_LIMIT   = 5        # 5 failed reads (0.25 s) = locked
MAG_FROZEN_LIMIT = 40       # 40 identical readings (2 s) = frozen
LOCK_MSG_INTERVAL_MS = 5000

IMU_NONE, IMU_DS3TRC, IMU_DSV32X = 0, 1, 2


class State:
    imu_type = IMU_NONE
    mag_ok = False
    baro_ok = False
    hub_mode = False          # False = pass-through, True = sensor hub
    mag_locked = False
    mag_fail_count = 0
    mag_same_count = 0
    mag_last_raw = b""
    last_lock_msg = 0
    text_messages = False
    mode = "a"
    led_pattern = 0           # 0 = off, 1..3 = one LED, 4 = all
    ax = ay = az = 0.0
    gx = gy = gz = 0.0
    mx = my = mz = 0.0
    imu_temp = baro_temp = 0.0
    pressure = 101325.0
    pressure_base = 101325.0
    g_bias = [0.0, 0.0, 0.0]
    coeffs = None

s = State()
leds = [Pin(p, Pin.OUT) for p in LED_PINS]
i2c = None


def msg(text):
    if s.text_messages:
        print(text)


# ---------------- I2C helpers ----------------
def i2c_start():
    global i2c
    i2c = I2C(0, scl=Pin(I2C_SCL), sda=Pin(I2C_SDA), freq=400000)

def write_reg(dev, reg, val):
    try:
        i2c.writeto_mem(dev, reg, bytes((val,)))
        return True
    except OSError:
        return False

def read_regs(dev, reg, n):
    try:
        return i2c.readfrom_mem(dev, reg, n)
    except OSError:
        return None

def read_reg(dev, reg):
    b = read_regs(dev, reg, 1)
    return b[0] if b else 0xFF

def sign_extend(v, bits):
    return v - (1 << bits) if v & (1 << (bits - 1)) else v

def device_answers(addr):
    try:
        i2c.writeto(addr, b"")
        return True
    except OSError:
        return False


# ---------------- IMU ----------------
def init_imu():
    who = read_reg(IMU_ADDR, 0x0F)
    if who == 0x6A:
        s.imu_type = IMU_DS3TRC
        write_reg(IMU_ADDR, 0x12, 0x01); time.sleep_ms(50)  # CTRL3_C: software reset
        write_reg(IMU_ADDR, 0x12, 0x44)                     # BDU + auto-increment
        write_reg(IMU_ADDR, 0x10, 0x44)                     # CTRL1_XL: 104 Hz, +-16 g
        write_reg(IMU_ADDR, 0x11, 0x4C)                     # CTRL2_G: 104 Hz, +-2000 dps
        write_reg(IMU_ADDR, 0x1A, 0x00)                     # MASTER_CONFIG: hub off
        time.sleep_ms(5)
        write_reg(IMU_ADDR, 0x1A, 0x04)                     # MASTER_CONFIG: pass-through on
    elif who == 0x70:
        s.imu_type = IMU_DSV32X
        write_reg(IMU_ADDR, 0x12, 0x01); time.sleep_ms(50)  # CTRL3: software reset
        write_reg(IMU_ADDR, 0x12, 0x44)                     # BDU + auto-increment
        write_reg(IMU_ADDR, 0x17, 0x02)                     # CTRL8: +-16 g
        write_reg(IMU_ADDR, 0x15, 0x04)                     # CTRL6: +-2000 dps
        write_reg(IMU_ADDR, 0x10, 0x06)                     # CTRL1: accel 120 Hz, high performance
        write_reg(IMU_ADDR, 0x11, 0x06)                     # CTRL2: gyro 120 Hz, high performance
        write_reg(IMU_ADDR, 0x01, 0x40)                     # FUNC_CFG_ACCESS: sensor hub bank
        write_reg(IMU_ADDR, 0x14, 0x00)                     # MASTER_CONFIG: hub off
        time.sleep_ms(5)
        write_reg(IMU_ADDR, 0x14, 0x10)                     # MASTER_CONFIG: pass-through on
        write_reg(IMU_ADDR, 0x01, 0x00)                     # back to main bank
    else:
        s.imu_type = IMU_NONE
        return False
    time.sleep_ms(20)
    return True

def read_imu():
    if s.imu_type == IMU_NONE:
        return
    b = read_regs(IMU_ADDR, 0x20, 14)                       # temp, gyro xyz, accel xyz
    if not b:
        return
    t, rx, ry, rz, qx, qy, qz = struct.unpack("<7h", b)
    s.imu_temp = 25.0 + t / 256.0
    s.gx = rx * GYRO_SCALE - s.g_bias[0]
    s.gy = ry * GYRO_SCALE - s.g_bias[1]
    s.gz = rz * GYRO_SCALE - s.g_bias[2]
    s.ax = qx * ACC_SCALE
    s.ay = qy * ACC_SCALE
    s.az = qz * ACC_SCALE


# ---------------- Magnetometer (MMC5603NJ) ----------------
def mag_answers():
    return read_reg(MAG_ADDR, 0x39) == 0x10                 # product ID

def init_mag():
    found = mag_answers()
    if not found:
        found = recover_mag_bus()      # try to unstick it without a power cycle
    if not found:
        # The IMU answers but the mag doesn't: almost always the lock-up
        s.mag_locked = (s.imu_type != IMU_NONE)
        return False
    s.mag_locked = False
    s.mag_fail_count = 0
    s.mag_same_count = 0

    write_reg(MAG_ADDR, 0x1C, 0x80); time.sleep_ms(20)      # software reset
    write_reg(MAG_ADDR, 0x1B, 0x08); time.sleep_ms(1)       # SET pulse
    write_reg(MAG_ADDR, 0x1A, 100)                          # ODR 100 Hz
    write_reg(MAG_ADDR, 0x1B, 0xA0)                         # Cmm_freq_en + Auto_SR_en
    write_reg(MAG_ADDR, 0x1D, 0x10)                         # Cmm_en: continuous mode
    return True


# ---------------- Magnetometer lock-up recovery (no power cycle) ----------------
# A lock-up usually happens when the ESP32 resets in the middle of a
# magnetometer transfer. The IMU and magnetometer stay powered and the mag is
# left half-way through a byte. 9 or more clock pulses on SCL finish that byte,
# then a STOP returns it to idle. The ESP32 can only reach the mag's private
# bus through the IMU's pass-through switch, so the IMU is rebooted first.

def i2c_bus_recovery():
    sda = Pin(I2C_SDA, Pin.OPEN_DRAIN, value=1)   # take the pins from the I2C driver
    scl = Pin(I2C_SCL, Pin.OPEN_DRAIN, value=1)
    time.sleep_us(10)
    for _ in range(16):                           # clock out any half-finished byte
        scl(0); time.sleep_us(10)
        scl(1); time.sleep_us(10)
    # STOP condition: SDA goes low -> high while SCL is high
    scl(0); time.sleep_us(10)
    sda(0); time.sleep_us(10)
    scl(1); time.sleep_us(10)
    sda(1); time.sleep_us(10)
    i2c_start()                                   # hand the pins back to the I2C driver

def recover_mag_bus():
    if s.imu_type == IMU_NONE:
        return False
    for _ in range(3):
        msg("Mag recovery: rebooting IMU, clocking the bus")
        write_reg(IMU_ADDR, 0x12, 0x80)           # CTRL3: BOOT - reload IMU, stops the hub
        time.sleep_ms(30)
        init_imu()                                # software reset, sensors on, pass-through on
        s.hub_mode = False
        i2c_bus_recovery()
        time.sleep_ms(10)
        if mag_answers():
            msg("Mag recovery: magnetometer answering again")
            return True
    return False


# ---------------- Magnetometer path: pass-through or sensor hub ----------------
# The two modes must never be on together: always turn one off, wait, then
# turn the other on.

def set_pass_through():
    if s.imu_type == IMU_DS3TRC:
        write_reg(IMU_ADDR, 0x1A, 0x00)   # MASTER_CONFIG: hub master off
        time.sleep_ms(20)                 # let any hub transaction finish
        write_reg(IMU_ADDR, 0x19, 0x00)   # CTRL10_C: embedded functions off
        write_reg(IMU_ADDR, 0x1A, 0x04)   # MASTER_CONFIG: pass-through on
    elif s.imu_type == IMU_DSV32X:
        write_reg(IMU_ADDR, 0x01, 0x40)   # sensor hub bank
        write_reg(IMU_ADDR, 0x14, 0x00)   # MASTER_CONFIG: master off
        time.sleep_ms(20)
        write_reg(IMU_ADDR, 0x14, 0x10)   # MASTER_CONFIG: pass-through on
        write_reg(IMU_ADDR, 0x01, 0x00)   # main bank
        write_reg(IMU_ADDR, 0x03, read_reg(IMU_ADDR, 0x03) & ~0x40)  # IF_CFG: hub pull-ups off
    time.sleep_ms(5)
    s.hub_mode = False

def set_hub():
    if s.imu_type == IMU_DS3TRC:
        write_reg(IMU_ADDR, 0x1A, 0x00)   # MASTER_CONFIG: pass-through off
        time.sleep_ms(5)
        write_reg(IMU_ADDR, 0x01, 0x80)   # FUNC_CFG_ACCESS: embedded bank A
        write_reg(IMU_ADDR, 0x02, (MAG_ADDR << 1) | 1)  # SLV0_ADD: 0x61 (read)
        write_reg(IMU_ADDR, 0x03, 0x00)   # SLV0_SUBADD: start at Xout0
        write_reg(IMU_ADDR, 0x04, 0x06)   # SLAVE0_CONFIG: read 6 bytes
        write_reg(IMU_ADDR, 0x01, 0x00)   # main bank
        write_reg(IMU_ADDR, 0x19, 0x3C)   # CTRL10_C: embedded functions on
        write_reg(IMU_ADDR, 0x1A, 0x09)   # MASTER_CONFIG: master on + internal pull-ups
    elif s.imu_type == IMU_DSV32X:
        write_reg(IMU_ADDR, 0x03, read_reg(IMU_ADDR, 0x03) | 0x40)   # IF_CFG: hub pull-ups on
        write_reg(IMU_ADDR, 0x01, 0x40)   # sensor hub bank
        write_reg(IMU_ADDR, 0x14, 0x00)   # MASTER_CONFIG: pass-through off
        time.sleep_ms(5)
        write_reg(IMU_ADDR, 0x15, (MAG_ADDR << 1) | 1)  # SLV0_ADD: 0x61 (read)
        write_reg(IMU_ADDR, 0x16, 0x00)   # SLV0_SUBADD: start at Xout0
        write_reg(IMU_ADDR, 0x17, 0x86)   # SLV0_CONFIG: 120 Hz, read 6 bytes
        write_reg(IMU_ADDR, 0x14, 0x04)   # MASTER_CONFIG: master on, one sensor
        write_reg(IMU_ADDR, 0x01, 0x00)   # main bank
    time.sleep_ms(50)                     # give the hub a few cycles to fill its registers
    s.hub_mode = True
    s.mag_fail_count = 0
    s.mag_same_count = 0

def print_lock_message():
    msg("!! MAGNETOMETER LOCKED UP !!")
    msg("   Disconnect the battery AND the USB cable for 10 seconds, then reconnect.")
    msg("   (Press p to try a software recovery first.)")
    s.last_lock_msg = time.ticks_ms()

def declare_mag_locked(reason):
    s.mag_locked = True
    msg("Magnetometer problem: " + reason)
    print_lock_message()

def try_recover_mag():
    msg("Trying to recover the magnetometer...")
    set_pass_through()
    time.sleep_ms(50)
    if init_mag():
        s.mag_ok = True
        msg("Magnetometer recovered - mode: PASS-THROUGH")
        return True
    print_lock_message()
    return False

def toggle_mag_path():
    if s.mag_locked:
        try_recover_mag()
        return
    if s.imu_type == IMU_NONE or not s.mag_ok:
        msg("Magnetometer not available - cannot switch mode")
        return
    if s.hub_mode:
        set_pass_through()
        msg("Mag mode: PASS-THROUGH (ESP32 reads 0x30 directly)" if mag_answers()
            else "Mag mode: PASS-THROUGH - mag not answering, try a power cycle")
    else:
        set_hub()
        msg("Mag mode: SENSOR HUB (IMU reads the mag every cycle)")

def read_mag_raw():
    if not s.hub_mode:
        return read_regs(MAG_ADDR, 0x00, 6)
    if s.imu_type == IMU_DS3TRC:
        return read_regs(IMU_ADDR, 0x2E, 6)           # SENSORHUB1..6_REG
    # LSM6DSV32X: SENSOR_HUB_1..6 live in the sensor hub bank
    write_reg(IMU_ADDR, 0x01, 0x40)
    b = read_regs(IMU_ADDR, 0x02, 6)
    write_reg(IMU_ADDR, 0x01, 0x00)
    return b

def read_mag():
    if not s.mag_ok or s.mag_locked:
        return
    b = read_mag_raw()

    # 1. No reply on the bus (pass-through lock-up)
    if not b:
        s.mag_fail_count += 1
        if s.mag_fail_count >= MAG_FAIL_LIMIT:
            declare_mag_locked("no reply on I2C")
        return
    s.mag_fail_count = 0

    # 2. Frozen data: a live sensor always flickers by a few counts
    if b == s.mag_last_raw:
        s.mag_same_count += 1
        if s.mag_same_count >= MAG_FROZEN_LIMIT:
            all_zero = not any(b)
            declare_mag_locked("hub data all zero (check hub pull-ups)"
                               if s.hub_mode and all_zero else "readings frozen")
            return
    else:
        s.mag_same_count = 0
        s.mag_last_raw = bytes(b)

    # 16-bit, offset binary, 1024 counts per gauss, 1 gauss = 100 uT
    x, y, z = struct.unpack(">3H", b)
    s.mx = (x - 32768) / 1024.0 * 100.0
    s.my = (y - 32768) / 1024.0 * 100.0
    s.mz = (z - 32768) / 1024.0 * 100.0


# ---------------- Barometer (SPL07-006) ----------------
def init_baro():
    if not device_answers(BARO_ADDR):
        return False
    write_reg(BARO_ADDR, 0x0C, 0x09)                  # soft reset
    time.sleep_ms(40)
    start = time.ticks_ms()
    while (read_reg(BARO_ADDR, 0x08) & 0xC0) != 0xC0: # COEF_RDY + SENSOR_RDY
        if time.ticks_diff(time.ticks_ms(), start) > 500:
            return False
        time.sleep_ms(5)

    b = read_regs(BARO_ADDR, 0x10, 21)
    if not b:
        return False
    s.coeffs = (
        sign_extend((b[0] << 4) | (b[1] >> 4), 12),                  # c0
        sign_extend(((b[1] & 0x0F) << 8) | b[2], 12),                # c1
        sign_extend((b[3] << 12) | (b[4] << 4) | (b[5] >> 4), 20),   # c00
        sign_extend(((b[5] & 0x0F) << 16) | (b[6] << 8) | b[7], 20), # c10
        sign_extend((b[8] << 8) | b[9], 16),                         # c01
        sign_extend((b[10] << 8) | b[11], 16),                       # c11
        sign_extend((b[12] << 8) | b[13], 16),                       # c20
        sign_extend((b[14] << 8) | b[15], 16),                       # c21
        sign_extend((b[16] << 8) | b[17], 16),                       # c30
        sign_extend((b[18] << 4) | (b[19] >> 4), 12),                # c31
        sign_extend(((b[19] & 0x0F) << 8) | b[20], 12),              # c40
    )
    write_reg(BARO_ADDR, 0x06, 0x43)   # pressure: 16 Hz, 8x oversampling
    write_reg(BARO_ADDR, 0x07, 0xC0)   # temperature: 16 Hz, 1x
    write_reg(BARO_ADDR, 0x09, 0x00)   # no result shift needed at 8x
    write_reg(BARO_ADDR, 0x08, 0x07)   # continuous pressure + temperature
    time.sleep_ms(100)
    return True

def read_baro():
    if not s.baro_ok:
        return
    b = read_regs(BARO_ADDR, 0x00, 6)
    if not b:
        return
    c0, c1, c00, c10, c01, c11, c20, c21, c30, c31, c40 = s.coeffs
    pr = sign_extend((b[0] << 16) | (b[1] << 8) | b[2], 24)
    tr = sign_extend((b[3] << 16) | (b[4] << 8) | b[5], 24)
    ps = pr / BARO_KP
    ts = tr / BARO_KT
    s.pressure = (c00 + ps * (c10 + ps * (c20 + ps * (c30 + ps * c40)))
                  + ts * c01 + ts * ps * (c11 + ps * (c21 + ps * c31)))
    s.baro_temp = c0 * 0.5 + c1 * ts + BARO_TEMP_OFFSET

def height_metres():
    return 44330.0 * (1.0 - (s.pressure / s.pressure_base) ** 0.1903)


# ---------------- Zeroing ----------------
def zero_sensors():
    msg("Zeroing - keep the board still")
    n = 50
    sx = sy = sz = sp = 0.0
    s.g_bias = [0.0, 0.0, 0.0]
    for _ in range(n):
        read_imu()
        read_baro()
        sx += s.gx; sy += s.gy; sz += s.gz; sp += s.pressure
        time.sleep_ms(20)
    s.g_bias = [sx / n, sy / n, sz / n]
    if s.baro_ok:
        s.pressure_base = sp / n


# ---------------- LEDs ----------------
def apply_led_pattern():
    for i, led in enumerate(leds):
        on = (s.led_pattern == len(leds) + 1) or (s.led_pattern == i + 1)
        led.value(1 if on == LED_ACTIVE_HIGH else 0)
    if s.led_pattern == 0:
        msg("LED: all off")
    elif s.led_pattern == len(leds) + 1:
        msg("LED: all on")
    else:
        msg("LED: " + LED_NAMES[s.led_pattern - 1])


# ---------------- Commands ----------------
def print_help():
    print("Keys: a accel, g gyro, m mag, h height, b pressure, t temp, "
          "p mag path, l LEDs, z zero, x text, ? help")
    print("Mag mode: " + ("SENSOR HUB" if s.hub_mode else "PASS-THROUGH"))
    imu = {IMU_DS3TRC: "LSM6DS3TR-C", IMU_DSV32X: "LSM6DSV32X"}.get(s.imu_type, "not found")
    mag = "LOCKED UP" if s.mag_locked else ("OK" if s.mag_ok else "not found")
    print("IMU: %s  Mag: %s  Baro: %s" % (imu, mag, "OK" if s.baro_ok else "not found"))

poller = select.poll()
poller.register(sys.stdin, select.POLLIN)

def handle_serial():
    while poller.poll(0):
        c = sys.stdin.read(1).lower()
        if c in "aghmbt" and c.strip():
            s.mode = c
            refresh_graph()              # same key again = manual refresh
        elif c == "l":
            s.led_pattern = (s.led_pattern + 1) % (len(leds) + 2)
            apply_led_pattern()
        elif c == "p":
            toggle_mag_path()
        elif c == "z":
            zero_sensors()
        elif c == "x":
            s.text_messages = not s.text_messages
            if s.text_messages:
                print("Text messages ON")
        elif c == "?":
            print_help()                 # help always prints, even with text off
        # Enter, spaces and unknown keys are ignored


# ---------------- Plot output ----------------
# Every mode sends exactly 3 columns so the traces keep their colours.
def build_line():
    m = s.mode
    if m == "a":
        return "ax_g:%.3f,ay_g:%.3f,az_g:%.3f" % (s.ax, s.ay, s.az)
    if m == "g":
        return "gx_dps:%.3f,gy_dps:%.3f,gz_dps:%.3f" % (s.gx, s.gy, s.gz)
    if m == "m":
        if s.mag_locked:   # the labels show in the plotter legend
            return "MAG_LOCKED:0,unplug_battery_and_USB:0,wait_10s_then_reconnect:0"
        return "mx_uT:%.3f,my_uT:%.3f,mz_uT:%.3f" % (s.mx, s.my, s.mz)
    if m == "h":
        # Height last, so it is drawn on top of the 0 m ground line
        return "ground:0,ground:0,height_m:%.3f" % height_metres()
    if m == "b":
        zero = s.pressure_base / 100.0
        return "zero_hPa:%.2f,zero_hPa:%.2f,pressure_hPa:%.2f" % (zero, zero, s.pressure / 100.0)
    # "t": third column is the average, so it sits between the two
    return "imu_C:%.3f,baro_C:%.3f,avg_C:%.3f" % (
        s.imu_temp, s.baro_temp, (s.imu_temp + s.baro_temp) / 2.0)

def read_all():
    read_imu()
    read_mag()
    read_baro()

def refresh_graph():
    read_all()
    line = build_line() + "\n"
    for _ in range(FLUSH_LINES):
        sys.stdout.write(line)


# ---------------- Start-up ----------------
for led in leds:
    led.value(0 if LED_ACTIVE_HIGH else 1)

i2c_start()
init_imu()
s.mag_ok = init_mag()      # needs pass-through, so after the IMU
s.baro_ok = init_baro()

print_help()
if s.mag_locked:
    print_lock_message()
zero_sensors()

# ---------------- Main loop ----------------
last_plot = time.ticks_ms()
while True:
    handle_serial()

    now = time.ticks_ms()
    if s.mag_locked and time.ticks_diff(now, s.last_lock_msg) >= LOCK_MSG_INTERVAL_MS:
        print_lock_message()

    if time.ticks_diff(now, last_plot) >= PLOT_INTERVAL_MS:
        last_plot = now
        read_all()
        print(build_line())
