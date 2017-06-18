void hyperspace() {

  // one giant array
  // runs at 71fps

  nscale8(leds, MAX_LED_COUNT, 250);
  //fadeToBlackBy(leds, TOTAL_LEDS, 5);
  //hue++;

  EVERY_N_MILLIS_I( drum_timer, 50 ) {
    for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {

      drum_timer.setPeriod( random8( 10, 200) );

      position = random16(MAX_LED_COUNT - 1);
      width = random8(2, 17);
      left_pos  = position - width;
      right_pos = position + width;

      if (left_pos < 0) {
        left_pos = 0;
      }
      if (right_pos > MAX_LED_COUNT - 1) {
        right_pos = MAX_LED_COUNT - 1;
      }

      for (uint16_t led_index = left_pos; led_index < right_pos; led_index++) {
        leds[led_index] += CRGB::White;
        //leds[led_index] += CHSV(hue, 255, 255);
      }
    }
  }
  blur1d(leds, MAX_LED_COUNT, 150);
}


void hyperspace_individual_arrays() {

  // many small arrays for each drum

  blur1d(leds, TOTAL_LEDS, 120);
  //nscale8(leds, TOTAL_LEDS, 170);
  fadeToBlackBy(leds, TOTAL_LEDS, 5);


  EVERY_N_MILLIS_I( drum_timer, 50 ) {
    
    for (uint8_t strip = 0; strip < NUM_STRIPS - 2; strip++) {
      current_strip_length = NUM_LEDS_PER_STRIP[strip];
      start_of_each_strip  = strip * MAX_LEDS_PER_STRIP;

      //Serial.println(current_strip_length);
      drum_timer.setPeriod( random8( 10, 200) );

      position = random16(start_of_each_strip, start_of_each_strip + current_strip_length - 1);
      width = random8(2, 15);
      left_pos  = position - width;
      right_pos = position + width;

      if (left_pos < start_of_each_strip) {
        left_pos = start_of_each_strip;
      }
      if (right_pos > (start_of_each_strip + current_strip_length - 1)) {
        right_pos = start_of_each_strip + current_strip_length - 1;
      }

      for (uint16_t led_index = left_pos; led_index < right_pos; led_index++) {
        leds[led_index] += CRGB::White;
      }
    }
  }
  //LEDS.show();
}



