
/*
  BOSTON 2017 HYPER SPACE TOUR: 14-channel LED controller for use with
                                piezo sensors, WS2812b LEDS, & Teensy 3.2

                                Version 1.3 - Boston, MA - June 2017

  Designed by Drew André: www.drewandre.net, www.linkedin.com/in/drewandre, www.instagram.com/drewandre

    This is a revised version of the project with the LEDS now running in parallel
    for a framerate increase of ~125% (15fps to ~64fps). This leaves more time
    to read the drum triggers, thus giving a more reactive feel to the kit.

      This systems assumes the following pin/cable mapping:
      CHAN 1: tom one       CHAN 8:  cymbal one
      CHAN 2: tom two       CHAN 9:  cymbal two
      CHAN 3: tom three     CHAN 10: cymbal three
      CHAN 4: tom four      CHAN 11: cymbal four
      CHAN 5: kick_left     CHAN 12: cymbal five
      CHAN 6: kick_right    CHAN 13: aux one
      CHAN 7: snare         CHAN 14: aux two

    This program does not write to each drum in that order, however.
    Teensy 3.2 + FastLED 16-channel parallel output will write
    in the following order:

      CH:     DRUM:
      ch. 1   (tom one)       TRUE  0   21
      ch. 11  (cymbal 4)      TRUE  1   190
      ch. 4   (tom four)      TRUE  2   84
      ch. 5   (kick left)     TRUE  3   0
      ch. 3   (tom 3)         TRUE  4   63
      ch. 9   (cymbal 2)      TRUE  5   147
      ch. 8   (cymbal 1)      TRUE  6   126
      ch. 2   (tom two)       TRUE  7   42
      ch. 10  (cymbal three)  TRUE  8   169
      // so here we are missing 9 and 10, respectively strips 15 and 16
      // account for by placing dummy triggers at strip #9 and #10
      ch. 6   (kick right)    true  11  0
      ch. 7   (snare)         true  12  105
      ch. 12  (cymbal 5)      true  13  211
      ch. 14  (aux 2)         true  14  253
      ch. 13  (aux 1)         true  15  232

    Boston HyperSpace Tour information

  "Music legends BOSTON continue their Hyper Space Tour this summer
  with the iconic JOAN JETT & THE BLACKHEARTS, for an additional 21 cities,
  presented by Live Nation. Performances with Joan Jett will begin on Wednesday,
  June 14 in Mountain View, CA at Shoreline Amphitheatre, and continue eastward
  across North America. More information can be found at livenation.com. Itinerary
  for the Live Nation portion of BOSTON’s Hyper Space Tour with Joan Jett below."

  Project files: www.github.com/drewandre/boston
  Project equipment:
   144led/meter WS2812b addressable LED strips: https://www.adafruit.com/products/1506
   8x8 WS2812b panels: https://www.amazon.com/gp/product/B01MCUOD8N/ref=oh_aui_detailpage_o05_s01?ie=UTF8&psc=1
   MEAS Vibration sensors  for cymbals and DDrum Acoustic Pro triggers/piezos for drums heads
   Daniel Garcia and Mark Kriegsman's FastLED library: http://fastled.io/
   Teensy 3.2: https://www.pjrc.com/store/teensy32.html
   Custom designed PCB
   60amp power supply: https://www.amazon.com/gp/product/B014018EWA/ref=oh_aui_detailpage_o06_s00?ie=UTF8&psc=1
   128x32 OLED screen: https://www.adafruit.com/product/2675

  Voltage dividers for piezo sensors use a 10pf cap and varying resistor values to remove vibration noise

  RESISTOR VALUES 4-7
  -----------------
  TOM 1:
  TOM 2: 6.2M Ohms 5%
  TOM 3: 10M Ohms 5%    (gold, blue, black, brown)
  TOM 4: 13M Ohms 5%    (gold, blue, orange, brown)
  KICK L: 13M Ohms 5%   (gold, blue, orange, brown)
  KICK R: 13M Ohms 5%   (gold, blue, orange, brown)
  SNARE: 13M Ohm 5%     (gold, blue, orange, brown)

  TODO:
  - adjust initial button_checking bug.. chooses a mode, which
    is immediately overwritten by setting startup mode to OFF at conclusion of setup..
    bug is written to serial monitor during startup.


  (begin MIT license for FastLED library)

    The MIT License (MIT)

    Copyright (c) 2013 FastLED

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in
    the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
    the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
    FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  (end MIT license)

*/

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
