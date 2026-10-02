#include "driver/pulse_cnt.h"

// =========================================================================
// HYPER S3 ROCKET - Hardware Pin Configuration
// =========================================================================
#define SITIME_CLOCK_PIN 15  // Input from your SiTime Oscillator

// Select your active physical LED track below:
#define USER_LED_PIN     48  // Blue Status LED (Default Active)
// #define USER_LED_PIN   45  // Green LED
// #define USER_LED_PIN   46  // Red LED (Backside Placement)
// #define USER_LED_PIN   21  // Alternative Red LED

#define TARGET_PULSES    16384 // 32,768 Hz / 16,384 = clean 0.5s intervals

pcnt_unit_handle_t pcnt_unit = NULL;
volatile bool led_state = false;
uint32_t total_seconds = 0;
bool last_known_state = false;

// =========================================================================
// Hardware Interrupt Service Routine (ISR)
// =========================================================================
static bool IRAM_ATTR pcnt_on_reach_watch_point(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx) {
    led_state = !led_state;
    digitalWrite(USER_LED_PIN, led_state); // Execute state transition instantly
    return true; 
}

void setup() {
    delay(1000);
    Serial.begin(115200);
    Serial.println("\n--- HYPER S3 ROCKET Flight Clock Online (Modern API) ---");

    pinMode(USER_LED_PIN, OUTPUT);
    digitalWrite(USER_LED_PIN, LOW);

    // 1. Configure the Main Unit Accumulator
    pcnt_unit_config_t unit_config = {
        .low_limit = -1,
        .high_limit = TARGET_PULSES, // Automatic silicon boundary
    };
    pcnt_new_unit(&unit_config, &pcnt_unit);

    // 2. Configure the Channel Layout Mapped to GPIO 15
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = SITIME_CLOCK_PIN,
        .level_gpio_num = -1, // Unused control trace
    };
    pcnt_channel_handle_t pcnt_chan = NULL;
    pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan);

    // Increase on rising edges, hold/freeze on falling edges
    pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD);

    // 3. Establish the Event Target Limit
    pcnt_unit_add_watch_point(pcnt_unit, TARGET_PULSES);

    // 4. Attach the Callback Handler Structure
    pcnt_event_callbacks_t cbs = {
        .on_reach = pcnt_on_reach_watch_point,
    };
    pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, NULL);

    // 5. Fire Up the Hardware Subsystem
    pcnt_unit_enable(pcnt_unit);
    pcnt_unit_clear_count(pcnt_unit); 
    pcnt_unit_start(pcnt_unit);

    Serial.println("Hardware counter is running. Processing completely offline.");
}

void loop() {
    // Sample the hardware variable safely once per loop
    bool current_state = led_state;

    // Detect a rising edge on the LED state (LED turned ON, indicating the start of a new second)
    if (last_known_state == false && current_state == true) {
        total_seconds++;
        
        Serial.print("Mission Time: ");
        Serial.print(total_seconds);
        Serial.println(" s");
    }

    last_known_state = current_state; // Sync the logic state anchor
}
