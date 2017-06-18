void initialize() {

  unsigned long startup_ms = millis();

#ifdef debug
  Serial.begin(115200);
  delay(500);
#endif

  pinMode(REDPIN,   OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN,  OUTPUT);

  init_ws2812b();
  init_ssd1305();
  init_buttons();
  restore_drum_sensitivity_settings();

  showAnalogRGB(CRGB::Black);

  master_mode = 0;
  temp_mode = 0;

#ifdef debug
  uint16_t value = startup_ms - previousMillis;
  Serial.println();
  Serial.print("Startup time: ");
  Serial.print(value);
  Serial.print("ms");
  Serial.println();
  Serial.print("effect = ");
  Serial.print(effects[master_mode]);
  Serial.println();
  Serial.println();
  Serial.println("--------------- MAIN LOOP ---------------");
  Serial.println();
#endif

}


void init_ws2812b() {
  //uint16_t average_strip_size = ceil(TOTAL_LEDS / NUM_STRIPS);
  // WS2811_PORTDC: 2,14,7,8,6,20,21,5,15,22,23,9,10,13,11,12 - 16 way parallel
  LEDS.addLeds<WS2811_PORTDC, NUM_STRIPS>(leds, MAX_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  LEDS.setBrightness(master_brightness);
  fill_solid(leds, TOTAL_LEDS, CRGB::Black);
  LEDS.show();
}

void init_buttons() {
  pinMode(CUE_BACKWARDS, INPUT);
  digitalWrite(CUE_BACKWARDS, HIGH);
  pinMode(CUE_FORWARD, INPUT);
  digitalWrite(CUE_FORWARD, HIGH);
  pinMode(GO, INPUT);
  digitalWrite(GO, HIGH);

  user_input = false;
  check_for_user_input();

}

void init_ssd1305() {

  display.begin();
  display.clearDisplay(); // clears the screen and buffer
  display.setTextColor(WHITE);
  display.invertDisplay(false);

#ifdef display_logos
  display.drawBitmap(0, 1, boston_logo_bmp, display.width(), display.height(), WHITE);
  display.display();
  delay(1500);
  display.clearDisplay();
  display.drawBitmap(0, 1, drew_andre_bmp, display.width(), display.height(), WHITE);
  display.display();
  delay(1500);
  display.clearDisplay();
  display.display();
#endif

}

void restore_drum_sensitivity_settings() {

  unsigned int drum_sensitity_preset = 0;
  byte current_eeprom_value;
  for (drum_sensitity_preset = 0; drum_sensitity_preset < NUM_STRIPS; drum_sensitity_preset++) {
    current_eeprom_value = EEPROM.read(drum_sensitity_preset);
    current_eeprom_value *= 4;
    triggerMin[drum_sensitity_preset] = current_eeprom_value;
#ifdef print_EEPROM_values
    Serial.print(drumNames[drum_sensitity_preset]);
    Serial.print(" ");
    Serial.print(triggerMin[drum_sensitity_preset]);
    Serial.println();
#endif
  }
#ifdef print_EEPROM_values
  Serial.println();
#endif

}


