// Flash Mass Storage Device
// 1. In the Arduino IDE, go to Tools -> Partition Scheme.
// 2. Change it to Default 4MB with ffat (1.2MB APP/1.5MB FATFS)
// 3. USB Mode is USB OTG (TinyUSB)
// 4. USB CDC on Boot is Enabled
// 5. Before loading firmware do the Boot Mode Dance - Toggle Boot on, Reset On, Reset Off, Boot Off


#include "USBMSC.h"
#include "esp_partition.h"
#include "wear_levelling.h"

USBMSC msc;
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
  return (wl_write(s_wl_handle, (lba * 512) + offset, buffer, bufsize) == ESP_OK) ? bufsize : -1;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
  return (wl_read(s_wl_handle, (lba * 512) + offset, buffer, bufsize) == ESP_OK) ? bufsize : -1;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject) { return true; }

void setup() {
  const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, "ffat");
  
  if (partition && wl_mount(partition, &s_wl_handle) == ESP_OK) {
    msc.vendorID("ESP32S3");
    msc.productID("Flash_Drive");
    msc.onRead(onRead);
    msc.onWrite(onWrite);
    msc.onStartStop(onStartStop);
    msc.mediaPresent(true);
    msc.isWritable(true);
    
    msc.begin(wl_size(s_wl_handle) / wl_sector_size(s_wl_handle), wl_sector_size(s_wl_handle));
  }
}

void loop() {
  // Nothing needed here. The USB stack runs entirely on background interrupts!
}
