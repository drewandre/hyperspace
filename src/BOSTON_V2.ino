#include<FastLED.h>
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305_patch.h>
#include <EEPROM.h>
unsigned int trigger_sensitivites_address = 0;



/*
   The initial variables below are high-level values for safely adjusting
   various parameters of the kit. Included alongside each values are
   their ranges. Do not go below/beyond these ranges.
*/

uint8_t master_brightness = 50; // 0 (off) to 255 (max brightness/amperage)

// sensor noise values can be adjusted on the circuit itself (resistors 1 to 14)
// below are additional software noise floors if additional noise removal is desired
uint16_t TRIGGER_MIN_tom_one      = 300; // 210
uint16_t TRIGGER_MIN_tom_two      = 0; // 210
uint16_t TRIGGER_MIN_tom_three    = 0; // 220
uint16_t TRIGGER_MIN_tom_four     = 0; // 240 (drum min)
uint16_t TRIGGER_MIN_kick_right   = 0; // 300
uint16_t TRIGGER_MIN_kick_left    = 0; // 300
uint16_t TRIGGER_MIN_snare        = 300; // 240 (drum min/240)
uint16_t TRIGGER_MIN_cymbal_one   = 0; // 210 (cymbal min/210)
uint16_t TRIGGER_MIN_cymbal_two   = 0; // 210 (cymbal min)
uint16_t TRIGGER_MIN_cymbal_three = 0; // 210 (cymbal min)
uint16_t TRIGGER_MIN_cymbal_four  = 0; // 210 (cymbal min)
uint16_t TRIGGER_MIN_cymbal_five  = 0; // 210 (cymbal min)
uint16_t TRIGGER_MIN_aux_one      = 0; // 210 (cymbal min)
uint16_t TRIGGER_MIN_aux_two      = 0; // 210 (cymbal min)
uint16_t TRIGGER_MIN_aux_three    = 0; // DOES NOT EXIST
uint16_t TRIGGER_MIN_aux_four     = 0; // DOES NOT EXIST

// comment debug to enable debugging
#define debug
#ifdef debug
//#define print_trigger_values
//#define auto_drummer
//#define print_fps
//#define display_strip_channel_number
#define print_button_states
#define print_EEPROM_values
#endif

#define display_logos

//#define effect_fixed
#ifdef  effect_fixed
uint8_t effect_fixed_num = 1;
#endif

bool user_input = true;
bool inverted_display = false;
uint8_t flash;
long flash_interval = 250; // rate at which oled screen inverts when picking modes
uint8_t RGB_pulse_interval = 13; // rate at which RGB button pulese when picking mode
#define OLED_CLK   3
#define OLED_MOSI  4
#define OLED_CS    1
#define OLED_DC    25
#define OLED_RESET 0
Adafruit_SSD1305 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS); // software SPI

const uint8_t num_modes = 6;
uint8_t master_mode = 0;
uint8_t temp_mode = 0; // startup mode always set to "off"
char*   effects[num_modes] = {"OFF", "GREEN", "MAGENTA", "WHITE", "RAINBOW", "HYPERSPACE"};
uint8_t effectNameXPos[num_modes + 1] = {46, 33, 23, 35, 23, 5};

#define NUM_STRIPS 16
#define MAX_LEDS_PER_STRIP 288 // TODO: this unfortunately doubles the object size..
uint16_t STRIP_LED_COUNT;
const uint16_t MAX_LED_COUNT = NUM_STRIPS * MAX_LEDS_PER_STRIP;
uint8_t current_strip;

CRGB leds[MAX_LED_COUNT];

// order here is very important
char* drumNames[NUM_STRIPS] = {"TOM_1: ", "CYM_4: ", "TOM_4: ", "KIK_L: ", "TOM_3: ", "CYM_2: ", "CYM_1: ", "TOM_2: ", "CYM_3: ", "AUX_3: ", "AUX_4: ", "KIK_R: ", "SNARE: ", "CYM_5: ", "AUX_2: ", "AUX_1: "};
//char* drumNames[NUM_STRIPS] = {"TOM_1: ", "TOM_2: ", "TOM_3: ", "TOM_4: ", "KIK_R: ", "KIK_L: ", "SNARE: ", "CYM_1: ", "CYM_2: ", "CYM_3: ", "CYM_4: ", "CYM_5: ", "AUX_1: ", "AUX_2: ", "AUX_3: ", "AUX_4: "};

