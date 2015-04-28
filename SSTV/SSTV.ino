/* 

Sends an SSTV test pattern 

*/

#define DOT 100
#define CALLSIGN "1ZZ9ZZ/B"

/* Standard libraries and variable init */

#include <HAMShield.h>
#include <Wire.h>

HAMShield radio;
int16_t rssi;

/* get our radio ready */

void setup() { 
  Wire.begin();
  Serial.begin(9600);
  Serial.print("Radio status: ");
  int result = radio.testConnection();
  Serial.println(result);
  radio.initialize();
  radio.setFrequency(446000);
}

/* main program loop */


void loop() {
     if(radio.waitForChannel(0,2000)) {        // Wait forever for calling frequency to open, then wait 2 seconds for breakers 
       radio.setModeTransmit();                // Turn on the transmitter
       delay(250);                             // Wait a moment
       radio.morseOut(CALLSIGN);               // Identify our transmission
       delay(1000);                            // Wait a second
       radio.SSTVVISCode(ROBOT8BW);             // Send the VIS header for SC2-180
       // SSTV TO DO 
     } else { delay(30000); }                  // someone broke in fast after prior transmission, was it an emergency? wait 30 secs.
     
     delay(10000);
}

