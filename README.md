![band boston logo](http://www.trademarkandcopyrightlawblog.com/wp-content/uploads/sites/9/2016/11/boston-1.png)
# HyperSpace LED Drum Kit
###### Featured on band Boston's 2017 HyperSpace Tour: 14-channel LED controller for use with piezo sensors, WS2812b LEDS, Teensy 3.2 and FastLED library. Version 1.3 - Boston, MA - June 2017.
*This project uses hardware which cannot be made publically available due to the nature of client contract. The firmware, though, uses the <a href='https://github.com/FastLED/FastLED'>FastLED</a> library running on a <a href='https://www.pjrc.com/store/teensy32.html'>Teensy 3.2</a>. Regular ol' pizeo sensors can be used to send trigger events to the Teensy, which should through full bridge rectifiers to covert the AC signals to a safe, readable DC range for the Microcontroller. Teensy 3.2 operates at 3v logic, so a logic buffer is required.*

Hardware and firmware designed by <a href='www.drew-andre.com'>Drew André</a>.

***Tour description:*** Music legends BOSTON continue their Hyper Space Tour this summer with the iconic JOAN JETT & THE BLACKHEARTS, for an additional 21 cities, presented by Live Nation. Performances with Joan Jett will begin on Wednesday, June 14 in Mountain View, CA at Shoreline Amphitheatre, and continue eastward across North America. More information can be found at livenation.com. Itinerary for the Live Nation portion of BOSTON’s Hyper Space Tour with Joan Jett below.

## Description
The HyperSpace LED Drum Kit system reads piezo drum sensor events and triggers various LED effects. The system is waterproof, extensively stress-tested for road conditions, and configurable on-the-fly. The system can save noise thresholds for any number of drummers/playing styles, and features a high-contrast OLED screen for configuration. The effects uploaded here are highly specific to client specifications, but can be configured to taste.

***2,176 leds used in this project running at 172fps!***

[![HyperSpace LED Drum Kit](https://img.youtube.com/vi/aQiT6eR48F4/0.jpg)](https://www.youtube.com/watch?v=aQiT6eR48F4)       

## Hardware
This systems assumes the following pin/cable mapping:
  ```
  - CHAN 1: tom one       CHAN 8:  cymbal one
  - CHAN 2: tom two       CHAN 9:  cymbal two
  - CHAN 3: tom three     CHAN 10: cymbal three
  - CHAN 4: tom four      CHAN 11: cymbal four
  - CHAN 5: kick_left     CHAN 12: cymbal five
  - CHAN 6: kick_right    CHAN 13: aux one
  - CHAN 7: snare         CHAN 14: aux two
  ```

This program does not write to each drum in that order, however.
Teensy 3.2 + FastLED 16-channel parallel output will write
in the following order:

```
- CH:     DRUM:
- ch. 1   (tom one)   
- ch. 11  (cymbal 4)   
- ch. 4   (tom four)  
- ch. 5   (kick left)
- ch. 3   (tom 3)     
- ch. 9   (cymbal 2)   
- ch. 8   (cymbal 1)   
- ch. 2   (tom two)   
- ch. 10  (cymbal three
- ch. 6   (kick right
- ch. 7   (snare)      
- ch. 12  (cymbal 5)   
- ch. 14  (aux 2)      
- ch. 13  (aux 1)      
```

## Equipment
* 144led/meter WS2812b addressable LED strips: https://www.adafruit.com/products/1506
* 8x8 WS2812b panels: https://www.amazon.com/gp/product/B01MCUOD8N/ref=oh_aui_detailpage_o05_s01?ie=UTF8&psc=1
* Daniel Garcia and Mark Kriegsman's FastLED library: http://fastled.io/
* Teensy 3.2: https://www.pjrc.com/store/teensy32.html
* Custom designed PCB, Piezo sensors, and 5-pin cable
* 60A power supply: https://www.amazon.com/gp/product/B014018EWA/ref=oh_aui_detailpage_o06_s00?ie=UTF8&psc=1
* 128x32 OLED screen: https://www.adafruit.com/product/2675

## Contact for custom builds
I can be reached at <a href='mailto:drewjamesandre@gmail.com'>drewjamesandre@gmail.com</a>. 
