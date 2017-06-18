void display_leds() {

#ifdef display_strip_channel_number
  // Set the first n leds on each strip to show which strip it is
  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    for (uint8_t led_index = 0; led_index <= strip; led_index++) {
      leds[(strip * MAX_LEDS_PER_STRIP) + led_index] = CRGB::White;
    }
  }
#endif

  LEDS.show();

#ifdef print_fps
  Serial.println(LEDS.getFPS());
#endif
}


void showAnalogRGB( const CRGB& rgb)
{ // for use with common anode RGB LED
  analogWrite(REDPIN,   255 - rgb.r );
  analogWrite(GREENPIN, 255 - rgb.g );
  analogWrite(BLUEPIN,  255 - rgb.b );
}

void update_display() {

  switch (button_mode) {

    case 0:

      if (temp_mode != master_mode) {
        display.clearDisplay();
        display.setCursor(5, 4);
        display.setTextSize(1);
        display.println("press GO to display: ");
        display.setCursor(effectNameXPos[temp_mode], 16);
        display.setTextSize(2.5);
        display.println(effects[temp_mode]);
        display.display();
      } else { // else if temp_mode = master_mode
        display.clearDisplay();
        display.setCursor(32, 4);
        display.setTextSize(1);
        display.println("displaying: ");
        display.setCursor(effectNameXPos[temp_mode], 16);
        display.setTextSize(2.5);
        display.println(effects[temp_mode]);
        display.display();
        display.invertDisplay(false);
        if (master_mode == 0) {
          showAnalogRGB(CRGB::Black);
        } else if (master_mode == 1) { // green
          showAnalogRGB(CRGB::Green);
        } else if (master_mode == 2) { // magenta
          showAnalogRGB(CRGB::Magenta);
        } else if (master_mode == 3) { // white
          showAnalogRGB(CRGB::White);
        }
      }
      user_input = false;
      //Serial.println("display updated");
      break;

    case 1:

      if (entering_calibration) { // entering
        display.invertDisplay(false);
        display.setCursor(15, 10);
        display.setTextSize(1);
        display.println("HOLD TO CALIBRATE");
        display.drawPixel(line_x, 22, WHITE);
        display.drawRect(30, 20, 70, 5, WHITE);
        display.display();
        line_x++;
        if (line_x > line_x_end) {
          display.clearDisplay();
          display.setCursor(11, 11);
          display.setTextSize(2);
          display.println("CALIBRATE");
          display.display();
          line_x = line_x_start;

          while (doublePressed) {

            uint8_t temp_cue_backwards = digitalRead(CUE_BACKWARDS);
            uint8_t temp_cue_forwards = digitalRead(CUE_FORWARD);

            if ((temp_cue_backwards == 0) && (temp_cue_forwards == 0)) {
              doublePressed = true;
            }
            if ((temp_cue_backwards == 1) && (temp_cue_forwards == 1)) {
              doublePressed = false;
            }
          }

          doublePressed = true;
          button_mode = 2; // set a calibration flag
          entering_calibration = false;
          exiting_calibration = true;
          just_entered_calibration = true;
          delay(500);
          display.clearDisplay();
          display.display();
        }
      }

      else if (exiting_calibration) {

        display.invertDisplay(false);
        display.setCursor(12, 10);
        display.setTextSize(1);
        display.println("HOLD TO EXIT");
        display.drawPixel(line_x, 25, WHITE);

        //        display.drawRect(1, 23, display.width() - 1, display.height() - 23, WHITE);
        //        display.fillRect(2, 24, display.width() - pixel_x, display.height() - 22, WHITE);

        display.display();
        line_x++;
        if (line_x > line_x_end) {
          button_mode = 0; // set a calibration flag
          entering_calibration = false;
          exiting_calibration = false;
          just_exited_calibration = true;
          temp_mode = safety_mode_backup_variable;
          line_x = line_x_start;
          display.clearDisplay();
          display.display();
        }

        current_drum_to_calibrate = 0;

      }

      break;

    case 2:

      display.clearDisplay();
      display.setCursor(1, 3);
      display.setTextSize(2);
      display.println(drumNames[current_drum_to_calibrate]);
      display.setCursor(71, 6);
      display.setTextSize(1);
      currentTriggerMin = triggerMin[current_drum_to_calibrate];
      display.print(currentTriggerMin);
      display.print("/");
      display.println(typicalDMax);

      pixel_x = (currentTriggerMin * pixelPoint) - 3;
      display.drawPixel(pixel_x + 1, 21, WHITE);
      display.drawPixel(pixel_x, 20, WHITE);
      display.drawPixel(pixel_x + 1, 20, WHITE);
      display.drawPixel(pixel_x + 2, 20, WHITE);

      display.drawRect(1, 23, display.width() - 1, display.height() - 23, WHITE);
      display.fillRect(3, 25, display.width() - (triggerValues[current_drum_to_calibrate] * pixelPoint) - 5, display.height() - 27, WHITE);

      display.display();
      user_input = false;
      break;

  }
}
