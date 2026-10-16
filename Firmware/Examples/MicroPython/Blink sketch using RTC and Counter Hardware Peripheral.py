import esp32
import time
from machine import Pin

# =========================================================================
# HYPER S3 ROCKET - Production Pin Map
# =========================================================================
user_led = Pin(48, Pin.OUT)  # Blue Status LED (Active Default)
# user_led = Pin(45, Pin.OUT)  # Green LED
# user_led = Pin(46, Pin.OUT)  # Red LED (Backside Placement)
# user_led = Pin(21, Pin.OUT)  # Alternative Red LED

sitime_clock = Pin(15, Pin.IN)  # SiTime 32,768 Hz clock input

# =========================================================================
# Native ESP32 Pulse Counter (PCNT)
# =========================================================================
# 32,768 Hz / 2 = 16,384 pulses = exactly 0.5 seconds.
#
# The counter counts rising edges on GPIO 15 by itself. When it reaches
# max it goes back to 0 in hardware, in the same instant, and raises an
# event. Because the software never resets the counter, no clock pulses
# are ever lost - the blink stays locked to the SiTime crystal, which
# makes this sketch usable for a long-term clock drift test.

TARGET_PULSES = 16384

counter = esp32.PCNT(0, pin=sitime_clock, rising=esp32.PCNT.INCREMENT,
                     max=TARGET_PULSES)

half_seconds = 0

def on_half_second(pcnt):
    global half_seconds
    user_led.toggle()            # flip the LED every 0.5 s
    half_seconds += 1
    if half_seconds % 2 == 0:    # every second LED change = 1 s
        print("Mission Time:", half_seconds // 2, "s")

counter.irq(on_half_second, esp32.PCNT.IRQ_MAX)
counter.start()

print("HYPER S3 ROCKET: PCNT hardware engine locked onto GPIO 15 clock source.")

# Nothing to do here - the hardware counter runs the timing.
while True:
    time.sleep(1)
