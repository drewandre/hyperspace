void read_all_triggers() {

  for (uint8_t i = 0; i < NUM_STRIPS - 2; i++) {

    currentTrigger = triggerInputs[i];
    currentTriggerMin = triggerMin[i];
    //currentTriggerMax = triggerMax[i];

    currentTriggerValue = analogRead(currentTrigger);

    currentTriggerValue = constrain(currentTriggerValue, currentTriggerMin, typicalDMax); // was currentTriggerMax
    // currentTriggerValue = map(currentTriggerValue, currentTriggerMin, currentTriggerMax, 0, currentTriggerMax);

    triggerValues[i] = currentTriggerValue;

  }

#ifdef print_trigger_values
  PRINT_TRIGGER_DATA();
#endif

#ifdef auto_drummer
  AUTO_TRIGGER();
#endif

}

void AUTO_TRIGGER() {

  static uint16_t drum_num;
  EVERY_N_MILLIS_I( drum_timer, 250 ) {
    drum_timer.setPeriod( random8( 10, 200) );
    drum_num = random(0, NUM_STRIPS - 3);
    triggerValues[drum_num] = typicalDMax;
  }

}

void PRINT_TRIGGER_DATA() {
  for (uint8_t i  = 0; i < NUM_STRIPS - 2; i++) { // NUM_STRIPS - 2 to ignore aux 3 and 4
    Serial.print(drumNames[i]);
    Serial.print(triggerValues[i]);
    Serial.print("\t");
  }
  Serial.println();
}


