void check_for_user_input() {

  for (uint8_t i = 0; i < num_buttons; i++) {

    safety_mode_backup_variable = temp_mode;

    buttonReadings[i] = digitalRead(buttons[i]);
    if ((buttonReadings[0] == 0) && (buttonReadings[1] == 0)) {

      if ((button_mode == 0) && (!just_exited_calibration)) {
        display.clearDisplay();
        entering_calibration = true;
        exiting_calibration = false;
        button_mode = 1;
        user_input = true;
      }
      if (button_mode == 2) {
        display.clearDisplay();
        //exiting_calibration = true;
        button_mode = 1;
        user_input = true;
      }
    }
    else {
      line_x = line_x_start;
      just_exited_calibration = false;
      if (button_mode != 2) {
        button_mode = 0;
      }
    }

    switch (button_mode) {

      case 0: // 0 = regular cue_forward and cue_backwards modes

//        if (first_enter_into_case_0) {
//          Serial.println("just entered main mode chooser");
//          display.clearDisplay();
//          display.display();
//          display.invertDisplay(false);
//          first_enter_into_case_0 = false;
//          first_enter_into_case_1 = true;
//        }

        first_enter_into_case_1 = true;

        if (buttonReadings[i] != lastButtonState[i]) {
          lastDebounceTime = millis();
        }

        if ((millis() - lastDebounceTime) > debounceDelay) {
          if (buttonReadings[i] != buttonStates[i]) {
            buttonStates[i] = buttonReadings[i];

            if (buttonStates[0] == LOW) {
              temp_mode--;
              if ((temp_mode < 0) || (temp_mode > num_modes - 1)) {
                temp_mode = num_modes - 1;
              }
#ifdef print_button_states
              Serial.print("CUE-, temp_mode = ");
              Serial.print(temp_mode);
              Serial.print(", master_mode = ");
              Serial.print(master_mode);
              Serial.println();
#endif
              user_input = true;
            }

            if (buttonStates[1] == LOW) {
              temp_mode++;
              if ((temp_mode < 0) || temp_mode > num_modes - 1) {
                temp_mode = 0;
              }
#ifdef print_button_states
              Serial.print("CUE+, temp_mode = ");
              Serial.print(temp_mode);
              Serial.print(", master_mode = ");
              Serial.print(master_mode);
              Serial.println();
#endif
              user_input = true;
            }

            // GO
            if (buttonStates[2] == LOW) {
              master_mode = temp_mode;
#ifdef print_button_states
              Serial.print("GO: master_mode set to ");
              Serial.print(master_mode);
              Serial.println();
#endif
              user_input = true;
            } // end GO

          } // end changed button state

        } // end debounce check

        lastButtonState[i] = buttonReadings[i];
        break;

      case 1: // case 1 = calibrating modes

        //Serial.println("in case 1"); // yes

//        if (first_enter_into_case_1) {
//          display.clearDisplay();
//          display.display();
//          display.invertDisplay(false);
//          first_enter_into_case_1 = false;
//          first_enter_into_case_0 = true;
//        }

        break;

      case 2: // currently calibrating...

        // Serial.println("in case 2"); //yes
        if (buttonReadings[i] != lastButtonState[i]) {
          lastDebounceTime = millis();
        }

        if ((millis() - lastDebounceTime) > debounceDelay) {
          if (buttonReadings[i] != buttonStates[i]) {
            buttonStates[i] = buttonReadings[i];

            if (buttonStates[0] == LOW) {
              triggerMin[current_drum_to_calibrate] -= 20;
              if ((triggerMin[current_drum_to_calibrate] > typicalDMax) || (triggerMin[current_drum_to_calibrate] < 0)) {
                triggerMin[current_drum_to_calibrate] = 0;
              }
#ifdef print_button_states
              Serial.print("trigger_sensitivity = ");
              Serial.print(triggerMin[current_drum_to_calibrate]);
              Serial.println();
#endif
              
              EEPROM.write(current_drum_to_calibrate, (triggerMin[current_drum_to_calibrate] / 4));
              user_input = true;
            }

            if (buttonStates[1] == LOW) {
              triggerMin[current_drum_to_calibrate] += 20;
              if ((triggerMin[current_drum_to_calibrate] > typicalDMax) || (triggerMin[current_drum_to_calibrate] < 0)) {
                triggerMin[current_drum_to_calibrate] = typicalDMax;
              }

#ifdef print_button_states
              Serial.print("trigger_sensitivity = ");
              Serial.print(triggerMin[current_drum_to_calibrate]);
              Serial.println();
#endif
              EEPROM.write(current_drum_to_calibrate, (triggerMin[current_drum_to_calibrate] / 4));
              user_input = true;
            }

            // SWITCH DRUM_TO_CALIBRATE
            if (buttonStates[2] == LOW) {
              current_drum_to_calibrate++;
              if (current_drum_to_calibrate > NUM_STRIPS - 1) {
                current_drum_to_calibrate = 0;
              }
              if (current_drum_to_calibrate < 0) {
                current_drum_to_calibrate = NUM_STRIPS - 1;
              }
#ifdef print_button_states
              Serial.println(drumNames[current_drum_to_calibrate]);
#endif
              user_input = true;
            } // end GO for CASE 1

          } // end changed button state for CASE 1

        } // end debounce check for CASE 1

        lastButtonState[i] = buttonReadings[i];
        break;

    } // END SWITCH CASE!

  } // end button for loop

} // end button checking function
