// =========================================================================
// HYPER S3 ROCKET - Hardware Interrupt Dual LED Toggle (empty loop)
// STEMFORGE
//
// Press the BOOT button to swap the red and green LEDs. loop() is empty:
// the button's falling edge triggers a hardware interrupt, and a tiny
// interrupt routine (a few microseconds per press) swaps the LEDs.
// Between presses the CPU does nothing for this demo at all.
//
// Works on battery too: the sketch no longer waits for the Serial Monitor.
// =========================================================================

#include "hal/gpio_ll.h"   // direct GPIO register writes (safe inside an ISR)
#include "esp_timer.h"     // microsecond timer (safe inside an ISR)

#define USER_BUTTON_PIN   0   // Physical BOOT button
#define RED_LED_PIN      21   // 🔴 Top red LED
#define GREEN_LED_PIN    45   // 🟢 Top green LED

#define DEBOUNCE_US  200000   // 200 ms mechanical switch debounce

// Shared between the interrupt and the rest of the program
volatile bool    greenActive = false;  // false = red on, true = green on
volatile int64_t lastPressUs = 0;

// =========================================================================
// Hardware Interrupt Service Routine (ISR)
// Everything used here runs from RAM, so it is safe even while the flash
// is busy (for example during a Wi-Fi or file write).
// =========================================================================
void IRAM_ATTR button_isr() {
  int64_t now = esp_timer_get_time();

  if (now - lastPressUs > DEBOUNCE_US) {
    greenActive = !greenActive;
    gpio_ll_set_level(&GPIO, RED_LED_PIN,   greenActive ? 0 : 1);
    gpio_ll_set_level(&GPIO, GREEN_LED_PIN, greenActive ? 1 : 0);
    lastPressUs = now;
  }
}

void setup() {
  Serial.begin(115200);

  // Wait up to 3 seconds for the Serial Monitor, then carry on without it
  unsigned long startWait = millis();
  while (!Serial && (millis() - startWait < 3000)) { delay(10); }

  // LED outputs, starting state: red on, green off
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, LOW);

  // BOOT button with pull-up, interrupt on the press (HIGH -> LOW)
  pinMode(USER_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(USER_BUTTON_PIN), button_isr, FALLING);

  Serial.println("\n--- HYPER S3 ROCKET Dual LED Engine Online ---");
  Serial.println("loop() is empty. A hardware interrupt handles every press.");
  Serial.println("Always close this Serial Monitor window BEFORE uploading next time!\n");
}

void loop() {
  // Left completely blank on purpose.
}
