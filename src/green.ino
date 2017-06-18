void green() {

  nscale8(leds, MAX_LED_COUNT, 200);

  for (int strip = 0; strip < NUM_STRIPS; strip++) {
    if (triggerValues[strip] > triggerMin[strip]) {
      for (int led_index = 0; led_index < MAX_LEDS_PER_STRIP; led_index++) {
        leds[(strip * MAX_LEDS_PER_STRIP) + led_index] += CRGB::Green;
      }
    }
  }
}