uint16_t drum_sensitivities[NUM_STRIPS] = {TRIGGER_MIN_tom_one, TRIGGER_MIN_cymbal_four, TRIGGER_MIN_tom_four, TRIGGER_MIN_kick_left, TRIGGER_MIN_tom_three, TRIGGER_MIN_cymbal_two, TRIGGER_MIN_cymbal_one, TRIGGER_MIN_tom_two, TRIGGER_MIN_cymbal_three, TRIGGER_MIN_aux_three, TRIGGER_MIN_aux_four, TRIGGER_MIN_kick_right, TRIGGER_MIN_snare, TRIGGER_MIN_cymbal_five, TRIGGER_MIN_aux_two, TRIGGER_MIN_aux_one};

const uint16_t NUM_LEDS_tom_one      = 288;
const uint16_t NUM_LEDS_tom_two      = 288;
const uint16_t NUM_LEDS_tom_three    = 288;
const uint16_t NUM_LEDS_tom_four     = 288;
const uint16_t NUM_LEDS_kick_right   = 280;
const uint16_t NUM_LEDS_kick_left    = 280;
const uint16_t NUM_LEDS_snare        = 144;
const uint16_t NUM_LEDS_cymbal_one   = 64;
const uint16_t NUM_LEDS_cymbal_two   = 64;
const uint16_t NUM_LEDS_cymbal_three = 64;
const uint16_t NUM_LEDS_cymbal_four  = 64;
const uint16_t NUM_LEDS_cymbal_five  = 64;
const uint16_t NUM_LEDS_aux_one      = 0;
const uint16_t NUM_LEDS_aux_two      = 0;
const uint16_t NUM_LEDS_aux_three    = 0; // DOES NOT EXIST
const uint16_t NUM_LEDS_aux_four     = 0; // DOES NOT EXIST

const uint16_t TOTAL_LEDS = NUM_LEDS_tom_one + NUM_LEDS_tom_two + NUM_LEDS_tom_three + NUM_LEDS_tom_four + NUM_LEDS_kick_right + NUM_LEDS_kick_left + NUM_LEDS_snare + NUM_LEDS_cymbal_one + NUM_LEDS_cymbal_two + NUM_LEDS_cymbal_three + NUM_LEDS_cymbal_four + NUM_LEDS_cymbal_five + NUM_LEDS_aux_one + NUM_LEDS_aux_two + NUM_LEDS_aux_three + NUM_LEDS_aux_four;
//uint16_t NUM_LEDS_PER_STRIP[NUM_STRIPS] = {NUM_LEDS_tom_one, NUM_LEDS_tom_two, NUM_LEDS_tom_three, NUM_LEDS_tom_four, NUM_LEDS_kick_right, NUM_LEDS_kick_left, NUM_LEDS_snare, NUM_LEDS_cymbal_one, NUM_LEDS_cymbal_two, NUM_LEDS_cymbal_three, NUM_LEDS_cymbal_four, NUM_LEDS_cymbal_five, NUM_LEDS_aux_one, NUM_LEDS_aux_two, NUM_LEDS_aux_three, NUM_LEDS_aux_four};
// very important ordering:
uint16_t NUM_LEDS_PER_STRIP[NUM_STRIPS] = {NUM_LEDS_tom_one, NUM_LEDS_cymbal_four, NUM_LEDS_tom_four, NUM_LEDS_kick_left, NUM_LEDS_tom_three, NUM_LEDS_cymbal_two, NUM_LEDS_cymbal_one, NUM_LEDS_tom_two, NUM_LEDS_cymbal_three, NUM_LEDS_aux_three, NUM_LEDS_aux_four, NUM_LEDS_kick_right, NUM_LEDS_snare, NUM_LEDS_cymbal_five, NUM_LEDS_aux_two, NUM_LEDS_aux_one};

