// =========================================================================
// HYPER S3 ROCKET - Zero-CPU Hardware Interrupt Dual LED Toggle
// =========================================================================

#define USER_BUTTON_PIN   0   // Physical BOOT button

// Dual LED Pin Mappings
#define RED_LED_PIN      21   // Red LED
#define GREEN_LED_PIN    45   // Green LED

// Volatile variables for safe cross-domain execution
volatile bool toggle_state = false; // false = Red Active, true = Green Active
volatile unsigned long last_interrupt_time = 0;

// =========================================================================
// Hardware Interrupt Service Routine (ISR)
// =========================================================================
void IRAM_ATTR button_isr() {
    unsigned long interrupt_time = millis(); // Hardware debounce timer tick
    
    // 200ms mechanical switch debounce filter
    if (interrupt_time - last_interrupt_time > 200) {
        toggle_state = !toggle_state; // Swap state
        
        if (!toggle_state) {
            // State A: Red ON, Green OFF
            digitalWrite(RED_LED_PIN, HIGH);
            digitalWrite(GREEN_LED_PIN, LOW);
        } else {
            // State B: Red OFF, Green ON
            digitalWrite(RED_LED_PIN, LOW);
            digitalWrite(GREEN_LED_PIN, HIGH);
        }
        
        last_interrupt_time = interrupt_time;
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    // Initialize both target LED channels as output paths
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    
    // Set initial baseline state (Red ON, Green OFF)
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);

    // Set GPIO 0 input channel
    pinMode(USER_BUTTON_PIN, INPUT_PULLUP);

    // Bind the hardware interrupt matrix using the proper camelCase API macro
    attachInterrupt(digitalPinToInterrupt(USER_BUTTON_PIN), button_isr, FALLING);

    Serial.println("\n--- HYPER S3 ROCKET Dual LED Engine Online ---");
    Serial.println("The loop() is completely empty. Hardware gates are handling the toggle.");
    Serial.println("Always close this Serial Monitor window BEFORE uploading next time!\n");
}

void loop() {
    // Left completely blank. 
    // The CPU is utilizing 0% resources to monitor button transitions.
}
