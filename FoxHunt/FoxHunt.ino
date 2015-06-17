/* Fox Hunt */

#include <HAMShield.h>
#include <Wire.h>

// transmit for 1 minute, every 10 minutes 

#define TRANSMITLENGTH 1
#define INTERVAL 10

HAMShield radio;

void setup() { 
  Wire.begin();
  radio.initialize();
  radio.setFrequency(145510);
  radio.setModeReceive();
}

void loop() {
   waitMinute(INTERVAL);
   if(radio.waitForChannel(30000,2000)) {
     radio.setModeTransmit();
     tone(1000,11,TRANSMITLENGTH * 60 * 1000);        // play a long solid tone
     radio.morseOut("1ZZ9ZZ/B FOXHUNT");
     radio.setModeReceive();
   }
}

void waitMinute(int period) { 
  delay(period * 60 * 1000);
}