#define TRIGGER_tom_one      A14
#define TRIGGER_tom_two      A13
#define TRIGGER_tom_three    A12
#define TRIGGER_tom_four     A11
#define TRIGGER_kick_right   30
#define TRIGGER_kick_left    A10
#define TRIGGER_snare        31
#define TRIGGER_cymbal_one   19
#define TRIGGER_cymbal_two   18
#define TRIGGER_cymbal_three 17
#define TRIGGER_cymbal_four  26
#define TRIGGER_cymbal_five  27
#define TRIGGER_aux_one      28
#define TRIGGER_aux_two      29
#define TRIGGER_aux_three    255 // DOES NOT EXIST
#define TRIGGER_aux_four     255 // DOES NOT EXIST

// software *typical* noise floor constrain values
uint16_t typicalDMax   = 1000;
uint16_t typicalCMax   = 1000;

float pixelPoint = 128.0 / typicalDMax;
uint16_t pixel_x;

const uint16_t TRIGGER_MAX_tom_one      = typicalDMax;
const uint16_t TRIGGER_MAX_tom_two      = typicalDMax;
const uint16_t TRIGGER_MAX_tom_three    = typicalDMax;
const uint16_t TRIGGER_MAX_tom_four     = typicalDMax;
const uint16_t TRIGGER_MAX_kick_right   = typicalDMax;
const uint16_t TRIGGER_MAX_kick_left    = typicalDMax;
const uint16_t TRIGGER_MAX_snare        = typicalDMax;
const uint16_t TRIGGER_MAX_cymbal_one   = typicalCMax;
const uint16_t TRIGGER_MAX_cymbal_two   = typicalCMax;
const uint16_t TRIGGER_MAX_cymbal_three = typicalCMax;
const uint16_t TRIGGER_MAX_cymbal_four  = typicalCMax;
const uint16_t TRIGGER_MAX_cymbal_five  = typicalCMax;
const uint16_t TRIGGER_MAX_aux_one      = typicalDMax; // assumes aux inputs
const uint16_t TRIGGER_MAX_aux_two      = typicalDMax; // are set at drum max
const uint16_t TRIGGER_MAX_aux_three    = typicalDMax; // DOES NOT EXIST
const uint16_t TRIGGER_MAX_aux_four     = typicalDMax; // DOES NOT EXIST

uint16_t currentTriggerMin, currentTriggerMax;
volatile uint16_t currentTriggerValue;
uint8_t  currentTrigger;
uint16_t triggerValues[NUM_STRIPS];
//uint8_t  triggerInputs[NUM_STRIPS] = {TRIGGER_tom_one, TRIGGER_tom_two, TRIGGER_tom_three, TRIGGER_tom_four, TRIGGER_kick_right, TRIGGER_kick_left, TRIGGER_snare, TRIGGER_cymbal_one, TRIGGER_cymbal_two, TRIGGER_cymbal_three, TRIGGER_cymbal_four, TRIGGER_cymbal_five, TRIGGER_aux_one, TRIGGER_aux_two, TRIGGER_aux_three, TRIGGER_aux_four};
//uint16_t triggerMin[NUM_STRIPS] = {TRIGGER_MIN_tom_one, TRIGGER_MIN_tom_two, TRIGGER_MIN_tom_three, TRIGGER_MIN_tom_four, TRIGGER_MIN_kick_right, TRIGGER_MIN_kick_left, TRIGGER_MIN_snare, TRIGGER_MIN_cymbal_one, TRIGGER_MIN_cymbal_two, TRIGGER_MIN_cymbal_three, TRIGGER_MIN_cymbal_four, TRIGGER_MIN_cymbal_five, TRIGGER_MIN_aux_one, TRIGGER_MIN_aux_two, TRIGGER_MIN_aux_three, TRIGGER_MIN_aux_four};
//uint16_t triggerMax[NUM_STRIPS] = {TRIGGER_MAX_tom_one, TRIGGER_MAX_tom_two, TRIGGER_MAX_tom_three, TRIGGER_MAX_tom_four, TRIGGER_MAX_kick_right, TRIGGER_MAX_kick_left, TRIGGER_MAX_snare, TRIGGER_MAX_cymbal_one, TRIGGER_MAX_cymbal_two, TRIGGER_MAX_cymbal_three, TRIGGER_MAX_cymbal_four, TRIGGER_MAX_cymbal_five, TRIGGER_MAX_aux_one, TRIGGER_MAX_aux_two, TRIGGER_MAX_aux_three, TRIGGER_MAX_aux_four};

