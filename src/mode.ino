void mode() {

  //#ifdef effect_fixed
  //  master_mode = effect_fixed_num;
  //  temp_mode = master_mode;
  //#endif

  //master_mode = 1;

  switch (master_mode) {

    case 0:
      fill_solid(leds, MAX_LED_COUNT, CRGB::Black);
      display_leds();
      break;

    case 1:
      white();
      break;

    case 2:
      green();
      break;

    case 3:
      magenta();
      break;

    case 4:
      each_channel_set_rainbow_color();
      EVERY_N_MILLISECONDS(100) {
        hue++;
        showAnalogRGB(CHSV(hue, 255, 255));
      }
      break;

    case 5:
      hyperspace();
      RGB_brightness -= 10; // TODO: may need adjusting
      EVERY_N_MILLIS_I( flash_timer, 100 ) {
        RGB_brightness = 255;
        showAnalogRGB(CHSV(0, 0, RGB_brightness));
        flash_timer.setPeriod( random8( 10, 200) );
      }
      break;

    default:
      temp_mode = 0;
      master_mode = temp_mode;
      break;
  }

  // if user is changing modes, and the temp_mode ("mode") does not
  // match the master_mode variable used in the main switch case,
  // repeatedly invert the screen to indicate that the effect will
  // be switched when the GO trigger is pressed
  if ((temp_mode != master_mode) && (button_mode == 0)) {

    // invert display at a rate of flash_interval when picking modes
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis > flash_interval) {
      previousMillis = currentMillis;
      //Serial.println("CHOOSING NEW MODE");
      if (inverted_display == false) {
        display.invertDisplay(true);
        inverted_display = true;
      }
      else {
        display.invertDisplay(false);
        inverted_display = false;
      }
    }

    flash += RGB_pulse_interval;
    // pulse RGB button leds at given mode's color
    if (temp_mode == 0) { // if mode is "off"
      showAnalogRGB(CHSV(0, 0, flash * 0.5)); // pulse at a low white
    } else if (temp_mode == 1) { // if mode is "green"
      showAnalogRGB(CHSV(92, 255, flash));
    } else if (temp_mode == 2) { // if mode is "magenta"
      showAnalogRGB(CHSV(192, 255, flash));
    } else if (temp_mode == 3) { // if mode is "white"
      showAnalogRGB(CHSV(0, 0, flash));
    } else if (temp_mode == 4) { // if mode is "rainbow"
      EVERY_N_MILLISECONDS(75) {
        hue++;
        showAnalogRGB(CHSV(hue, 255, 255));
      }
    } else if (temp_mode == 5) {                // if mode is "hyperspace"
      RGB_brightness -= 10; // TODO: may need adjusting
      EVERY_N_MILLIS_I( flash_timer, 100 ) {
        RGB_brightness = 255;
        showAnalogRGB(CHSV(0, 0, RGB_brightness));
        flash_timer.setPeriod( random8( 10, 200) );
      }
    }
  }

}
