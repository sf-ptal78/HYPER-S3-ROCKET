import time
from machine import Pin

print("=========================================================")
print(" 🎛️ BUTTON TOGGLE DEMO: NON-BLOCKING DEBOUNCE ACTIVE")
print(" Press the BOOT button (GPIO 0) to swap lights!")
print(" The blue light keeps blinking even while you hold it.")
print("=========================================================")

# 1. Setup the physical button and LEDs
button    = Pin(0, Pin.IN, Pin.PULL_UP)   # BOOT button (1 = not pressed)
led_red   = Pin(21, Pin.OUT)              # 🔴 TOP RED Status/Fault Light
led_green = Pin(45, Pin.OUT)              # 🟢 TOP GREEN Charge Done Light
led_blue  = Pin(48, Pin.OUT)              # 🔵 TOP BLUE User Light (heartbeat)

# 2. State tracking
red_active = False                        # False = green on, True = red on

def show_leds():
    led_red.value(1 if red_active else 0)
    led_green.value(0 if red_active else 1)

show_leds()                               # start with green on

# Button debounce
last_reading = 1
stable_state = 1
last_change = time.ticks_ms()
DEBOUNCE_MS = 50                          # ⏱️ 50 ms safety window

# Heartbeat
last_blink = time.ticks_ms()
BLINK_MS = 250                            # blue light toggles every 250 ms

while True:
    now = time.ticks_ms()

    # ---- Button: read it, never wait for it ----
    reading = button.value()

    if reading != last_reading:           # pin changed (noise or a real press)
        last_change = now                 # restart the debounce timer
        last_reading = reading

    # Steady for longer than the debounce window and different from before?
    if time.ticks_diff(now, last_change) > DEBOUNCE_MS and reading != stable_state:
        stable_state = reading

        if stable_state == 0:             # act once, on the press only
            red_active = not red_active
            show_leds()
            if red_active:
                print("🔴 Button Pushed! Swapping to RED alert.")
            else:
                print("🟢 Button Pushed! Swapping to GREEN safe.")
        # Releasing the button sets stable_state back to 1 and does nothing else.

    # ---- Heartbeat: keeps running while the button is held ----
    if time.ticks_diff(now, last_blink) >= BLINK_MS:
        last_blink = now
        led_blue.value(not led_blue.value())

    # 🌟 Add your own flight loops or telemetry here - nothing above ever waits.
