import time
from machine import Pin

print("=========================================================")
print(" 🎛️ BUTTON TOGGLE DEMO: NON-BLOCKING DEBOUNCE ACTIVE")
print(" Press the BOOT button (GPIO 0) to swap lights!")
print("=========================================================")

# 1. Setup the physical button and LEDs
# GPIO 0 is the physical BOOT button on the board (requires PULL_UP)
button = Pin(0, Pin.IN, Pin.PULL_UP)  
led_red = Pin(21, Pin.OUT)            # 🔴 TOP RED Status/Fault Light
led_green = Pin(45, Pin.OUT)          # 🟢 TOP GREEN Charge Done Light

# 2. Set the starting condition
led_green.value(1)                    # Start with Green ON
led_red.value(0)                      # Start with Red OFF

# 3. State tracking variables
current_state = "GREEN_ALIVE"         # Tracks which LED is currently on
last_button_value = 1                 # 1 means unpressed (due to Pull Up)
last_debounce_time = 0                # Stores the timestamp of the last twitch
debounce_delay = 50                   # ⏱️ 50 milliseconds safety window

while True:
    # Read the instantaneous physical status of the button right now
    button_now = button.value()
    
    # Check if the button reading just changed (due to noise or a real press)
    if button_now != last_button_value:
        # Reset the countdown timer to right now
        last_debounce_time = time.ticks_ms()
        # Save the reading for the next loop check
        last_button_value = button_now

    # If the reading has stayed steady for longer than our safety delay, 
    # we know it is a real press, not a background hardware twitch!
    if (time.ticks_ms() - last_debounce_time) > debounce_delay:
        
        # In Pull_Up mode, '0' means the button is physically pushed down
        if button_now == 0:
            
            # Only trigger the toggle if we haven't processed this press yet
            if current_state == "GREEN_ALIVE":
                print("🔴 Button Pushed! Swapping to RED alert.")
                led_green.value(0)
                led_red.value(1)
                current_state = "RED_ALIVE"
                
                # Small pause to prevent rapid cycling while holding the button down
                while button.value() == 0:
                    pass
                    
            elif current_state == "RED_ALIVE":
                print("🟢 Button Pushed! Swapping to GREEN safe.")
                led_red.value(0)
                led_green.value(1)
                current_state = "GREEN_ALIVE"
                
                # Small pause to prevent rapid cycling while holding the button down
                while button.value() == 0:
                    pass

    # 🌟 BECAUSE THIS IS NON-BLOCKING, YOU CAN WRITE OTHER LIVE
    # FLIGHT LOOPS OR TELEMETRY PRINTS RIGHT HERE WITHOUT DELAYS!

