#include "FS.h"
#include "SD_MMC.h"

// Corrected native hardware pin layout
#define SD_CLK  36
#define SD_CMD  35
#define SD_D0   37
#define SD_D1   38
#define SD_D2   33
#define SD_D3   34

// Status / Activity LED pin
#define RED_LED_PIN 46

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  delay(2000); 

  Serial.println("\n--- SD Card File Explorer Setup ---");

  // Initialize LED and ensure it starts OFF (Safe to remove)
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW); 

  if (!SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3)) {
    Serial.println("Pin configuration FAILED!");
    return;
  }

  if (!SD_MMC.begin("/sdcard", false, false, 40000)) {
    Serial.println("Card Mount FAILED!");
    return;
  }
  Serial.println("Card Mount SUCCESS!");

  // --- SHOW FOLDERS AND FILES ---
  // We pass the file system (SD_MMC), the starting directory ("/" for root), and folder depth levels (0)
  listDir(SD_MMC, "/", 0);

  Serial.println("\nDirectory read complete. LED is OFF. Safe to remove card.");
}

void loop() {
  // Empty loop
}

// --- FUNCTION TO DISPLAY ALL FOLDERS AND FILES ---
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  // 1. Turn ON activity LED (Busy)
  digitalWrite(RED_LED_PIN, HIGH); 
  Serial.printf("\nListing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    digitalWrite(RED_LED_PIN, LOW); // Turn OFF LED before exiting
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    root.close();
    digitalWrite(RED_LED_PIN, LOW); // Turn OFF LED before exiting
    return;
  }

  File file = root.openNextFile();
  if (!file) {
    Serial.println(" (Directory is empty)");
  }

  while (file) {
    // Print indentation for nested folders to make it easier to read
    for (uint8_t i = 0; i < levels; i++) {
      Serial.print("  ");
    }

    if (file.isDirectory()) {
      Serial.printf("[DIR]  %s\n", file.name());
      // If levels is greater than 0, recursively explore subfolders
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.printf("FILE:  %s  (Size: %d bytes)\n", file.name(), file.size());
    }
    
    file.close();
    file = root.openNextFile();
  }
  
  root.close();

  // 2. Turn OFF activity LED (Safe to remove)
  digitalWrite(RED_LED_PIN, LOW); 
}
