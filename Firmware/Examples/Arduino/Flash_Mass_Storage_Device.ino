#include "USBMSC.h"
#include "SD_MMC.h"

// Access the low-level hardware structures cleanly
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"

USBMSC msc;

#define SD_CLK 36
#define SD_CMD 35
#define SD_D0  37
#define SD_D1  38
#define SD_D2  33
#define SD_D3  34

// CORRECT EXPLICIT STRUCT LAYOUT FOR ESP32 CORE 3.X
// This matches the exact underlying memory footprints inside 'SD_MMC.cpp'
struct SDMMCFS_Core3_Layout {
    void*     _impl;      // Base class implementations
    void*     _vfs;       // Pointer to VFS allocation structure 
    sdmmc_card_t* _card;  // THIS IS THE EXACT DIRECT CARD POINTER
};

static sdmmc_card_t* get_valid_card_pointer() {
    // Safely cast to extract the actual _card property address out of Core 3.x
    SDMMCFS_Core3_Layout* layout = (SDMMCFS_Core3_Layout*)&SD_MMC;
    return layout->_card;
}

// FIXED HIGH-SPEED WRITE CALLBACK
static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
  uint32_t sector_size = SD_MMC.sectorSize();
  uint32_t sector_count = bufsize / sector_size;

  sdmmc_card_t* native_card = get_valid_card_pointer();
  if (!native_card) return -1;

  // Triggers the direct native hardware block DMA stream
  esp_err_t err = sdmmc_write_sectors(native_card, buffer, lba, sector_count);
  
  return (err == ESP_OK) ? bufsize : -1;
}

// FIXED HIGH-SPEED READ CALLBACK
static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
  uint32_t sector_size = SD_MMC.sectorSize();
  uint32_t sector_count = bufsize / sector_size;

  sdmmc_card_t* native_card = get_valid_card_pointer();
  if (!native_card) return -1;

  // Triggers the direct native hardware block DMA stream
  esp_err_t err = sdmmc_read_sectors(native_card, (uint8_t*)buffer, lba, sector_count);

  return (err == ESP_OK) ? bufsize : -1;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject) { 
  return true; 
}

void setup() {
  if (!SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3)) {
    return;
  }

  // Force the 4-bit bus interface directly at 40MHz High-Speed execution
  if (SD_MMC.begin("/sdcard", false, false, 40000)) { 
    msc.vendorID("ESP32S3");
    msc.productID("SD_Card_4Bit");
    msc.onRead(onRead);
    msc.onWrite(onWrite);
    msc.onStartStop(onStartStop);
    
    msc.mediaPresent(true);
    msc.isWritable(true);
    
    msc.begin(SD_MMC.numSectors(), SD_MMC.sectorSize());
  }
}

void loop() {
  // Driven automatically on background core peripheral hardware interrupts
}
