# =========================================================================
# HYPER S3 ROCKET - Interrupt Dual LED Toggle (no loop at all)
# STEMFORGE
#
# Press the BOOT button to swap the red and green LEDs. There is no
# while loop: the button's falling edge triggers an interrupt, and a small
# handler swaps the LEDs. The script finishes, but the interrupt keeps
# working in the background - you can even type in the REPL while it runs.
#
# Note: pressing Stop in Thonny (or Ctrl-D) resets MicroPython and
# removes the interrupt. Run the script again to bring it back.
# =========================================================================

import time
from machine import Pin

DEBOUNCE_MS = 200                         # mechanical switch debounce

button    = Pin(0, Pin.IN, Pin.PULL_UP)   # Physical BOOT button
red_led   = Pin(21, Pin.OUT, value=1)     # 🔴 Top red LED   - starts ON
green_led = Pin(45, Pin.OUT, value=0)     # 🟢 Top green LED - starts OFF

green_active = False                      # False = red on, True = green on
last_press = time.ticks_ms()

def button_isr(pin):
    global green_active, last_press
    now = time.ticks_ms()
    if time.ticks_diff(now, last_press) > DEBOUNCE_MS:
        green_active = not green_active
        red_led.value(0 if green_active else 1)
        green_led.value(1 if green_active else 0)
        last_press = now

# Interrupt on the press (HIGH -> LOW)
button.irq(trigger=Pin.IRQ_FALLING, handler=button_isr)

print("\n--- HYPER S3 ROCKET Dual LED Engine Online ---")
print("No loop is running. An interrupt handles every press.")
