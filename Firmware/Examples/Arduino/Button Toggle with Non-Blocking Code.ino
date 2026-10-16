// =========================================================================
// HYPER S3 ROCKET - Button Toggle with Non-Blocking Code
// STEMFORGE
//
// Press the BOOT button (GPIO 0) to swap between the red and green LEDs.
// The blue LED blinks the whole time. It keeps blinking even while you
// hold the button down - that is the proof the code never stops and waits.
// =========================================================================

// 1. Physical pin mappings
const int BUTTON_PIN = 0;    // BOOT button
const int LED_RED    = 21;   // 🔴 TOP RED Status/Fault Light
const int LED_GREEN  = 45;   // 🟢 TOP GREEN Charge Done Light
const int LED_BLUE   = 48;   // 🔵 TOP BLUE User Light (heartbeat)

// 2. State tracking
bool redActive = false;                 // false = green on, true = red on

// Button debounce
int  lastReading  = HIGH;               // HIGH = not pressed (internal pull-up)
int  stableState  = HIGH;               // the debounced button state
unsigned long lastChangeTime = 0;
const unsigned long debounceDelay = 50; // ⏱️ 50 ms safety window

// Heartbeat
unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 250; // blue LED toggles every 250 ms
bool blueOn = false;

void showLeds() {
  digitalWrite(LED_RED,   redActive ? HIGH : LOW);
  digitalWrite(LED_GREEN, redActive ? LOW  : HIGH);
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  showLeds();                           // start with green on
  Serial.println("🚀 HYPER S3 ROCKET: Button Toggle Demo Online!");
}

void loop() {
  unsigned long now = millis();

  // ---- Button: read it, never wait for it ----
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastReading) {         // pin changed (noise or a real press)
    lastChangeTime = now;               // restart the debounce timer
    lastReading = reading;
  }

  // Steady for longer than the debounce window and different from before?
  if ((now - lastChangeTime) > debounceDelay && reading != stableState) {
    stableState = reading;

    if (stableState == LOW) {           // act once, on the press only
      redActive = !redActive;
      showLeds();
      Serial.println(redActive ? "🔴 Button Pushed! Swapping to RED alert."
                               : "🟢 Button Pushed! Swapping to GREEN safe.");
    }
    // Releasing the button changes stableState back to HIGH and does nothing else.
  }

  // ---- Heartbeat: keeps running while the button is held ----
  if (now - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = now;
    blueOn = !blueOn;
    digitalWrite(LED_BLUE, blueOn ? HIGH : LOW);
  }

  // 🌟 Add your own flight loops or telemetry here - nothing above ever waits.
}
