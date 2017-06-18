void rainbow() {

  static uint8_t hue = 0;
  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    //STRIP_LED_COUNT = NUM_LEDS_PER_STRIP[strip];
    for (uint16_t led_index = 0; led_index < MAX_LEDS_PER_STRIP; led_index++) {
      leds[(strip * MAX_LEDS_PER_STRIP) + led_index] = CHSV((32 * strip) + hue + led_index, 255, 255);
      //leds[(strip * STRIP_LED_COUNT) + led_index] = CHSV(0, 255, 255);
      //Serial.println(led_index);
      // print this ^ see if it's going to ~4000 leds?
    }
  }
  hue++;
  
}

uint8_t rainbow_hues[NUM_STRIPS] = {21, 190, 84, 0, 63, 147, 126, 42, 169, /*DNE*/0, 0/*DNE*/, 0, 105, 211, 253, 232 /* should be 266.. */};


void each_channel_set_rainbow_color() {

  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    //    Serial.print("current strip = ");
    //    Serial.print(strip);
    hue = rainbow_hues[strip];
    start_of_each_strip = strip * MAX_LEDS_PER_STRIP;
    for (uint16_t led_index = 0; led_index < MAX_LEDS_PER_STRIP; led_index++) {
      leds[start_of_each_strip + led_index] = CHSV(hue, 255, 255);
    }
  }
}

