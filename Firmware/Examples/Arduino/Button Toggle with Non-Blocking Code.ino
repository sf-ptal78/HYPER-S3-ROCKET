// 1. Define the physical hardware pin mappings
const int BUTTON_PIN = 0;   // The physical BOOT button on the ESP32-S3
const int LED_RED = 21;    // 🔴 TOP RED Status/Fault Light
const int LED_GREEN = 45;  // 🟢 TOP GREEN Charge Done Light

// 2. State tracking variables
String currentState = "GREEN_ALIVE"; // Tracks which LED is currently on
int lastButtonState = HIGH;          // HIGH means unpressed (due to internal Pull-Up)

// Time tracking variables
unsigned long lastDebounceTime = 0;  // Stores the last time the button pin twitched
const unsigned long debounceDelay = 50; // ⏱️ 50 milliseconds safety window

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  // Configure physical pin behaviors
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Requires internal Pull-Up resistor
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  // Set the starting condition
  digitalWrite(LED_GREEN, HIGH);  // Start with Green ON
  digitalWrite(LED_RED, LOW);     // Start with Red OFF
  
  Serial.println("🚀 HYPER S3 ROCKET: Button Toggle Demo Online!");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // Read the instantaneous physical status of the button right now
  int reading = digitalRead(BUTTON_PIN);

  // Check if the button reading just changed (due to noise or an actual press)
  if (reading != lastButtonState) {
    // Reset the countdown timer to right now
    lastDebounceTime = millis();
    // Save the reading for the next loop check
    lastButtonState = reading;
  }

  // If the reading has stayed steady for longer than our safety delay,
  // we know it is a real press, not a background hardware twitch!
  if ((millis() - lastDebounceTime) > debounceDelay) {
    
    // In INPUT_PULLUP mode, 'LOW' means the button is physically pushed down
    if (reading == LOW) {
      
      // Trigger the toggle based on the active tracking state
      if (currentState == "GREEN_ALIVE") {
        Serial.println("🔴 Button Pushed! Swapping to RED alert.");
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, HIGH);
        currentState = "RED_ALIVE";
        
        // Small pause to prevent rapid cycling while holding the button down
        while (digitalRead(BUTTON_PIN) == LOW) {
          // Do nothing, wait for user to release finger
        }
      } 
      else if (currentState == "RED_ALIVE") {
        Serial.println("🟢 Button Pushed! Swapping to GREEN safe.");
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, HIGH);
        currentState = "GREEN_ALIVE";
        
        // Small pause to prevent rapid cycling while holding the button down
        while (digitalRead(BUTTON_PIN) == LOW) {
          // Do nothing, wait for user to release finger
        }
      }
    }
  }

  // 🌟 BECAUSE THIS IS NON-BLOCKING, YOU CAN WRITE OTHER LIVE
  // FLIGHT LOOPS OR TELEMETRY PRINTS RIGHT HERE WITHOUT DELAYS!
}
