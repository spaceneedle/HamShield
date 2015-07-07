/* 

IMTS 

*/

#include <HAMShield.h>
#include <Wire.h>

HAMShield radio;

void setup() { 
  Serial.begin(9600);
  Serial.println("starting up..");
  Wire.begin();
  pinMode(9,OUTPUT);
  Serial.print("Radio status: ");
  int result = radio.testConnection();
  Serial.println(result,DEC);
  radio.initialize();                         // setup radio
  radio.dangerMode();                         // turn off band restrictions
  radio.setFrequency(446000);                 // set to first IMTS channel (we are only using one)
  radio.setModeTransmit();
  Serial.println("Done with IMTS setup.");
}

int state = 0;

void loop() {
  tone(9,1200); delayMicroseconds(833); tone(9,2200); delayMicroseconds(833);
}

    

