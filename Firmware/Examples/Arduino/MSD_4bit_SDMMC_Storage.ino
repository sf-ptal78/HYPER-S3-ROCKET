// Storage device using the USB-C to the MicroSD Card using Espressif's 4bit SDMMC protocol for fast data transfeer (compared with 1bit or SPI mode)


#include "USBMSC.h"
#include "SD_MMC.h"

USBMSC msc;

// Target hardware pins pulled from your HYPER-S3-ROCKET repository
#define SD_CLK 36
#define SD_CMD 35
#define SD_D0  37
#define SD_D1  38
#define SD_D2  33
#define SD_D3  34

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
  return SD_MMC.writeRAW(buffer, lba) ? bufsize : -1;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
  return SD_MMC.readRAW((uint8_t*)buffer, lba) ? bufsize : -1;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject) { 
  return true; 
}

void setup() {
  // 1. Assign your explicit 4-bit pin configuration array layout
  if (!SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3)) {
    return;
  }

  // 2. Initialize the card matching your exact repository arguments:
  //    (mount_point, mode_1bit=false, format_if_mount_failed=false, speed_khz=40000)
  if (SD_MMC.begin("/sdcard", false, false, 40000)) { 
    
    msc.vendorID("ESP32S3");
    msc.productID("SD_Card_4Bit");
    msc.onRead(onRead);
    msc.onWrite(onWrite);
    msc.onStartStop(onStartStop);
    
    msc.mediaPresent(true);
    msc.isWritable(true);
    
    // Pass the actual storage cluster layout to the Windows driver stack
    msc.begin(SD_MMC.numSectors(), SD_MMC.sectorSize());
  }
}

void loop() {
  // Driven automatically on background core peripheral hardware interrupts
}
