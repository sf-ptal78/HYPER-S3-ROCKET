# HYPER S3 ROCKET - microSD card file explorer, 4-bit SDMMC (MicroPython)
# STEMFORGE
#
# Mounts the card in fast 4-bit SD mode, lists the root folder, then
# unmounts it so the card is safe to remove.

import os
import machine
from machine import Pin

# Native hardware pin layout
SD_CLK = 36
SD_CMD = 35
SD_D0  = 37
SD_D1  = 38
SD_D2  = 33
SD_D3  = 34

# Status / activity LED (bottom red)
red_led = Pin(46, Pin.OUT, value=0)

MOUNT_POINT = "/sd"
LEVELS = 0                     # folder depth to explore (0 = root only)

def list_dir(path, levels, indent=0):
    red_led.value(1)           # busy
    print("\nListing directory: %s" % path)
    try:
        entries = list(os.ilistdir(path))
    except OSError:
        print("Failed to open directory")
        red_led.value(0)
        return

    if not entries:
        print(" (Directory is empty)")

    for entry in entries:
        name, kind = entry[0], entry[1]
        full = path.rstrip("/") + "/" + name
        if kind == 0x4000:     # folder
            print("  " * indent + "[DIR]  " + name)
            if levels:
                list_dir(full, levels - 1, indent + 1)
                red_led.value(1)
        else:
            size = entry[3] if len(entry) > 3 else os.stat(full)[6]
            print("  " * indent + "FILE:  %s  (Size: %d bytes)" % (name, size))

    red_led.value(0)           # safe to remove

print("\n--- SD Card File Explorer Setup ---")

try:
    sd = machine.SDCard(slot=1, width=4, freq=40_000_000,
                        sck=SD_CLK, cmd=SD_CMD,
                        data=(SD_D0, SD_D1, SD_D2, SD_D3))
    os.mount(sd, MOUNT_POINT)
except OSError as e:
    print("Card Mount FAILED!", e)
else:
    print("Card Mount SUCCESS!")
    list_dir(MOUNT_POINT, LEVELS)
    os.umount(MOUNT_POINT)
    print("\nDirectory read complete. LED is OFF. Safe to remove card.")
