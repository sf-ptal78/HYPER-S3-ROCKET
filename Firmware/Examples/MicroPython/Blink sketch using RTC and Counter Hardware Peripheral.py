import esp32
from machine import Pin

# =========================================================================
# HYPER S3 ROCKET - Production Pin Map
# =========================================================================
user_led = Pin(48, Pin.OUT)  # Blue Status LED (Active Default)
# user_led = Pin(45, Pin.OUT)  # Green LED
# user_led = Pin(46, Pin.OUT)  # Red LED (Backside Placement)
# user_led = Pin(21, Pin.OUT)  # Alternative Red LED

sitime_clock = Pin(15, Pin.IN) # SiTime Hardware input trace

# =========================================================================
# Native ESP32 Pulse Counter (PCNT) Initialisation
# =========================================================================
# Binds PCNT unit 0 straight to GPIO 15 to capture the rising edges.
# The internal silicon register handles the increments automatically in the background.
counter = esp32.PCNT(0, pin=sitime_clock, rising=esp32.PCNT.INCREMENT)
counter.start()

print("HYPER S3 ROCKET: PCNT hardware engine locked onto GPIO 15 clock source.")

# =========================================================================
# Flight Execution Loop (Solely reliant on SiTime clock ticks)
# =========================================================================
# No software timers or time.sleep() delays are used.
# 32,768 Hz / 2 = 16,384 pulses needed to create a clean 0.5-second state transition.

TARGET_PULSES = 16384

while True:
    # Read the raw hardware tracking register directly from the silicon
    if counter.value() >= TARGET_PULSES:
        user_led.toggle()   # Instantly flip the physical LED state
        counter.value(0)    # Instantly clear and reset the hardware counter to zero
