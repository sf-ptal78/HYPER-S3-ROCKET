import time
from machine import Pin

# 1. Say hello in the computer terminal window!
print("🚀 HELLO WORLD! THE HYPER S3 ROCKET IS READY FOR LAUNCH! 🌟")

# 2. Pick your favorite light on the rocket circuit board!
# (To use a different light, just remove the '#' symbol from the start of its line)

rocket_light = Pin(48, Pin.OUT)  # 🔵 The Pretty BLUE User Light (Active right now!)
# rocket_light = Pin(21, Pin.OUT)  # 🔴 The TOP RED Status/Fault Light
# rocket_light = Pin(45, Pin.OUT)  # 🟢 The TOP GREEN Charge Done Light
# rocket_light = Pin(46, Pin.OUT)  # 🔴 The BOTTOM RED SD Card Light

# 3. Blink the light like a flashing star!
while True:
    rocket_light.value(1)  # 🌟 Turn your light ON!
    time.sleep(0.5)        # ⏱️ Wait half a second
    
    rocket_light.value(0)  # 🌑 Turn your light OFF!
    time.sleep(0.5)        # ⏱️ Wait half a second
