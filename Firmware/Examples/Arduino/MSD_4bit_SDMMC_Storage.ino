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
  uint32_t secSize = SD_MMC.sectorSize();
  if (secSize == 0 || secSize > 512) return -1;
  for (uint32_t x = 0; x < bufsize / secSize; x++) {
    uint8_t blk[512] __attribute__((aligned(4)));          // aligned copy for the SD driver
    memcpy(blk, buffer + secSize * x, secSize);
    if (!SD_MMC.writeRAW(blk, lba + x)) return -1;
  }
  return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
  uint32_t secSize = SD_MMC.sectorSize();
  if (secSize == 0) return -1;
  for (uint32_t x = 0; x < bufsize / secSize; x++) {
    if (!SD_MMC.readRAW((uint8_t*)buffer + secSize * x, lba + x)) return -1;
  }
  return bufsize;
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