// order here is extremely important to match with writing of led data. if off, trigger will write color data to the wrong led strip.
uint8_t  triggerInputs[NUM_STRIPS] = {TRIGGER_tom_one, TRIGGER_cymbal_four, TRIGGER_tom_four, TRIGGER_kick_left, TRIGGER_tom_three, TRIGGER_cymbal_two, TRIGGER_cymbal_one, TRIGGER_tom_two, TRIGGER_cymbal_three, TRIGGER_aux_three, TRIGGER_aux_four, TRIGGER_kick_right, TRIGGER_snare, TRIGGER_cymbal_five, TRIGGER_aux_two, TRIGGER_aux_one};
uint16_t triggerMin[NUM_STRIPS] = {TRIGGER_MIN_tom_one, TRIGGER_MIN_cymbal_four, TRIGGER_MIN_tom_four, TRIGGER_MIN_kick_left, TRIGGER_MIN_tom_three, TRIGGER_MIN_cymbal_two, TRIGGER_MIN_cymbal_one, TRIGGER_MIN_tom_two, TRIGGER_MIN_cymbal_three, TRIGGER_MIN_aux_three, TRIGGER_MIN_aux_four, TRIGGER_MIN_kick_right, TRIGGER_MIN_snare, TRIGGER_MIN_cymbal_five, TRIGGER_MIN_aux_two, TRIGGER_MIN_aux_one};
uint16_t triggerMax[NUM_STRIPS] = {TRIGGER_MAX_tom_one, TRIGGER_MAX_cymbal_four, TRIGGER_MAX_tom_four, TRIGGER_MAX_kick_left, TRIGGER_MAX_tom_three, TRIGGER_MAX_cymbal_two, TRIGGER_MAX_cymbal_one, TRIGGER_MAX_tom_two, TRIGGER_MAX_cymbal_three, TRIGGER_MAX_aux_three, TRIGGER_MAX_aux_four, TRIGGER_MAX_kick_right, TRIGGER_MAX_snare, TRIGGER_MAX_cymbal_five, TRIGGER_MAX_aux_two, TRIGGER_MAX_aux_one};


#define REDPIN    22
#define BLUEPIN   32
#define GREENPIN  23
uint8_t RGB_brightness = 255;

#define num_buttons   3
#define CUE_BACKWARDS 16
#define CUE_FORWARD   33
#define GO            24
uint8_t  buttonReadings[num_buttons];
uint8_t  buttonStates[num_buttons];
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 30;
uint8_t  lastButtonState[num_buttons] = {HIGH, HIGH, HIGH};
const uint8_t buttons[num_buttons] = {CUE_BACKWARDS, CUE_FORWARD, GO};

unsigned long threshold_hold_delay = 30;
long previousThresholdMillis = 0;
unsigned long previousMillis = 0;
uint8_t hue;

uint16_t left_pos, right_pos, position, width, start_of_each_strip, current_strip_length;

uint16_t value;

bool cue_backwards_hold = false;
bool cue_forwards_hold = false;
bool adjust_thresholds = false;
uint8_t button_mode = 0;

bool first_enter_into_case_0 = true;
bool first_enter_into_case_1 = false;
bool entering_calibration = true;
bool exiting_calibration = false;

bool just_entered_calibration = false;
bool just_exited_calibration = false;

bool doublePressed = true;

uint8_t line_x_start = 32;
uint8_t line_x_end = 96;
uint8_t line_x = line_x_start;

uint16_t trigger_sensitivity;
byte current_drum_to_calibrate = 0;

uint8_t safety_mode_backup_variable = temp_mode;

void setup() {
  initialize();
}

void loop() {

  read_all_triggers();

  check_for_user_input();
  if (user_input) {
    update_display();
  }

  mode();

  display_leds();

}
