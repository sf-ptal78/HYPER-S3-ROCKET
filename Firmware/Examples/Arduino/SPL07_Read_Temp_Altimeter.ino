#include <Wire.h>
#include <SPL07-003.h> // Library by Kenneract / Seeed Studio

// Create the sensor instance globally
SPL07_003 spl;

// Flag to track if the sensor actually connected
bool sensorOnline = false;

// Non-blocking timing variables for 10Hz (100ms interval)
unsigned long lastLogTime = 0;
const unsigned long logInterval = 100; // 100 milliseconds = 10Hz

// LOCAL ENVIRONMENT CONSTRAINTS
const float SEA_LEVEL_PRESSURE_HPA = 1020.0; // Adjust to your city's local pressure

void setup() {
  Serial.begin(115200);

  // Wait up to 3 seconds for ESP32-S3 Hardware CDC Serial Monitor to open
  unsigned long startWait = millis();
  while (!Serial && (millis() - startWait < 3000)) {
    delay(10);
  }
  delay(500); 

  Serial.println("\n=============================================");
  Serial.println("  SPL07-003 16Hz INT SAMPLING / 10Hz MONITOR ");
  Serial.println("=============================================");

  // 1. Lock onto hardware pins (GPIO 18 = SDA, GPIO 17 = SCL)
  Wire.setPins(18, 17); 

  // 2. Initialize the main system I2C bus
  Wire.begin(); 

  // 3. TARGET ADDRESS ENFORCED: Initialize exclusively on 0x76 
  if (spl.begin(0x76, &Wire) == true) {
    Serial.println("🎉 SUCCESS: SPL07-003 connected at address 0x76!");
    sensorOnline = true;
    
    // Configured back to 16Hz internal sampling rates
    spl.setPressureConfig(SPL07_16HZ, SPL07_16SAMPLES); 
    spl.setTemperatureConfig(SPL07_16HZ, SPL07_1SAMPLE);
    
    // Put sensor into continuous background tracking
    spl.setMode(SPL07_CONT_PRES_TEMP);
    
    // HARDWARE FACTORY OFFSET CORRECTION
    // If your room is 23C but the uncalibrated chip reports 31.5C, 
    // this linear correction brings the telemetry exactly back to reality.
    spl.setTemperatureOffset(-8.5); 
    
    Serial.println("Sensor background streaming and calibration active.");
  } 
  else {
    Serial.println("❌ ERROR: SPL07-003 NOT responding at address 0x76!");
  }
  
  Serial.println("---------------------------------------------");
  Serial.println("Timestamp(ms), Pressure(Pa), Temp(C), Altitude(m)");
}

void loop() {
  unsigned long currentTime = millis();

  // Enforce the precise 10Hz data delivery window (100ms)
  if (currentTime - lastLogTime >= logInterval) {
    lastLogTime = currentTime;

    if (sensorOnline) {
      // Pull only when data thresholds are updated in hardware
      if (spl.pressureAvailable() || spl.temperatureAvailable()) {
        
        double pressurePa = spl.readPressure();
        double temperatureC = spl.readTemperature();
        
        // Calculate true altitude based off adjusted real-time pressure
        double pressureHpa = pressurePa / 100.0;
        double altitudeMeters = 44330.0 * (1.0 - pow((pressureHpa / SEA_LEVEL_PRESSURE_HPA), 0.1903));

        // Output clean metrics
        Serial.print(currentTime);
        Serial.print(", ");
        Serial.print(pressurePa, 2); 
        Serial.print(" Pa, ");
        Serial.print(temperatureC, 2);
        Serial.print(" C, ");
        Serial.print(altitudeMeters, 2);
        Serial.println(" m");
      }
    }
  }
}
