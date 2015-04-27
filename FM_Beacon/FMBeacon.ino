/* 

Morse Code Beacon

Test beacon will transmit and wait 30 seconds. 
Beacon will check to see if the channel is clear before it will transmit.


*/

#include <RDA.h>
#include <Wire.h>

#define DOT 100

RDA1846 radio;

const char *ascii = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,?'!/()&:;=+-_\"$@",
  *itu[] = { ".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--..","-----",".----","..---","...--","....-",".....","-....","--...","---..","----.",".-.-.-","--..--","..--..",".----.","-.-.--","-..-.","-.--.","-.--.-",".-...","---...","-.-.-.","-...-",".-.-.","-....-","..--.-",".-..-.","...-..-",".--.-."
  };

void setup() { 
  Serial.begin(9600);
  Serial.println("starting up..");
  Wire.begin();
  Serial.print("Radio status: ");
  int result = radio.testConnection();
  Serial.println(result,DEC);
  radio.initialize();
  radio.setFrequency(446000);
  radio.setVolume1(0xF);
  radio.setVolume2(0xF);
  radio.setModeReceive();
  radio.setModeReceive();
  radio.setTxSourceMic();
  radio.setSQLoThresh(80);
  radio.setSQOn();
  Serial.println("Done with radio beacon setup.");
}

void loop() {
   int16_t rssi = radio.readRSSI();
   for(int s = 0; s < 20; s++) {  rssi = radio.readRSSI(); delay(100); } /* take a few samples because RSSI is a gradual thing */
   Serial.print("Current signal strength: ");
   Serial.println(rssi,DEC);
   if(rssi < -100) {
      Serial.println("Signal is clear -- Transmitting");
      radio.setTX(1);
      morse("1ZZ9ZZ/B CN87 ARDUINO HAMSHIELD");
      radio.setTX(0);
      Serial.print("TX Off");
      delay(30000);    
   } else { Serial.println("The channel was busy. Waiting 5 seconds."); delay(5000); } 
}

void morse(char buffer[80]) { 
 Serial.print("Sending in morse ["); 
 for(int x = 0; x < strlen(buffer); x++) {
  Serial.print(buffer[x]);  
  char output = lookup(buffer[x]); 
  if(buffer[x] != ' ') { 
  for(int m = 0; m < strlen(itu[output]); m++) {
     if(itu[output][m] == '-') { tone(9,1000,DOT*3); delay(DOT*3); }
     else { tone(9,1000,DOT); delay(DOT); }
     delay(DOT);
     }
     delay(DOT*3);
  } else { delay(DOT*7); } 
 }
 Serial.println("]");
 return;
}

char lookup(char letter) { 
 for(int x = 0; x < 54; x++) { 
  if(letter == ascii[x]) { 
    return x; 
   // return itu[x];
  } 
 }
}
